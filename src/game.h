#pragma once

#define DEBUG_MODE
//#define DEV

#include "pd_api.h"

#include <stdio.h>
#include <stdbool.h> 

#define ANIM_FPS 20

void setPDPtr(PlaydateAPI* _p);

int gameLoop(void* _data);
void render(void);

int getFrameCount(void);

void initGame(void);
void deinitGame(void);

void gameClickConfigHandler(uint32_t _buttonPressed);
void clickHandlerReplacement(void);

bool movePlayer(void);
