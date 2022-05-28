#include "sprite.h"


LCDBitmap* m_conveyorMasters[kConvDirN] = {NULL};

LCDBitmapTable* m_sheet16;

LCDFont* m_fontRoobert24;
LCDFont* m_fontRoobert10;

LCDBitmap* loadImageAtPath(const char* _path);

LCDBitmapTable* loadImageTableAtPath(const char* _path);

LCDFont* loadFontAtPath(const char* _path);

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
  return pd->graphics->getTableBitmap(m_sheet16, (SHEET16_SIZE * _y) + _x);
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

void initSprite() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  m_sheet16 = loadImageTableAtPath("images/sheet16");

  for (int32_t i = 0; i < kConvDirN; ++i) {
    m_conveyorMasters[i] = pd->graphics->copyBitmap(getSprite16(0, CONV_START_Y + i));
  }

  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert10 = loadFontAtPath("fonts/Roobert-10-Bold");
  pd->graphics->setFont(m_fontRoobert24);

}
