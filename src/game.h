#pragma once

#define DEBUG_MODE
#define DEV

#include "pd_api.h"

#include <stdio.h>
#include <stdbool.h> 

#define ANIM_FPS 20

#define SCREEN_X 400
#define SCREEN_Y 240

#define TILE_SIZE 16

#define TILES_X SCREEN_X/TILE_SIZE
#define TILES_Y SCREEN_Y/TILE_SIZE


void setPDPtr(PlaydateAPI* _p);

int gameLoop(void* _data);
void render(void);

int getFrameCount(void);

void initGame(void);
void deinitGame(void);

void gameClickConfigHandler(uint32_t _buttonPressed);
void clickHandlerReplacement(void);

bool movePlayer(void);
