#pragma once
#include "game.h"
#include "location.h"


void initSprite(void);

void animateConveyor(void);

void setRoobert10(void);

void setRoobert24(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y, uint8_t _zoom);

LCDBitmap* getConveyorMaster(uint8_t _zoom, enum kConvDir _dir);

