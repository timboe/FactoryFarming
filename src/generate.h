#pragma once
#include "game.h"
#include "sprite.h"
#include "chunk.h"

struct Tile_t {
  // Persistent
  uint8_t m_tile;

  // Transient
};

void generate(void);

void renderChunkBackgroundImage(struct Chunk_t* _chunk);

void initWorld(void);

void resetWorld(void);

void serialiseWorld(struct json_encoder* je);