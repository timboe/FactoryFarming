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

// 16x16 sprite sheet has how many rows/columns
#define SHEET16_SIZE 8

// Number of available floor spires
#define FLOOR_TILES 8

// How big is the world in X and Y
#define WORLD_SCREENS_X 4
#define WORLD_SCREENS_Y 4

#define TOT_WORLD_X WORLD_SCREENS_X*SCREEN_X
#define TOT_WORLD_Y WORLD_SCREENS_Y*SCREEN_Y


#define TOT_TILES_X TILES_X*WORLD_SCREENS_X
#define TOT_TILES_Y TILES_Y*WORLD_SCREENS_Y
#define TOT_TILES TOT_TILES_X*TOT_TILES_Y

// Player acceleration and friction
#define PLAYER_A 0.75f
#define PLAYER_FRIC 0.8f

#define SQRT_HALF 0.70710678118f

// Fraction of the screen to trigger scrolling
#define SCROLL_EDGE 0.8f

// Conveyor directions
enum kConvDir{SN, NS, EW, WE, SE, SW, NE, NW, EN, ES, WN, WS, kConvDirN};

extern PlaydateAPI* pd;

void setPDPtr(PlaydateAPI* _p);

int gameLoop(void* _data);
void render(void);

int getFrameCount(void);

void initGame(void);
void deinitGame(void);

void gameClickConfigHandler(uint32_t _buttonPressed);
void clickHandlerReplacement(void);

uint8_t getZoom(void);

bool movePlayer(void);
