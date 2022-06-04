#pragma once
#include "game.h"
#include "cargo.h"



struct Location_t{
  // Persistent
  // Chunk index - obtained from chunk
  // Building index - obtained from building

  // Transient
  struct Cargo_t* m_cargo;
  struct Building_t* m_building;
  struct Chunk_t* m_chunk; // My chunk
  uint16_t m_x; // In location coords
  uint16_t m_y;
};


struct Location_t* getLocation(int32_t _x, int32_t _y);

struct Location_t* getLocation_noCheck(int32_t _x, int32_t _y);

void clearLocation(struct Location_t* _loc);

void initLocation(void);

void resetLocation(void);

void serialiseLocation(struct json_encoder* je);