#pragma once
#include "game.h"
#include "sprite.h"
#include "chunk.h"

struct Tile_t {
  // Persistent
  uint8_t m_tile;
  uint8_t m_wetness;

  // Transient
};

void generate(void);

struct Tile_t* getTile(int32_t _x, int32_t _y);

struct Tile_t* getTile_fromLocation(struct Location_t* _loc);

void renderChunkBackgroundImage(struct Chunk_t* _chunk);

void renderChunkBackgroundImageAround(struct Chunk_t* _chunk);

void setChunkBackgrounds(void);

bool tileIsObstacle(struct Tile_t* _tile);

void addObstacles(void);

void doWetness(void);

const char* toStringWetness(struct Tile_t* _tile);

const char* toStringSoil(struct Tile_t* _tile);

const char* getWorldName(void);

void setWorldName(const char* _name);

void initWorld(void);

void resetWorld(void);

void serialiseWorld(struct json_encoder* je);

void deserialiseValueWorld(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneWorld(json_decoder* jd, const char* _name, json_value_type _type);

void didDecodeWorldName(json_decoder* jd, const char* _key, json_value _value);