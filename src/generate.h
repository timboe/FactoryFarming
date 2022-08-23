#pragma once
#include "game.h"
#include "chunk.h"

struct Tile_t {
  // Persistent
  uint8_t m_tile;
  uint8_t m_wetness;

  // Transient
};

// Paved must come after the plantable ground types
enum kGroundType{
  kSiltyGround, 
  kChalkyGround, 
  kPeatyGround, 
  kSandyGround, 
  kClayGround, 
  kLoamyGround, 
  kPavedGround, 
  kObstructedGround, 
  kLake, 
  kRiver,
  kOcean, 
  kNGroundTypes
};

enum kWorldType{
  kSiltWorld, 
  kChalkWorld, 
  kPeatWorld, 
  kSandWorld, 
  kClayWorld, 
  kLoamWoarld, 
  kWaterWorld, 
  kTranquilWorld,
  kNWorldTypes
};

extern const uint16_t kWarpUIIcon[];
extern const uint16_t kWarpPrice[];

enum kGroundWetness{kWater, kWet, kMoist, kDry, kNGroundWetnesses};

void generate(uint32_t _actionProgress);

void generateTitle(void);

struct Tile_t* getTile(int32_t _x, int32_t _y);

struct Tile_t* getTile_fromLocation(struct Location_t* _loc);

void renderChunkBackgroundImage(struct Chunk_t* _chunk);

void renderChunkBackgroundImageAround(struct Chunk_t* _chunk);

void setChunkBackgrounds(bool _forTitles);

bool tileIsObstacle(struct Tile_t* _tile);

bool isGroundTile(struct Tile_t* _tile);

bool isWaterTile(int32_t _x, int32_t _y);

bool isGroundTypeTile(int32_t _x, int32_t _y, enum kGroundType);

void addObstacles(void);

void doWetness(bool _forTitles);

bool tryRemoveObstruction(struct Location_t* _loc);

enum kGroundWetness getWetness(uint8_t _value);

enum kGroundType getGroundType(uint8_t _tile);

enum kGroundType getWorldGround(uint8_t _slotNumber, uint8_t _groundCounter);

const char* toStringWetness(enum kGroundWetness _wetness);

const char* toStringSoil(enum kGroundType _type);

const char* getWorldName(enum kWorldType _type, bool _mask);

void setWorldName(const char* _name);

void initWorld(void);

void resetWorld(void);

void serialiseWorld(struct json_encoder* je);

void deserialiseValueWorld(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneWorld(json_decoder* jd, const char* _name, json_value_type _type);

void didDecodeWorldName(json_decoder* jd, const char* _key, json_value _value);