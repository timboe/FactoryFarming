#include "sshot.h"
#include "player.h"
#include "render.h"
#include "chunk.h"
#include "input.h"
#include "io.h"
#include "ui.h"

SDFile* m_imageFile;
LCDBitmap* m_imageBitmap;

bool m_modeSnap;
bool m_doSaving;

bool m_doSnapshots;

int32_t m_pixIndex;

int m_pixWidth, m_pixHeight, m_pixRowBytes;
uint8_t* m_pixData;

int16_t m_chunkX;
int16_t m_chunkY;
uint16_t m_cachePlayerX;
uint16_t m_cachePlayerY;
uint8_t m_cacheZoom;


// Simple BMP class by u/Daeke

typedef struct {
  char magic[2];
  int32_t fileSize;
  int32_t reserved;
  int32_t contentPosition;
} BitmapFileHeader;

typedef struct {
  int32_t headerLength;
  int16_t width;
  int16_t height;
  int16_t planes;
  int16_t bitsPerPixel;
} BitmapCoreHeader;

void saveLCDBitmapHeader(SDFile* _file, LCDBitmap* _bitmap);

bool saveLCDBitmapToFile(SDFile* _file);

/// ///

bool doScreenShot(uint32_t* _actionProgress) {


  if (*_actionProgress == 0) {

    char filePath[64];
    snprintf(filePath, 64, "sshot_%i_%i_%i.bmp", getSave()+1, getSlot()+1, (int) getPlayer()->m_playTime);
    m_imageFile = pd->file->open(filePath, kFileWrite);

    m_imageBitmap = pd->graphics->newBitmap(TOT_WORLD_PIX_X, TOT_WORLD_PIX_Y, kColorWhite);

    chunkResetTorus();

    m_chunkX = 0;
    m_chunkY = 0;

    m_pixIndex = 0;
    m_pixWidth = 0;
    m_pixHeight = 0;
    m_pixRowBytes = 0;
    m_pixData = NULL;

    m_cachePlayerX = getPlayer()->m_pix_x;
    m_cachePlayerY = getPlayer()->m_pix_y;
    m_cacheZoom = getZoom();
    unZoom();
    m_modeSnap = true;
    m_doSnapshots = true;
    m_doSaving = false;

    saveLCDBitmapHeader(m_imageFile, m_imageBitmap);

  } else if (m_doSnapshots && m_modeSnap == false) {
    m_modeSnap = true;

    m_chunkX += 2;
    if (m_chunkX >= WORLD_CHUNKS_X) {

      m_chunkX = 0;
      m_chunkY += 2;

      if (m_chunkY >= WORLD_CHUNKS_Y) {
        m_doSnapshots = false;
        m_doSaving = true;
      }
    }
  
  } else if (m_doSnapshots && m_modeSnap == true) {
    m_modeSnap = false;

    LCDBitmap* frame = pd->graphics->getDisplayBufferBitmap(); // Not owned

    pd->graphics->pushContext(m_imageBitmap);
    pd->graphics->drawBitmap(frame, m_chunkX * CHUNK_PIX_X, m_chunkY * CHUNK_PIX_Y, kBitmapUnflipped);
    pd->graphics->popContext();

  }

  if (m_doSnapshots) {
    uint16_t offX = m_chunkX * CHUNK_PIX_X;
    uint16_t offY = m_chunkY * CHUNK_PIX_Y;

    setPlayerVisible(false);
    setPlayerPosition(offX + CHUNK_PIX_X, offY + CHUNK_PIX_Y, /*updateCurrentLocation*/ true);
    updateRenderList();
    pd->graphics->setDrawOffset(getOffX(), getOffY());
    pd->sprite->drawSprites();

    #ifdef DEV
    pd->system->logToConsole("doScreenShot %i c(%i,%i) camera off:(%i,%i) draw off:(%i,%i)", *_actionProgress, m_chunkX, m_chunkY, offX, offY, getOffX(), getOffY());
    #endif
  }

  bool finished = false;
  if (m_doSaving) {
    pd->sprite->addSprite(getSaveSprite());

    addSaveProgressSprite(m_pixIndex / SSHOT_PIXELS_PER_FRAME, (m_pixWidth * m_pixHeight) / SSHOT_PIXELS_PER_FRAME);

    finished = saveLCDBitmapToFile(m_imageFile);

    if (finished) {
      pd->file->close(m_imageFile);
      m_imageFile = NULL;

      pd->graphics->freeBitmap(m_imageBitmap);
      m_imageBitmap = NULL;

      setPlayerPosition(m_cachePlayerX, m_cachePlayerY, /*updateCurrentLocation*/ true);
      updateRenderList();
      setPlayerVisible(true);
      setZoom(m_cacheZoom);

      forceTorus();
    }
  }

  (*_actionProgress)++;
  return finished;

}

void saveLCDBitmapHeader(SDFile* _file, LCDBitmap* _bitmap) {
  const int32_t fileHeaderLength = 14;
  const int32_t coreHeaderLength = 12;
  const int32_t bitsInAByte = 8;
  const int32_t bytesPerColors = 3;

  int width, height, rowBytes;
  uint8_t *data;
  pd->graphics->getBitmapData(_bitmap, &width, &height, &rowBytes, NULL, &data);

  BitmapFileHeader fileHeader = (BitmapFileHeader) {
    .magic = {'B', 'M'},
    .fileSize = fileHeaderLength + coreHeaderLength + width * height * bytesPerColors,
    .contentPosition = fileHeaderLength + coreHeaderLength
  };
  BitmapCoreHeader coreHeader = (BitmapCoreHeader) {
    .headerLength = coreHeaderLength,
    .width = width,
    .height = height,
    .planes = 1,
    .bitsPerPixel = bitsInAByte * bytesPerColors
  };
  pd->file->write(_file, &fileHeader.magic, sizeof(char[2]));
  pd->file->write(_file, &fileHeader.fileSize, sizeof(int32_t));
  pd->file->write(_file, &fileHeader.reserved, sizeof(int32_t));
  pd->file->write(_file, &fileHeader.contentPosition, sizeof(int32_t));
  pd->file->write(_file, &coreHeader.headerLength, sizeof(int32_t));
  pd->file->write(_file, &coreHeader.width, sizeof(int16_t));
  pd->file->write(_file, &coreHeader.height, sizeof(int16_t));
  pd->file->write(_file, &coreHeader.planes, sizeof(int16_t));
  pd->file->write(_file, &coreHeader.bitsPerPixel, sizeof(int16_t));

  pd->graphics->getBitmapData(_bitmap, &m_pixWidth, &m_pixHeight, &m_pixRowBytes, NULL, &m_pixData);
}


bool saveLCDBitmapToFile(SDFile* _file) {
  const int32_t bitsInAByte = 8;

  const int32_t count = m_pixWidth * m_pixHeight;
  int32_t localCount = 0;

  while (true) {
    const int32_t x = m_pixIndex % m_pixWidth;
    const int32_t y = m_pixHeight - (m_pixIndex / m_pixWidth) - 1;

    const int32_t byteIndex = x / bitsInAByte + y * m_pixRowBytes;
    const int bitIndex = (1 << (bitsInAByte - 1)) >> (x % bitsInAByte);

    const uint8_t color = m_pixData[byteIndex] & bitIndex ? 0xFF : 0x00;
    pd->file->write(_file, &color, sizeof(uint8_t)); // Red
    pd->file->write(_file, &color, sizeof(uint8_t)); // Green
    pd->file->write(_file, &color, sizeof(uint8_t)); // Blue

    if (++m_pixIndex == count) return true; 
    if (++localCount == SSHOT_PIXELS_PER_FRAME) return false;
  }

  return false;
}