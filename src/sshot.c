#include "sshot.h"
#include "player.h"
#include "render.h"
#include "chunk.h"
#include "input.h"
#include "io.h"
#include "ui.h"
#include "sprite.h"

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

LCDBitmap* m_map = NULL;
LCDBitmap* m_mapP = NULL;
LCDBitmap* m_mapC = NULL;
LCDSprite* m_mapSprite = NULL;
LCDSprite* m_mapPSprite = NULL;
LCDSprite* m_mapCSprite = NULL;
LCDSprite* m_mapEdge[2] = {NULL};

LCDBitmap* m_7x7 = NULL;
LCDBitmap* m_9x9 = NULL;


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

void updateMap(void);

/// ///

void initMap() {
  m_map = pd->graphics->newBitmap(TOT_TILES_X * MAP_PIX_PER_TILE, TOT_TILES_Y * MAP_PIX_PER_TILE, kColorClear);
  m_mapSprite = pd->sprite->newSprite();

  PDRect mapBound = {.x = 0, .y = 0, .width = TOT_TILES_X * MAP_PIX_PER_TILE, .height = TOT_TILES_Y * MAP_PIX_PER_TILE};
  pd->sprite->setBounds(m_mapSprite, mapBound);
  pd->sprite->setImage(m_mapSprite, m_map, kBitmapUnflipped);
  pd->sprite->moveTo(m_mapSprite, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);
  pd->sprite->setZIndex(m_mapSprite, Z_INDEX_UI_B);
  pd->sprite->setIgnoresDrawOffset(m_mapSprite, 1);

  PDRect smallBound = {.x = 0, .y = 0, .width = 3 * MAP_PIX_PER_TILE, .height = 3 * MAP_PIX_PER_TILE};

  m_mapP = pd->graphics->newBitmap(3 * MAP_PIX_PER_TILE, 3 * MAP_PIX_PER_TILE, kColorClear);
  m_mapPSprite = pd->sprite->newSprite();
  pd->sprite->setBounds(m_mapPSprite, smallBound);
  pd->sprite->setImage(m_mapPSprite, m_mapP, kBitmapUnflipped);
  pd->sprite->setZIndex(m_mapPSprite, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_mapPSprite, 1);
  pd->graphics->pushContext(m_mapP);
  pd->graphics->drawBitmap(getSprite6(5), 0, 0, kBitmapUnflipped);
  pd->graphics->popContext();

  m_mapC = pd->graphics->newBitmap(3 * MAP_PIX_PER_TILE, 3 * MAP_PIX_PER_TILE, kColorClear);
  m_mapCSprite = pd->sprite->newSprite();
  pd->sprite->setBounds(m_mapCSprite, smallBound);
  pd->sprite->setImage(m_mapCSprite, m_mapC, kBitmapUnflipped);
  pd->sprite->setZIndex(m_mapCSprite, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_mapCSprite, 1);
  pd->graphics->pushContext(m_mapC);
  pd->graphics->drawBitmap(getSprite6(6), 0, 0, kBitmapUnflipped);
  pd->graphics->popContext();

  PDRect edgeBound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TOT_TILES_Y * MAP_PIX_PER_TILE};
  m_mapEdge[0] = pd->sprite->newSprite();
  pd->sprite->setBounds(m_mapEdge[0], edgeBound);
  pd->sprite->setImage(m_mapEdge[0], getSpriteParchment(true), kBitmapUnflipped);
  pd->sprite->setZIndex(m_mapEdge[0], Z_INDEX_UI_B);
  pd->sprite->setIgnoresDrawOffset(m_mapEdge[0], 1);
  pd->sprite->moveTo(m_mapEdge[0], 48 - 8, SCREEN_PIX_Y/2);
  //
  m_mapEdge[1] = pd->sprite->newSprite();
  pd->sprite->setBounds(m_mapEdge[1], edgeBound);
  pd->sprite->setImage(m_mapEdge[1], getSpriteParchment(false), kBitmapUnflipped);
  pd->sprite->setZIndex(m_mapEdge[1], Z_INDEX_UI_B);
  pd->sprite->setIgnoresDrawOffset(m_mapEdge[1], 1);
  pd->sprite->moveTo(m_mapEdge[1], 352 - 8, SCREEN_PIX_Y/2);

  m_7x7 = pd->graphics->newBitmap(7 * MAP_PIX_PER_TILE, 7 * MAP_PIX_PER_TILE, kColorClear);
  m_9x9 = pd->graphics->newBitmap(9 * MAP_PIX_PER_TILE, 9 * MAP_PIX_PER_TILE, kColorClear);
  pd->graphics->pushContext(m_7x7);
  pd->graphics->drawRect(0, 0, 7 * MAP_PIX_PER_TILE, 7 * MAP_PIX_PER_TILE, kColorBlack);
  pd->graphics->popContext();
  pd->graphics->pushContext(m_9x9);
  pd->graphics->drawRect(0, 0, 9 * MAP_PIX_PER_TILE, 9 * MAP_PIX_PER_TILE, kColorBlack);
  pd->graphics->popContext();
}

#define M_BLANK 0
#define M_DIAG_R 1
#define M_HORIZ 2
#define M_CORNER 3
#define M_VERT 4
#define M_3CORNER 5
#define M_DIAG_L 6
#define M_FULL 7

void updateMap() {
  const int16_t offX = (SCREEN_PIX_X/2) - ((TOT_TILES_X*MAP_PIX_PER_TILE)/2);
  const int16_t offY = (SCREEN_PIX_Y/2) - ((TOT_TILES_Y*MAP_PIX_PER_TILE)/2);
  const struct Location_t* _loc = getPlayerLocation();
  pd->sprite->moveTo(m_mapPSprite, offX + ((_loc->m_x+1)*MAP_PIX_PER_TILE), offY + ((_loc->m_y+1)*MAP_PIX_PER_TILE));

  pd->graphics->clearBitmap(m_map, kColorWhite);
  pd->graphics->pushContext(m_map);
  pd->graphics->setDrawMode(kDrawModeCopy);
  // Ground
  for (uint16_t _x = 0; _x < TOT_TILES_X; ++_x) {
    for (uint16_t _y = 0; _y < TOT_TILES_Y; ++_y) {
      const struct Tile_t* _t = getTile(_x, _y);
      uint8_t _i = 0;
      // Regular or patch?
      const bool _mainGroundType = (getWorldGround(getSlot(), 0) == _t->m_groundType);
      switch (_t->m_groundType) {
        case kNGroundTypes: case kSiltyGround: case kSandyGround: 
        case kChalkyGround: case kPeatyGround: case kClayGround: 
        case kLoamyGround: _i = (_mainGroundType ? M_BLANK : M_CORNER); break;
        case kPavedGround: _i = M_DIAG_L; break;
        case kObstructedGround: _i = M_VERT; break;
        case kLake: case kRiver: case kOcean: _i = M_3CORNER; 
      }
      pd->graphics->drawBitmap(getSprite2(_i), _x * MAP_PIX_PER_TILE, _y * MAP_PIX_PER_TILE, kBitmapUnflipped);
    }
  }
  // Buildings
  for (uint16_t _x = 0; _x < TOT_TILES_X; ++_x) {
    for (uint16_t _y = 0; _y < TOT_TILES_Y; ++_y) {
      const struct Location_t* _l = getLocation(_x, _y);
      struct Building_t* _b = NULL;
      if (_l->m_building && _l->m_notOwned == false) _b = _l->m_building;
      if (!_b) continue;
      const enum kBuildingType _bt = _b->m_type; 
      if (_bt == kConveyor) {
        pd->graphics->drawBitmap(getSprite2(M_FULL), _x * MAP_PIX_PER_TILE, _y * MAP_PIX_PER_TILE, kBitmapUnflipped);
      } else if (_bt == kSpecial || (_bt == kUtility && _b->m_subType.utility == kRetirement)) {
        pd->graphics->drawBitmap(getSprite6(4), (_x-1) * MAP_PIX_PER_TILE, (_y-1) * MAP_PIX_PER_TILE, kBitmapUnflipped);
      } else if (_bt == kPlant) {
        pd->graphics->drawBitmap(getSprite2(M_HORIZ), _x * MAP_PIX_PER_TILE, _y * MAP_PIX_PER_TILE, kBitmapUnflipped);
      } else if (_bt == kUtility) {
        pd->graphics->drawBitmap(getSprite2(M_DIAG_R), _x * MAP_PIX_PER_TILE, _y * MAP_PIX_PER_TILE, kBitmapUnflipped);
      } else if (_bt == kFactory || _bt == kExtractor) {
        pd->graphics->drawBitmap(getSprite6((uint32_t)_b->m_dir), (_x-1) * MAP_PIX_PER_TILE, (_y-1) * MAP_PIX_PER_TILE, kBitmapUnflipped);
      }
    }
  }
  // Extractor boundaries
  for (uint16_t _x = 0; _x < TOT_TILES_X; ++_x) {
    for (uint16_t _y = 0; _y < TOT_TILES_Y; ++_y) {
      const struct Location_t* _l = getLocation(_x, _y);
      struct Building_t* _b = NULL;
      if (_l->m_building && _l->m_notOwned == false && _l->m_building->m_type == kExtractor) _b = _l->m_building;
      if (!_b) continue;
      if (_b->m_subType.extractor == kCropHarvesterSmall) {
        pd->graphics->drawBitmap(m_7x7, (_x-3) * MAP_PIX_PER_TILE, (_y-3) * MAP_PIX_PER_TILE, kBitmapUnflipped);
      } else if (_b->m_subType.extractor == kCropHarvesterLarge) {
        pd->graphics->drawBitmap(m_9x9, (_x-4) * MAP_PIX_PER_TILE, (_y-4) * MAP_PIX_PER_TILE, kBitmapUnflipped);
      }
    }
  }
  pd->graphics->popContext();
}

LCDSprite* getMap(bool _update) {
  if (_update) updateMap();
  return m_mapSprite;
}

LCDSprite* getMapPlayer() { 
  return m_mapPSprite;
}

LCDSprite* getMapCursor() {
  return m_mapCSprite;
}

LCDSprite* getMapEdge(bool _left) {
  return m_mapEdge[ _left ? 0 : 1 ];
}

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
    const uint16_t offX = m_chunkX * CHUNK_PIX_X;
    const uint16_t offY = m_chunkY * CHUNK_PIX_Y;

    struct Player_t* p = getPlayer();

    setPlayerVisible(false);
    setPlayerPosition(offX + CHUNK_PIX_X, offY + CHUNK_PIX_Y, /*updateCurrentLocation*/ true);
    updateRenderList();

    // We're zoomed out here
    const int16_t drawOffX = -(p->m_pix_x - (SCREEN_PIX_X/2));
    const int16_t drawOffY = -(p->m_pix_y - (SCREEN_PIX_Y/2));

    pd->graphics->setDrawOffset(drawOffX, drawOffY);
    pd->sprite->drawSprites();

    #ifdef DEV
    pd->system->logToConsole("doScreenShot %i c(%i,%i) camera off:(%i,%i) draw off:(%i,%i)", *_actionProgress, m_chunkX, m_chunkY, offX, offY, drawOffX, drawOffY);
    #endif
  }

  bool finished = false;
  if (m_doSaving) {
    pd->sprite->addSprite(getSaveSprite());

    addSaveLoadProgressSprite(m_pixIndex / SSHOT_PIXELS_PER_FRAME, (m_pixWidth * m_pixHeight) / SSHOT_PIXELS_PER_FRAME);

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