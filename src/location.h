#pragma once
#include "game.h"
#include "cargo.h"



struct Location_t{
  struct Chunk_t* m_chunk; // My chunk
  struct Cargo_t* m_cargo;
  struct Building_t* m_building;
  uint16_t m_x; // In location coords
  uint16_t m_y;
};


struct Location_t* getLocation(int32_t _x, int32_t _y);

struct Location_t* getLocation_noCheck(int32_t _x, int32_t _y);

void clearLocation(struct Location_t* _loc);

void initLocation(void);