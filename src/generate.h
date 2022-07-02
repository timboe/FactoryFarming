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

enum kGroundType{kSiltyGround, kChalkyGround, kPeatyGround, kSandyGround, kPavedGround, kLake, kRiver, kNGroundTypes};

enum kGroundWetness{kWater, kWet, kMoist, kDry, kNGroundWetnesses};

void generate(void);

struct Tile_t* getTile(int32_t _x, int32_t _y);

struct Tile_t* getTile_fromLocation(struct Location_t* _loc);

void renderChunkBackgroundImage(struct Chunk_t* _chunk);

void renderChunkBackgroundImageAround(struct Chunk_t* _chunk);

void setChunkBackgrounds(void);

bool tileIsObstacle(struct Tile_t* _tile);

bool isWaterTile(int32_t _x, int32_t _y);

void addObstacles(void);

void doWetness(void);

enum kGroundWetness getWetness(uint8_t _value);

enum kGroundType getGroundType(uint8_t _tile);

const char* toStringWetness(enum kGroundWetness _wetness);

const char* toStringSoil(enum kGroundType _type);

const char* getWorldName(void);

void setWorldName(const char* _name);

void initWorld(void);

void resetWorld(void);

void serialiseWorld(struct json_encoder* je);

void deserialiseValueWorld(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneWorld(json_decoder* jd, const char* _name, json_value_type _type);

void didDecodeWorldName(json_decoder* jd, const char* _key, json_value _value);