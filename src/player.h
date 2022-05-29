#pragma once
#include "game.h"

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};

struct Player_t{
  LCDSprite* m_sprite[5]; // Only locations 1, 2 and 4 are used
  LCDSprite* m_blueprint[5]; // Only locations 1, 2 and 4 are used
  float m_pix_x; // Note: Centre
  float m_pix_y;
  float m_vX;
  float m_vY;
};

struct Player_t* getPlayer(void);

int16_t getOffX(void);

int16_t getOffY(void);

struct Chunk_t* getCurrentChunk(void);

enum kChunkQuad getCurrentQuadrant(void);

struct Location_t* getPlayerLocation(void);

struct Location_t* getPlayerLookingAtLocation(void);

bool movePlayer(void);

void initPlayer(void);

