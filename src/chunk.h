#pragma once
#include "game.h"
#include "cargo.h"
#include "location.h"

struct Chunk_t{
  // Persistent
  // None

  // Transient
  uint16_t m_x; // In chunk coords
  uint16_t m_y;

  LCDSprite* m_bkgSprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_bkgImage[ZOOM_LEVELS];  // Index 0 not used - OWNED
  // Not enough RAM :(
  //LCDBitmap* m_bkgImage2[ZOOM_LEVELS];  // Index 0 not used - OWNED
  //LCDBitmap* m_bkgImage3[ZOOM_LEVELS];  // Index 0 not used - OWNED
  //LCDBitmap* m_bkgImage4[ZOOM_LEVELS];  // Index 0 not used - OWNED

  struct Chunk_t* m_neighborsNE[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsSE[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsSW[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsNW[CHUNK_NEIGHBORS_CORNER];
  struct Chunk_t* m_neighborsALL[CHUNK_NEIGHBORS_ALL];

  struct Chunk_t* m_nonNeighborsNE[CHUNK_NONNEIGHBORS_CORNER]; // The "- 1" is for "me"
  struct Chunk_t* m_nonNeighborsSE[CHUNK_NONNEIGHBORS_CORNER];
  struct Chunk_t* m_nonNeighborsSW[CHUNK_NONNEIGHBORS_CORNER];
  struct Chunk_t* m_nonNeighborsNW[CHUNK_NONNEIGHBORS_CORNER];
  struct Chunk_t* m_nonNeighborsALL[CHUNK_NONNEIGHBORS_ALL];

  uint16_t m_nBuildingsRender;
  struct Building_t* m_buildingsRender[TILES_PER_CHUNK];

  uint16_t m_nBuildingsUpdate;
  struct Building_t* m_buildingsUpdate[TILES_PER_CHUNK];

  uint16_t m_nCargos;
  struct Cargo_t* m_cargos[TILES_PER_CHUNK];

  uint16_t m_nObstacles;
  LCDSprite* m_obstacles[TILES_PER_CHUNK][ZOOM_LEVELS]; // OWNED
};

struct Chunk_t* getChunk(int32_t _x, int32_t _y);

struct Chunk_t* getChunk_noCheck(const int32_t _x, const int32_t _y);

void chunkAddBuildingRender(struct Chunk_t* _chunk, struct Building_t* _building);

void chunkAddBuildingUpdate(struct Chunk_t* _chunk, struct Building_t* _building);

void chunkRemoveBuildingRender(struct Chunk_t* _chunk, struct Building_t* _building);

void chunkRemoveBuildingUpdate(struct Chunk_t* _chunk, struct Building_t* _building);

void chunkAddCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo);

void chunkRemoveCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo);

void chunkAddObstacle(struct Chunk_t* _chunk, LCDSprite* _obstacleZ1,  LCDSprite* _obstacleZ2);

void chunkRemoveObstacle(struct Chunk_t* _chunk, LCDSprite* _obstacleZ1);

uint16_t chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void chunkShiftTorus(bool _top, bool _left);

void chunkResetTorus(void);

void initChunk(void);

void resetChunk(void);