#include "sshot.h"
#include "player.h"
#include "render.h"
#include "chunk.h"
#include "io.h"

SDFile* m_imageFile;
LCDBitmap* m_imageBitmap;

bool m_modeSnap;
int16_t m_chunkX;
int16_t m_chunkY;

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

bool doScreenShot(uint32_t* _actionProgress) {

  bool finished = false;

  if (*_actionProgress == 0) {

    char filePath[64];
    snprintf(filePath, 64, "sshot_%i_%i_%i.bmp", getSave()+1, getSlot()+1, (int) getPlayer()->m_playTime);
    m_imageFile = pd->file->open(filePath, kFileWrite);

    m_imageBitmap = pd->graphics->newBitmap(TOT_WORLD_PIX_X, TOT_WORLD_PIX_Y, kColorWhite);

    chunkResetTorus();

    m_chunkX = 0;
    m_chunkY = 0;
    m_modeSnap = true;
  } else if (m_modeSnap == false) {
    m_modeSnap = true;

    m_chunkX += 2;
    if (m_chunkX >= WORLD_CHUNKS_X) {

      m_chunkX = 0;
      m_chunkY += 2;

      if (m_chunkY >= WORLD_CHUNKS_Y) {
        finished = true;
      }
    }
  
  } else if (m_modeSnap == true) {
    m_modeSnap = false;

    LCDBitmap* frame = pd->graphics->getDisplayBufferBitmap(); // Not owned

    pd->graphics->pushContext(m_imageBitmap);
    pd->graphics->drawBitmap(frame, m_chunkX * CHUNK_PIX_X, m_chunkY * CHUNK_PIX_Y, kBitmapUnflipped);
    pd->graphics->popContext();

  }

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

  if (finished) {
    saveLCDBitmapToFile(m_imageFile, m_imageBitmap);

    pd->file->close(m_imageFile);
    m_imageFile = NULL;

    pd->graphics->freeBitmap(m_imageBitmap);
    m_imageBitmap = NULL;

    setPlayerVisible(true);

    forceTorus();
  }

  (*_actionProgress)++;
  return finished;

}

void saveLCDBitmapToFile(SDFile* _file, LCDBitmap* _bitmap) {
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

  const int32_t count = width * height;
  for (int32_t index = 0; index < count; index++) {
    const int32_t x = index % width;
    const int32_t y = height - (index / width) - 1;

    const int32_t byteIndex = x / bitsInAByte + y * rowBytes;
    const int bitIndex = (1 << (bitsInAByte - 1)) >> (x % bitsInAByte);

    const uint8_t color = data[byteIndex] & bitIndex ? 0xFF : 0x00;
    pd->file->write(_file, &color, sizeof(uint8_t)); // Red
    pd->file->write(_file, &color, sizeof(uint8_t)); // Green
    pd->file->write(_file, &color, sizeof(uint8_t)); // Blue
  }
}