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

void deserialiseValueWorld(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneWorld(json_decoder* jd, const char* _name, json_value_type _type);