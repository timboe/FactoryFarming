#pragma once
#include "game.h"
#include "chunk.h"

struct Tile_t {
  // Persistent
  uint8_t m_tile;

  // Transient
  uint8_t m_wetness;
  uint8_t m_groundType; // Note: Not using 'enum kGroundType' to save bytes

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

struct WorldDescriptor_t {
  enum kWorldType subType;
  uint32_t price;
  uint16_t UIIcon;
  enum kCargoType unlock;
};
extern struct WorldDescriptor_t WDesc[];

enum kGroundWetness{kWater, kWet, kMoist, kDry, kNGroundWetnesses};

void generate(uint32_t _actionProgress);

void generateTitle(void);

int32_t getTile_idx(int32_t _x, int32_t _y);

struct Tile_t* getTile(int32_t _x, int32_t _y);

struct Tile_t* getTile_fromLocation(struct Location_t* _loc);

void setTile(uint16_t _i, uint8_t _tileValue);

void setTile_ptr(struct Tile_t* _tile, uint8_t _tileValue);

void renderChunkBackgroundImage(struct Chunk_t* _chunk);

void renderChunkBackgroundImageAround(struct Chunk_t* _chunk);

void setChunkBackgrounds(bool _forTitles);

bool isGroundTile(struct Tile_t* _tile);

bool isWaterTile(int32_t _x, int32_t _y);

bool isGroundTypeTile(int32_t _x, int32_t _y, enum kGroundType);

void addObstacles(void);

void doWetness(bool _forTitles);

bool tryRemoveObstruction(struct Location_t* _loc);

enum kGroundWetness getWetness(uint8_t _value);

enum kGroundType getWorldGround(uint8_t _slotNumber, uint8_t _groundCounter);

const char* toStringWetness(enum kGroundWetness _wetness);

const char* toStringSoil(enum kGroundType _type);

const char* getWorldName(enum kWorldType _type, bool _mask);

void setWorldName(const char* _name);

void initWorld(void);

void resetWorld(void);

void serialiseWorld(struct json_encoder* je);

void deserialiseArrayValueWorld(json_decoder* jd, int _pos, json_value _value);