#pragma once
#include "game.h"

void initSprite(void);

void animateConveyor(void);

void setRoobert10(void);

void setRoobert24(void);

LCDFont* getRoobert24(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y, uint8_t _zoom);

LCDBitmap* getSprite16_byidx(uint32_t _idx, uint8_t _zoom);

LCDBitmap* getSprite48(uint32_t _x, uint32_t _y, uint8_t _zoom);

LCDBitmap* getSprite48_byidx(uint32_t _idx, uint8_t _zoom);

LCDBitmap* getConveyorMaster(enum kDir _dir, uint8_t _speed);

// Short-form versions too
#define SID(X, Y) ((SHEET16_SIZE_X * Y) + X)
#define SPRITE16_ID(X, Y) ((SHEET16_SIZE_X * Y) + X)

#define BID(X, Y) ((SHEET48_SIZE_X * Y) + X)
#define SPRITE48_ID(X, Y) ((SHEET48_SIZE_X * Y) + X)
