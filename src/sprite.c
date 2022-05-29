#include "sprite.h"

LCDBitmap* m_conveyorMasters[kConvDirN] = {NULL};

LCDBitmapTable* m_sheet16;

LCDBitmap* m_bitmap16_x2[SHEET16_SIZE];

LCDBitmap* m_bitmap16_x4[SHEET16_SIZE];

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

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y) {
  return pd->graphics->getTableBitmap(m_sheet16, (SHEET16_SIZE_X * _y) + _x);
}

LCDBitmap* getSprite16_x2(uint32_t _x, uint32_t _y) {
  return m_bitmap16_x2[(SHEET16_SIZE_X * _y) + _x];
}

LCDBitmap* getSprite16_x4(uint32_t _x, uint32_t _y) {
  return m_bitmap16_x4[(SHEET16_SIZE_X * _y) + _x];
}

void animateConveyor() {
  static int8_t tick = 0;
  tick = (tick + 1) % 8;

  pd->graphics->setDrawMode(kDrawModeCopy);
  for (int32_t i = 0; i < kConvDirN; ++i) {
    pd->graphics->pushContext(m_conveyorMasters[i]);
    pd->graphics->drawBitmap(getSprite16(tick, CONV_START_Y + i), 0, 0, kBitmapUnflipped);
    pd->graphics->popContext();
  }

}

LCDBitmap* getConveyorMaster(enum kConvDir _dir) {
  return m_conveyorMasters[_dir];
}

void setRoobert10() {
  pd->graphics->setFont(m_fontRoobert10);
}

void setRoobert24() {
  pd->graphics->setFont(m_fontRoobert24);
}

void populateResizedSprites() {
  for (uint32_t i = 0; i < SHEET16_SIZE; ++i) {
    LCDBitmap* original = pd->graphics->getTableBitmap(m_sheet16, i);
    m_bitmap16_x2[i] = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorWhite);
    m_bitmap16_x4[i] = pd->graphics->newBitmap(TILE_PIX*4, TILE_PIX*4, kColorWhite);
    pd->graphics->pushContext(m_bitmap16_x2[i]);
    pd->graphics->drawScaledBitmap(original, 0, 0, 2.0f, 2.0f);
    pd->graphics->popContext();
    pd->graphics->pushContext(m_bitmap16_x4[i]);
    pd->graphics->drawScaledBitmap(original, 0, 0, 4.0f, 4.0f);
    pd->graphics->popContext();
  }

}

void initSprite() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  m_sheet16 = loadImageTableAtPath("images/sheet16");
  populateResizedSprites();

  for (int32_t i = 0; i < kConvDirN; ++i) {
    m_conveyorMasters[i] = pd->graphics->copyBitmap(getSprite16(0, CONV_START_Y + i));
  }

  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert10 = loadFontAtPath("fonts/Roobert-10-Bold");
  pd->graphics->setFont(m_fontRoobert24);

}
