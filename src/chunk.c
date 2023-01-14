#include "chunk.h"
#include "building.h"

const int32_t SIZE_CHUNK = TOT_CHUNKS * sizeof(struct Chunk_t);

uint32_t m_recursionCount;
#ifdef PRINT_MAX_RECURSION
uint32_t m_maxRecursion;
uint32_t m_totalRecursion;
#endif

struct Chunk_t* m_chunks;

void setChunkAssociations(void);

void setNonNeighborAssociationsFor(struct Chunk_t* _chunk);

void doNonNeighborAssociation(struct Chunk_t* _chunk, struct Chunk_t** _theNeighborList, struct Chunk_t** _theNonNeighborList, uint32_t _theListSize);

void chunkRemoveBuildingUpdateConveyor(struct Chunk_t* _chunk, struct Building_t* _building);

void chunkRemoveBuildingUpdateOther(struct Chunk_t* _chunk, struct Building_t* _building);

/// ///


struct Chunk_t* getChunk(int32_t _x, int32_t _y) {
  if (_x < 0) _x += WORLD_CHUNKS_X;
  else if (_x >= WORLD_CHUNKS_X) _x -= WORLD_CHUNKS_X;
  if (_y < 0) _y += WORLD_CHUNKS_Y;
  else if (_y >= WORLD_CHUNKS_Y) _y -= WORLD_CHUNKS_Y;
  return getChunk_noCheck(_x, _y);
}

struct Chunk_t* getChunk_noCheck(const int32_t _x, const int32_t _y) {
  const uint32_t l = (WORLD_CHUNKS_X * _y) + _x;
  return &m_chunks[l];
}

void setChunkSpriteOffsets(struct Chunk_t* _c, int16_t _x, int16_t _y) {
  // Move the background
  for (int32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    PDRect bound = {.x = 0, .y = 0, .width = CHUNK_PIX_X*zoom, .height = CHUNK_PIX_Y*zoom};
    if (_c->m_bkgSprite[zoom]) {
      pd->sprite->moveTo(_c->m_bkgSprite[zoom], (CHUNK_PIX_X*_c->m_x + CHUNK_PIX_X/2.0 + _x)*zoom, (CHUNK_PIX_Y*_c->m_y + CHUNK_PIX_Y/2.0 + _y)*zoom);
    }
  }
  // Set the offset to all locations such that any other moveTo calls can also apply the correct offset
  for (uint32_t x = TILES_PER_CHUNK_X * _c->m_x; x < TILES_PER_CHUNK_X * (_c->m_x + 1); ++x) {
    for (uint32_t y = TILES_PER_CHUNK_Y * _c->m_y; y < TILES_PER_CHUNK_Y * (_c->m_y + 1); ++y) {
      struct Location_t* loc = getLocation_noCheck(x, y);
      loc->m_pix_off_x = _x;
      loc->m_pix_off_y = _y; 
      // Apply the offset to any cargo we come across on the ground
      if (loc->m_cargo) {
        for (int32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
          // Cargo should always have a sprite
          pd->sprite->moveTo(loc->m_cargo->m_sprite[zoom], 
            (TILE_PIX*loc->m_x + loc->m_pix_off_x + TILE_PIX/2.0)*zoom, 
            (TILE_PIX*loc->m_y + loc->m_pix_off_y + TILE_PIX/2.0)*zoom);
        }
      }

      // Apply the offset to any buildings which have animated or large collision sprites. Utility is included due to fences
      if (loc->m_building) {
        if (loc->m_building->m_type >= kUtility) {
          for (int32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
            // TODO report, this crashes
            //pd->sprite->moveTo(loc->m_building->m_sprite[zoom], 
            //  (loc->m_building->m_pix_x + loc->m_pix_off_x - EXTRACTOR_PIX/2)*zoom, 
            //  (loc->m_building->m_pix_y + loc->m_pix_off_y - EXTRACTOR_PIX/2)*zoom);
          }
        } else {
          for (int32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
            // Note this is needed for animated conveyors
            if (loc->m_building->m_sprite[zoom]) {
              pd->sprite->moveTo(loc->m_building->m_sprite[zoom], 
                (loc->m_building->m_pix_x + loc->m_pix_off_x) * zoom, 
                (loc->m_building->m_pix_y + loc->m_pix_off_y) * zoom);
            }
          }
        }
      }

      //

    }
  }

}

void setChunkPairSpriteOffsets(struct Chunk_t* _c1, struct Chunk_t* _c2, int16_t _x, int16_t _y) {
  setChunkSpriteOffsets(_c1, _x, _y);
  setChunkSpriteOffsets(_c2, _x, _y);
}

void setChunkTripletSpriteOffsets(struct Chunk_t* _c1, struct Chunk_t* _c2, struct Chunk_t* _c3, int16_t _x, int16_t _y) {
  setChunkSpriteOffsets(_c1, _x, _y);
  setChunkSpriteOffsets(_c2, _x, _y);
  setChunkSpriteOffsets(_c3, _x, _y);
}

void setChunkQuadSpriteOffsets(struct Chunk_t* _c1, struct Chunk_t* _c2, struct Chunk_t* _c3, struct Chunk_t* _c4, int16_t _x, int16_t _y) {
  setChunkPairSpriteOffsets(_c1, _c2, _x, _y);
  setChunkPairSpriteOffsets(_c3, _c4, _x, _y);
}


void chunkResetTorus() {
  for (int32_t x = 0; x < WORLD_CHUNKS_X; ++x) {
    struct Chunk_t* top         = getChunk_noCheck(x, 0);
    struct Chunk_t* top_next    = getChunk_noCheck(x, 1);
    struct Chunk_t* bottom      = getChunk_noCheck(x, WORLD_CHUNKS_Y-1);
    struct Chunk_t* bottom_next = getChunk_noCheck(x, WORLD_CHUNKS_Y-2);
    setChunkPairSpriteOffsets(top, top_next, 0, 0);
    setChunkPairSpriteOffsets(bottom, bottom_next, 0, 0);
  }

  // Don't need to re-do the ones the x-loop already did
  for (int32_t y = 2; y < WORLD_CHUNKS_Y-2; ++y) {
    struct Chunk_t* left = getChunk_noCheck(0, y);
    struct Chunk_t* left_next = getChunk_noCheck(1, y);
    struct Chunk_t* right = getChunk_noCheck(WORLD_CHUNKS_X-1, y);
    struct Chunk_t* right_next = getChunk_noCheck(WORLD_CHUNKS_X-2, y);
    setChunkPairSpriteOffsets(left, left_next, 0, 0);
    setChunkPairSpriteOffsets(right, right_next, 0, 0);
  }
}


void chunkShiftTorus(bool _top, bool _left) {

  chunkResetTorus();

  #ifdef DEV
  pd->system->logToConsole("Shift Torus TOP:%i LEFT:%i", (int)_top, (int)_left);
  #endif

  for (int32_t x = 1; x < WORLD_CHUNKS_X - 1; ++x) {
    struct Chunk_t* top = getChunk_noCheck(x, 0);
    struct Chunk_t* top_next    = getChunk_noCheck(x, 1);
    struct Chunk_t* bottom = getChunk_noCheck(x, WORLD_CHUNKS_Y-1);
    struct Chunk_t* bottom_next = getChunk_noCheck(x, WORLD_CHUNKS_Y-2);
    if (_top) {
      setChunkPairSpriteOffsets(bottom, bottom_next, 0, -TOT_WORLD_PIX_Y);
    } else {
      setChunkPairSpriteOffsets(top, top_next, 0, TOT_WORLD_PIX_Y);
    }
  }

  for (int32_t y = 1; y < WORLD_CHUNKS_Y - 1; ++y) {
    struct Chunk_t* left = getChunk_noCheck(0, y);
    struct Chunk_t* left_next = getChunk_noCheck(1, y);
    struct Chunk_t* right = getChunk_noCheck(WORLD_CHUNKS_X-1, y);
    struct Chunk_t* right_next = getChunk_noCheck(WORLD_CHUNKS_X-2, y);
    if (_left) {
      setChunkPairSpriteOffsets(right, right_next, -TOT_WORLD_PIX_X, 0);
    } else {
      setChunkPairSpriteOffsets(left, left_next, TOT_WORLD_PIX_X, 0);
    }
  }

  // Handle the corners
  struct Chunk_t* TL = getChunk_noCheck(0, 0);
  struct Chunk_t* TL_SE = getChunk_noCheck(1, 1);
  struct Chunk_t* TL_S = getChunk_noCheck(0, 1);
  struct Chunk_t* TL_E = getChunk_noCheck(1, 0);

  struct Chunk_t* TR = getChunk_noCheck(WORLD_CHUNKS_X-1, 0);
  struct Chunk_t* TR_SW = getChunk_noCheck(WORLD_CHUNKS_X-2, 1);
  struct Chunk_t* TR_S = getChunk_noCheck(WORLD_CHUNKS_X-1, 1);
  struct Chunk_t* TR_W = getChunk_noCheck(WORLD_CHUNKS_X-2, 0);

  struct Chunk_t* BL = getChunk_noCheck(0, WORLD_CHUNKS_Y-1);
  struct Chunk_t* BL_NE = getChunk_noCheck(1, WORLD_CHUNKS_Y-2);
  struct Chunk_t* BL_N = getChunk_noCheck(0, WORLD_CHUNKS_Y-2);
  struct Chunk_t* BL_E = getChunk_noCheck(1, WORLD_CHUNKS_Y-1);

  struct Chunk_t* BR = getChunk_noCheck(WORLD_CHUNKS_X-1, WORLD_CHUNKS_Y-1);
  struct Chunk_t* BR_NW = getChunk_noCheck(WORLD_CHUNKS_X-2, WORLD_CHUNKS_Y-2);
  struct Chunk_t* BR_N = getChunk_noCheck(WORLD_CHUNKS_X-1, WORLD_CHUNKS_Y-2);
  struct Chunk_t* BR_W = getChunk_noCheck(WORLD_CHUNKS_X-2, WORLD_CHUNKS_Y-1);

  if (_top && _left) {
    setChunkTripletSpriteOffsets(TR, TR_SW, TR_W,       -TOT_WORLD_PIX_X, 0);
    setChunkTripletSpriteOffsets(BL, BL_NE, BL_N,       0, -TOT_WORLD_PIX_Y);
    setChunkQuadSpriteOffsets   (BR, BR_NW, BR_N, BR_W, -TOT_WORLD_PIX_X, -TOT_WORLD_PIX_Y);
  } else if (_top && !_left) {
    setChunkTripletSpriteOffsets(TL, TL_SE, TL_E, +TOT_WORLD_PIX_X, 0);
    setChunkQuadSpriteOffsets(BL, BL_NE, BL_N, BL_E, +TOT_WORLD_PIX_X, -TOT_WORLD_PIX_Y);
    setChunkTripletSpriteOffsets(BR, BR_NW, BR_N, 0, -TOT_WORLD_PIX_Y);
  } else if (!_top && _left) {
    setChunkTripletSpriteOffsets(TL, TL_SE, TL_S, 0, +TOT_WORLD_PIX_Y);
    setChunkQuadSpriteOffsets(TR, TR_SW, TR_S, TR_W, -TOT_WORLD_PIX_X, +TOT_WORLD_PIX_Y);
    setChunkTripletSpriteOffsets(BR, BR_NW, BR_W, -TOT_WORLD_PIX_X, 0);
  } else {
    setChunkQuadSpriteOffsets(TL, TL_SE, TL_S, TL_E, +TOT_WORLD_PIX_X, +TOT_WORLD_PIX_Y);
    setChunkTripletSpriteOffsets(TR, TR_SW, TR_S, 0, +TOT_WORLD_PIX_Y);
    setChunkTripletSpriteOffsets(BL, BL_NE, BL_E, +TOT_WORLD_PIX_X, 0);
  }
}

void chunkAddBuildingRender(struct Chunk_t* _chunk, struct Building_t* _building) {
  _chunk->m_buildingsRender[ _chunk->m_nBuildingsRender ] = _building;
  ++(_chunk->m_nBuildingsRender);
}

void chunkAddBuildingUpdate(struct Chunk_t* _chunk, struct Building_t* _building) {
  if (_building->m_type == kConveyor) {
    _chunk->m_buildingsUpdateConveyor[ _chunk->m_nBuildingsUpdateConveyor ] = _building;
    ++(_chunk->m_nBuildingsUpdateConveyor);
  } else {
    _chunk->m_buildingsUpdateOther[ _chunk->m_nBuildingsUpdateOther ] = _building;
    ++(_chunk->m_nBuildingsUpdateOther);
  }
}

void chunkRemoveBuildingRender(struct Chunk_t* _chunk, struct Building_t* _building) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_buildingsRender[i] == _building) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveBuildingRender!");
  #endif
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_buildingsRender[i] = _chunk->m_buildingsRender[i + 1];
    if (_chunk->m_buildingsRender[i] == NULL) break;
  }
  --(_chunk->m_nBuildingsRender);
}

void chunkRemoveBuildingUpdate(struct Chunk_t* _chunk, struct Building_t* _building) {
  if (_building->m_type == kConveyor) {
    chunkRemoveBuildingUpdateConveyor(_chunk, _building);
  } else {
    chunkRemoveBuildingUpdateOther(_chunk, _building);
  }
}

void chunkRemoveBuildingUpdateConveyor(struct Chunk_t* _chunk, struct Building_t* _building) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_buildingsUpdateConveyor[i] == _building) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    // All buildings are Render, but not all buildings are Update. Return if not found
    return;
  }
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_buildingsUpdateConveyor[i] = _chunk->m_buildingsUpdateConveyor[i + 1];
    if (_chunk->m_buildingsUpdateConveyor[i] == NULL) break;
  }
  --(_chunk->m_nBuildingsUpdateConveyor);
}

void chunkRemoveBuildingUpdateOther(struct Chunk_t* _chunk, struct Building_t* _building) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_buildingsUpdateOther[i] == _building) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    // All buildings are Render, but not all buildings are Update. Return if not found
    return;
  }
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_buildingsUpdateOther[i] = _chunk->m_buildingsUpdateOther[i + 1];
    if (_chunk->m_buildingsUpdateOther[i] == NULL) break;
  }
  --(_chunk->m_nBuildingsUpdateOther);
}

void chunkAddCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo) {
  _chunk->m_cargos[ _chunk->m_nCargos ] = _cargo;
  ++(_chunk->m_nCargos);
}

void chunkRemoveCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_cargos[i] == _cargo) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveCargo!");
  #endif
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_cargos[i] = _chunk->m_cargos[i + 1];
    if (_chunk->m_cargos[i] == NULL) break;
  }
  --(_chunk->m_nCargos);
}

void chunkAddObstacle(struct Chunk_t* _chunk, LCDSprite* _obstacleZ1, LCDSprite* _obstacleZ2) {
  _chunk->m_obstacles[ _chunk->m_nObstacles ][1] = _obstacleZ1;
  _chunk->m_obstacles[ _chunk->m_nObstacles ][2] = _obstacleZ2;
  ++(_chunk->m_nObstacles);
}

void chunkRemoveObstacle(struct Chunk_t* _chunk, LCDSprite* _obstacleZ1) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_obstacles[i][1] == _obstacleZ1) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveObstacle!");
  #endif
  // Note: We owned this!
  pd->sprite->freeSprite(_chunk->m_obstacles[idx][1]);
  pd->sprite->freeSprite(_chunk->m_obstacles[idx][2]);
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_obstacles[i][1] = _chunk->m_obstacles[i + 1][1];
    _chunk->m_obstacles[i][2] = _chunk->m_obstacles[i + 1][2];
    if (_chunk->m_obstacles[i] == NULL) break;
  }
  --(_chunk->m_nObstacles);
}

uint16_t chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {
  for (uint32_t i = 0; i < _chunk->m_nBuildingsUpdateConveyor; ++i) {
    m_recursionCount = 0;
    (*_chunk->m_buildingsUpdateConveyor[i]->m_updateFn)(_chunk->m_buildingsUpdateConveyor[i], _tickLength, _tickID, _zoom);
    #ifdef PRINT_MAX_RECURSION
    if (_tickLength == FAR_TICK_AMOUNT && m_recursionCount > m_maxRecursion) {
      m_maxRecursion = m_recursionCount;
      m_totalRecursion += m_recursionCount;
    }
    #endif
  }
  for (uint32_t i = 0; i < _chunk->m_nBuildingsUpdateOther; ++i) {
    (*_chunk->m_buildingsUpdateOther[i]->m_updateFn)(_chunk->m_buildingsUpdateOther[i], _tickLength, _tickID, _zoom);
  }
  return _chunk->m_nBuildingsUpdateConveyor + _chunk->m_nBuildingsUpdateOther;
}

#ifdef PRINT_MAX_RECURSION
void printRecursionSummary() {
  pd->system->logToConsole("Max recursion: %i, total recursion: %i", m_maxRecursion, m_totalRecursion);
  m_maxRecursion = 0;
  m_totalRecursion = 0;
}
#endif


void doNonNeighborAssociation(struct Chunk_t* _chunk, struct Chunk_t** _theNeighborList, struct Chunk_t** _theNonNeighborList, uint32_t _theListSize) {
  uint32_t counter = 0;
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      struct Chunk_t* testChunk = getChunk_noCheck(x, y);

      bool found = (testChunk == _chunk);
      for (uint32_t i = 0; i < _theListSize; ++i) {
        if (_theNeighborList[i] == testChunk) {
          found = true;
          break;
        }
      }

      if (!found) {
        _theNonNeighborList[counter++] = testChunk;
      }
    }
  }
}

void setNonNeighborAssociationsFor(struct Chunk_t* _chunk) {
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsNE, _chunk->m_nonNeighborsNE, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsSE, _chunk->m_nonNeighborsSE, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsSW, _chunk->m_nonNeighborsSW, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsNW, _chunk->m_nonNeighborsNW, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsALL, _chunk->m_nonNeighborsALL, CHUNK_NEIGHBORS_ALL);
}

// Each chunk should know who its neighbors are for fast rendering
void setChunkAssociations(void) {
  // Set neighbor associations
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      struct Chunk_t* c = getChunk_noCheck(x, y);
      c->m_neighborsNE[0] = getChunk(x + 0, y - 1);
      c->m_neighborsNE[1] = getChunk(x + 1, y - 1);
      c->m_neighborsNE[2] = getChunk(x + 1, y + 0);

      c->m_neighborsSE[0] = getChunk(x + 1, y + 0);
      c->m_neighborsSE[1] = getChunk(x + 1, y + 1);
      c->m_neighborsSE[2] = getChunk(x + 0, y + 1);

      c->m_neighborsSW[0] = getChunk(x + 0, y + 1);
      c->m_neighborsSW[1] = getChunk(x - 1, y + 1);
      c->m_neighborsSW[2] = getChunk(x - 1, y + 0);

      c->m_neighborsNW[0] = getChunk(x - 1, y + 0);
      c->m_neighborsNW[1] = getChunk(x - 1, y - 1);
      c->m_neighborsNW[2] = getChunk(x + 0, y - 1);

      c->m_neighborsALL[0] = getChunk(x + 0, y - 1);
      c->m_neighborsALL[1] = getChunk(x + 1, y - 1);
      c->m_neighborsALL[2] = getChunk(x + 1, y + 0);
      c->m_neighborsALL[3] = getChunk(x + 1, y + 1);
      c->m_neighborsALL[4] = getChunk(x + 0, y + 1);
      c->m_neighborsALL[5] = getChunk(x - 1, y + 1);
      c->m_neighborsALL[6] = getChunk(x - 1, y + 0);
      c->m_neighborsALL[7] = getChunk(x - 1, y - 1);
    }
  }

  // Set non-neighbor associations
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      setNonNeighborAssociationsFor(getChunk_noCheck(x, y));
    }
  }
}

void resetChunk() {
  for (uint32_t x = 0; x < WORLD_CHUNKS_X; ++x) {
    for (uint32_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
      // Owns background sprites and images
      for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
        struct Chunk_t* chunk = getChunk_noCheck(x, y);
        if (chunk->m_bkgSprite[zoom]) pd->sprite->freeSprite(chunk->m_bkgSprite[zoom]);
        if (chunk->m_bkgImage[zoom]) pd->graphics->freeBitmap(chunk->m_bkgImage[zoom]);
        // Not enough RAM :(
        //if (chunk->m_bkgImage2[zoom]) pd->graphics->freeBitmap(chunk->m_bkgImage2[zoom]);
        //...
        // Owns obstacle sprites
        for (uint32_t i = 0; i < chunk->m_nObstacles; ++i) {
          pd->sprite->freeSprite(chunk->m_obstacles[i][zoom]);
        }
      }
    }
  }
  memset(m_chunks, 0, SIZE_CHUNK);
  for (uint32_t x = 0; x < WORLD_CHUNKS_X; ++x) {
    for (uint32_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
      struct Chunk_t* chunk = getChunk_noCheck(x, y);
      chunk->m_x = x;
      chunk->m_y = y;
    }
  }
  // The neighboring associations, including modulo behavior
  setChunkAssociations();
}

void initChunk() {
  m_chunks = pd->system->realloc(NULL, SIZE_CHUNK);
  memset(m_chunks, 0, SIZE_CHUNK);
  #ifdef DEV
  pd->system->logToConsole("malloc: for chunks %i", SIZE_CHUNK/1024);
  #endif
}