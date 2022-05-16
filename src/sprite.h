#pragma once
#include "game.h"


void initSprite(PlaydateAPI* _pd);
void deinitSprite(void);

LCDSprite* getPlayer(void);

LCDBitmap* getSprite16(PlaydateAPI* _pd, uint32_t _x, uint32_t _y);

