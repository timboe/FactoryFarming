#include "chunk.h"
#include "building.h"

const int32_t SIZE_CHUNK = TOT_CHUNKS * sizeof(struct Chunk_t);

struct Chunk_t* m_chunks;

void setChunkAssociations(void);

void setNonNeighborAssociationsFor(struct Chunk_t* _chunk);

void doNonNeighborAssociation(struct Chunk_t* _chunk, struct Chunk_t** _theNeighborList, struct Chunk_t** _theNonNeighborList, uint32_t _theListSize);

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
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    PDRect bound = {.x = 0, .y = 0, .width = CHUNK_PIX_X*zoom, .height = CHUNK_PIX_Y*zoom};
    pd->sprite->moveTo(_c->m_bkgSprite[zoom], (CHUNK_PIX_X*_c->m_x + CHUNK_PIX_X/2.0 + _x)*zoom, (CHUNK_PIX_Y*_c->m_y + CHUNK_PIX_Y/2.0 + _y)*zoom);
  }
}


void chunkShiftTorus(bool _top, bool _left) {
  return; // Come back to this...

  for (int32_t x = 0; x < WORLD_CHUNKS_X; ++x) {
    int32_t yMod = 0;
    if (x == 0 && !_left) {
      yMod = TOT_WORLD_PIX_X;
    }


    struct Chunk_t* top = getChunk_noCheck(x, 0);
    struct Chunk_t* bottom = getChunk_noCheck(x, WORLD_CHUNKS_Y-1);
    if (_top) {
      setChunkSpriteOffsets(top, 0, 0);
      setChunkSpriteOffsets(bottom, 0, -TOT_WORLD_PIX_Y);
    } else {
      setChunkSpriteOffsets(top, 0, TOT_WORLD_PIX_Y);
      setChunkSpriteOffsets(bottom, 0, 0);
    }
  }
}
/*
void chunkShiftTorusHorizontal(bool _left) {
  for (int32_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    struct Chunk_t* left = getChunk_noCheck(0, y);
    struct Chunk_t* right = getChunk_noCheck(WORLD_CHUNKS_X-1, y);
    if (_left) {
      setChunkSpriteOffsets(left, 0, 0);
      setChunkSpriteOffsets(right, -TOT_WORLD_PIX_X, 0);
    } else {
      setChunkSpriteOffsets(left, TOT_WORLD_PIX_X, 0);
      setChunkSpriteOffsets(right, 0, 0);
    }
  }
}
*/


void chunkAddBuilding(struct Chunk_t* _chunk, struct Building_t* _building) {
  _chunk->m_buildings[ _chunk->m_nBuildings ] = _building;
  ++(_chunk->m_nBuildings);
}

void chunkRemoveBuilding(struct Chunk_t* _chunk, struct Building_t* _building) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_buildings[i] == _building) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveBuilding!");
  #endif
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_buildings[i] = _chunk->m_buildings[i + 1];
    if (_chunk->m_buildings[i] == NULL) break;
  }
  --(_chunk->m_nBuildings);
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

uint16_t chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tick, uint8_t _zoom) {
  //pd->system->logToConsole("Asked to tick %i for %i", _tick, _chunk);
  for (uint32_t i = 0; i < _chunk->m_nBuildings; ++i) {
    (*_chunk->m_buildings[i]->m_updateFn)(_chunk->m_buildings[i], _tick, _zoom);
  }
  return _chunk->m_nBuildings;
}

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
  pd->system->logToConsole("malloc: for chunks %i", SIZE_CHUNK/1024);
}