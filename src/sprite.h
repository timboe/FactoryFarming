#pragma once
#include "game.h"
#include "location.h"


void initSprite(void);

void animateConveyor(void);

void setRoobert10(void);

void setRoobert24(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y);

LCDBitmap* getSprite16_x2(uint32_t _x, uint32_t _y);

LCDBitmap* getSprite16_x4(uint32_t _x, uint32_t _y);

LCDBitmap* getConveyorMaster(enum kConvDir _dir);

