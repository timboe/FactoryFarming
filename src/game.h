#pragma once

#define DEBUG_MODE
#define DEV

#define PRETEND_ZOOMED_IN false

#include "pd_api.h"

#include <stdio.h>
#include <stdbool.h> 

#define TICK_FREQUENCY 32

#define NEAR_TICK_FREQUENCY 2
#define FAR_TICK_FREQUENCY 16

#define NEAR_TICK_AMOUNT 1
#define FAR_TICK_AMOUNT 8

#define TILE_PIX 16

#define DEVICE_PIX_X 400
#define DEVICE_PIX_Y 240

#define SCREEN_PIX_X (DEVICE_PIX_X-TILE_PIX)
#define SCREEN_PIX_Y (DEVICE_PIX_Y-TILE_PIX)

#define CHUNK_PIX_X (SCREEN_PIX_X/2)
#define CHUNK_PIX_Y (SCREEN_PIX_Y/2)

#define CHUNK_NEIGHBORS_CORNER 3
#define CHUNK_NEIGHBORS_ALL 8

#define TILES_PER_CHUNK_X (CHUNK_PIX_X/TILE_PIX)
#define TILES_PER_CHUNK_Y (CHUNK_PIX_Y/TILE_PIX)
#define TILES_PER_CHUNK (TILES_PER_CHUNK_X*TILES_PER_CHUNK_Y)

// 16x16 sprite sheet has how many rows/columns
#define SHEET16_SIZE 8

// Number of available floor spires
#define FLOOR_TILES 8

// How big is the world in X and Y
#define WORLD_CHUNKS_X 4
#define WORLD_CHUNKS_Y 4
#define TOT_CHUNKS (WORLD_CHUNKS_X*WORLD_CHUNKS_Y)

#define TOT_WORLD_PIX_X (WORLD_CHUNKS_X*CHUNK_PIX_X)
#define TOT_WORLD_PIX_Y (WORLD_CHUNKS_Y*CHUNK_PIX_Y)


#define TOT_TILES_X (TILES_PER_CHUNK_X*WORLD_CHUNKS_X)
#define TOT_TILES_Y (TILES_PER_CHUNK_Y*WORLD_CHUNKS_Y)
#define TOT_TILES (TOT_TILES_X*TOT_TILES_Y)

// Player acceleration and friction
#define PLAYER_A 0.75f
#define PLAYER_FRIC 0.8f

#define SQRT_HALF 0.70710678118f

// Fraction of the screen to trigger scrolling
#define SCROLL_EDGE 0.8f

// Conveyor directions
enum kConvDir{SN, NS, EW, WE, kConvDirN};

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};




extern PlaydateAPI* pd;

void setPDPtr(PlaydateAPI* _p);

int gameLoop(void* _data);
void render(void);

int getFrameCount(void);

void initGame(void);
void deinitGame(void);

void gameClickConfigHandler(uint32_t _buttonPressed);
void clickHandlerReplacement(void);

void updateRenderList(void);

uint8_t getZoom(void);

bool movePlayer(void);
