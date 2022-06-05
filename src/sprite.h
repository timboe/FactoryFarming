#pragma once
#include "game.h"
#include "building.h"

void initSprite(void);

void animateConveyor(void);

void setRoobert10(void);

void setRoobert24(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y, uint8_t _zoom);

LCDBitmap* getSprite16_byidx(uint32_t _idx, uint8_t _zoom);

uint16_t getSprite16_idx(uint32_t _x, uint32_t _y);

LCDBitmap* getSprite48(uint32_t _x, uint32_t _y, uint8_t _zoom);

LCDBitmap* getSprite48_byidx(uint32_t _idx, uint8_t _zoom);

uint16_t getSprite48_idx(uint32_t _x, uint32_t _y);

LCDBitmap* getConveyorMaster(uint8_t _zoom, enum kDir _dir);

