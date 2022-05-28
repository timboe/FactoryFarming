#pragma once
#include "game.h"

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};

struct Player_t{
  LCDSprite* m_sprite;
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

bool movePlayer(void);

void initPlayer(void);

