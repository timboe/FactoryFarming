#pragma once
#include "game.h"

enum kLocationType {kEmpty, kConveyor};

enum kCargoType {kNoCargo, kApple};

struct Cargo_t{
  enum kCargoType m_type;
  LCDSprite* m_sprite;
};


struct Location_t{
  enum kLocationType m_type;
  enum  kConvDir m_convDir; // Only used by conveyors
  struct Chunk_t* m_chunk; // My chunk
  LCDSprite* m_sprite;
  LCDBitmap* m_image;
  struct Cargo_t* m_cargo;
  struct Location_t* m_next;
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In location coords
  uint16_t m_y;
  uint8_t m_progress; // Counts towards the location's progress
  void (*m_updateFn)(struct Location_t*, uint8_t);
};


struct Chunk_t{
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In chunk coords
  uint16_t m_y;

  LCDSprite* m_bkgSprite;
  LCDBitmap* m_bkgImage; 

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


struct Player_t{
  LCDSprite* m_sprite;
  float m_x;
  float m_y;
  float m_vX;
  float m_vY;
};



void initSprite(void);
void deinitSprite(void);

struct Location_t* getLocation(int32_t _x, int32_t _y);

struct Chunk_t* getChunk(int32_t _x, int32_t _y);

struct Chunk_t* getChunk_noCheck(const int32_t _x, const int32_t _y);

struct Chunk_t* getEdgeChunk(uint32_t _i);

void animateConveyor(void);
bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir);
uint16_t getNConveyors(void);

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type);
uint16_t getNCargo(void);

void setRoobert10(void);

struct Player_t* getPlayer(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y);



void chunkAddLocation(struct Chunk_t* _chunk, struct Location_t* _loc);

void chunkRemoveLocation(struct Chunk_t* _chunk, struct Location_t* _loc);

void chunkAddCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo);

void chunkRemoveCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo);