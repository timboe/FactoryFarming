#include "sprite.h"

LCDBitmap* m_a;
LCDBitmapTable* m_b;

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
 
void initSprite(PlaydateAPI* _pd) {
  //m_ = loadImageAtPath(_pd, "images/");
  //m_= loadImageTableAtPath(_pd, "images/");


}

void deinitSprite() {
}
