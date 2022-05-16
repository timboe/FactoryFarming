#include "sprite.h"

LCDBitmap* m_a;

#define SHEET16_SIZE 8

LCDBitmapTable* m_sheet16;

LCDSprite* m_player;

LCDFont* m_fontRoobert24;

LCDBitmap* loadImageAtPath(PlaydateAPI* _pd, const char* _path) {
  const char* _outErr = NULL;
  LCDBitmap* _img = _pd->graphics->loadBitmap(_path, &_outErr);
  if (_outErr != NULL) {
    _pd->system->error("Error loading image at path '%s': %s", _path, _outErr);
  }
  return _img;
}

LCDBitmapTable* loadImageTableAtPath(PlaydateAPI* _pd, const char* _path) {
  const char* _outErr = NULL;
  LCDBitmapTable* _table = _pd->graphics->loadBitmapTable(_path, &_outErr);
  if (_outErr != NULL) {
    _pd->system->error("Error loading image table at path '%s': %s", _path, _outErr);
  }
  return _table;
}

LCDFont* loadFontAtPath(PlaydateAPI* _pd, const char* _path) {
  const char* _outErr = NULL;
  LCDFont* _f = _pd->graphics->loadFont(_path, &_outErr);
  if (_outErr != NULL) {
    _pd->system->error("Error loading font at path '%s': %s", _path, _outErr);
  }
  return _f;
}

LCDBitmap* getSprite16(PlaydateAPI* _pd, uint32_t _x, uint32_t _y) {
  return _pd->graphics->getTableBitmap(m_sheet16, (SHEET16_SIZE * _y) + _x);
}
 
LCDSprite* getPlayer() {
  return m_player;
}

void initSprite(PlaydateAPI* _pd) {
  //m_ = loadImageAtPath(_pd, "images/");
  m_sheet16 = loadImageTableAtPath(_pd, "images/sheet16");


  m_fontRoobert24 = loadFontAtPath(_pd, "fonts/Roobert-24-Medium");
  _pd->graphics->setFont(m_fontRoobert24);

  m_player = _pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = TILE_SIZE, .height = TILE_SIZE};
  _pd->sprite->setBounds(m_player, bound);
  _pd->sprite->setImage(m_player, getSprite16(_pd, 0, 1), kBitmapUnflipped);
  _pd->sprite->moveTo(m_player, SCREEN_X/2.0f, SCREEN_Y/2.0f);
  _pd->sprite->addSprite(m_player);
}

void deinitSprite() {
}
