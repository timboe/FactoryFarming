#pragma once
#include "game.h"
#include "cargo.h"
#include "location.h"

struct Chunk_t{
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In chunk coords
  uint16_t m_y;

  LCDSprite* m_bkgSprite[5]; // Only using indicies 1, 2, 4
  LCDBitmap* m_bkgImage[5];  // Only using indicies 1, 2, 4

  struct Chunk_t* m_neighborsNE[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsSE[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsSW[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsNW[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsALL[CHUNK_NEIGHBORS_ALL];

  struct Chunk_t* m_nonNeighborsNE[TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1]; // The "- 1" is for "me"
  struct Chunk_t* m_nonNeighborsSE[TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1];
  struct Chunk_t* m_nonNeighborsSW[TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1];
  struct Chunk_t* m_nonNeighborsNW[TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1];
  struct Chunk_t* m_nonNeighborsALL[TOT_CHUNKS - CHUNK_NEIGHBORS_ALL - 1];

  uint16_t m_nLocations;
  struct Location_t* m_locations[TILES_PER_CHUNK];

  uint16_t m_nCargos;
  struct Cargo_t* m_cargos[TILES_PER_CHUNK];
};

struct Chunk_t* getChunk(int32_t _x, int32_t _y);

struct Chunk_t* getChunk_noCheck(const int32_t _x, const int32_t _y);

struct Chunk_t* getChunk_fromLocation(struct Location_t* _loc);

struct Chunk_t* getEdgeChunk(uint32_t _i);

void chunkAddLocation(struct Chunk_t* _chunk, struct Location_t* _loc);

void chunkRemoveLocation(struct Chunk_t* _chunk, struct Location_t* _loc);

void chunkAddCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo);

void chunkRemoveCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo);

void chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tick, uint8_t _zoom);

void initChunk(void);