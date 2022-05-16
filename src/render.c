#include <math.h>
#include "render.h"
#include "generate.h"


void renderFloor(PlaydateAPI* _pd) {
  LCDBitmap* floors[4];
  get4Floor(floors);
  _pd->graphics->drawBitmap(floors[0], SCREEN_X*0, SCREEN_Y*0, kBitmapUnflipped);
  //_pd->graphics->drawBitmap(floors[1], SCREEN_X*1, SCREEN_Y*0, kBitmapUnflipped);
  //_pd->graphics->drawBitmap(floors[2], SCREEN_X*0, SCREEN_Y*1, kBitmapUnflipped);
  //_pd->graphics->drawBitmap(floors[3], SCREEN_X*1, SCREEN_Y*1, kBitmapUnflipped);
}