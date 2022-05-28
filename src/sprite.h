#pragma once
#include "game.h"
#include "location.h"


void initSprite(void);

void animateConveyor(void);

void setRoobert10(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y);

LCDBitmap* getConveyorMaster(enum kConvDir _dir);

