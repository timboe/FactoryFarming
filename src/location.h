#pragma once
#include "game.h"
#include "cargo.h"

struct Location_t{
  // Persistent
  // Chunk index - obtained from chunk
  // Building index - obtained from building
  bool m_notOwned; // For when there is a building covering this location, but this is not the primary location

  // Transient
  struct Cargo_t* m_cargo;
  struct Building_t* m_building;
  LCDSprite* m_obstacle; // Non-owning. Any obstacle sprite (only a reference to zoom level 1 is kept) 
  // Populated at RESET
  struct Chunk_t* m_chunk;
  uint16_t m_x; // In location coords
  uint16_t m_y;
  int16_t m_pix_off_x; // Pixel offset
  int16_t m_pix_off_y; // Pixel offset
  struct Location_t* m_desiredToMoveFrom; // Location which desired to move here
};

struct Location_t* getLocation(int32_t _x, int32_t _y);

struct Location_t* getLocation_noCheck(int32_t _x, int32_t _y);

bool clearLocation(struct Location_t* _loc, bool _clearCargo, bool _clearBuilding);

void initLocation(void);

void resetLocation(void);

void serialiseLocation(struct json_encoder* je);

void deserialiseValueLocation(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneLocation(json_decoder* jd, const char* _name, json_value_type _type);