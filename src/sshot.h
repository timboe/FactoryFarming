#pragma once
#include "game.h"

bool doScreenShot(uint32_t* _actionProgress);

void initMap(void);

LCDSprite* getMap(bool _update);

LCDSprite* getMapPlayer(void);

LCDSprite* getMapEdge(bool _left);

LCDSprite* getMapCursor(void);