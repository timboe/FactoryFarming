#pragma once
#include "game.h"
#include "building.h"
#include "cargo.h"

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};

struct Player_t{
  // Persistent
  float m_pix_x; // Note: Centre
  float m_pix_y;
  uint32_t m_money;
  uint16_t m_carryCargo[kNCargoType];
  uint16_t m_carryConveyor[kNConvSubTypes];
  uint16_t m_carryPlant[kNPlantSubTypes];
  uint16_t m_carryExtractor[kNExtractorSubTypes];
  uint16_t m_carryFactory[kNFactorySubTypes];

  // Transient   
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDSprite* m_blueprint[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDSprite* m_blueprintRadius[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap3x3[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap7x7[ZOOM_LEVELS]; // Index 0 not used - OWNED
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

void setPlayerLookingAtOffset(int8_t _offset);

bool movePlayer(void);

void initPlayer(void);

void resetPlayer(void);

void serialisePlayer(struct json_encoder* je); 

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type);

void deserialiseArrayValuePlayer(json_decoder* jd, int _pos, json_value _value);