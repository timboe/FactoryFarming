#include "sprite.h"
#include "input.h"

LCDBitmap* m_conveyorMasters[ZOOM_LEVELS][kDirN] = {NULL}; // Only 1, 2 and 4 are used

LCDBitmapTable* m_sheet16;

LCDBitmap* m_bitmap16_zoom[ZOOM_LEVELS][SHEET16_SIZE];

LCDFont* m_fontRoobert24;

LCDFont* m_fontRoobert10;

LCDBitmap* loadImageAtPath(const char* _path);

LCDBitmapTable* loadImageTableAtPath(const char* _path);

LCDFont* loadFontAtPath(const char* _path);

void populateResizedSprites(void);

/// ///

LCDBitmap* loadImageAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDBitmap* _img = pd->graphics->loadBitmap(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading image at path '%s': %s", _path, _outErr);
  }
  return _img;
}

LCDBitmapTable* loadImageTableAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDBitmapTable* _table = pd->graphics->loadBitmapTable(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading image table at path '%s': %s", _path, _outErr);
  }
  return _table;
}

LCDFont* loadFontAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDFont* _f = pd->graphics->loadFont(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading font at path '%s': %s", _path, _outErr);
  }
  return _f;
}

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y, uint8_t _zoom) {
  if (_zoom == 1) {
    return pd->graphics->getTableBitmap(m_sheet16, getSprite16_idx(_x, _y));
  } else if (_zoom < ZOOM_LEVELS) {
    return m_bitmap16_zoom[_zoom][getSprite16_idx(_x, _y)];
  }
  pd->system->error("getSprite16 Error");
  return NULL;
}

uint16_t getSprite16_idx(uint32_t _x, uint32_t _y) {
  return (SHEET16_SIZE_X * _y) + _x;
}

void animateConveyor() {
  static int8_t tick = 0;
  tick = (tick + 1) % 8;
  uint8_t zoom = getZoom();

  pd->graphics->setDrawMode(kDrawModeCopy);
  for (int32_t i = 0; i < kDirN; ++i) {
    pd->graphics->pushContext(m_conveyorMasters[zoom][i]);
    pd->graphics->drawBitmap(getSprite16(tick, CONV_START_Y + i, zoom), 0, 0, kBitmapUnflipped);
    pd->graphics->popContext();
  }

}

LCDBitmap* getConveyorMaster(uint8_t _zoom, enum kDir _dir) {
  return m_conveyorMasters[_zoom][_dir];
}

void setRoobert10() {
  pd->graphics->setFont(m_fontRoobert10);
}

void setRoobert24() {
  pd->graphics->setFont(m_fontRoobert24);
}

void populateResizedSprites() {
  for (uint32_t i = 0; i < SHEET16_SIZE; ++i) {
    for (uint32_t zoom = 2; zoom < ZOOM_LEVELS; ++zoom) {
      LCDBitmap* original = pd->graphics->getTableBitmap(m_sheet16, i);
      m_bitmap16_zoom[zoom][i] = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorClear);
      pd->graphics->pushContext(m_bitmap16_zoom[zoom][i]);
      pd->graphics->drawScaledBitmap(original, 0, 0, zoom, zoom);
      pd->graphics->popContext();
    }
  }
}

void initSprite() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  m_sheet16 = loadImageTableAtPath("images/sheet16");
  populateResizedSprites();

  for (int32_t i = 0; i < kDirN; ++i) {
    for (int32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      m_conveyorMasters[zoom][i] = pd->graphics->copyBitmap(getSprite16(0, CONV_START_Y + i, zoom));
    }
  }

  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert10 = loadFontAtPath("fonts/Roobert-10-Bold");
  pd->graphics->setFont(m_fontRoobert24);

}
