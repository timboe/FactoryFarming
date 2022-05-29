#include "chunk.h"

struct Chunk_t* m_chunks;
struct Chunk_t* m_edgeChunks;

/// ///

struct Chunk_t* getEdgeChunk(uint32_t _i) {
  return &(m_edgeChunks[_i]);
}

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

struct Chunk_t* getChunk_fromLocation(struct Location_t* _loc) {
  //pd->system->logToConsole("ADD TO CHUNK: %i / %i = %i", (int)_loc->m_y, (int)TILES_PER_CHUNK_Y, (int)(_loc->m_y) / (int)TILES_PER_CHUNK_Y);
  return getChunk_noCheck(_loc->m_x / TILES_PER_CHUNK_X, _loc->m_y / TILES_PER_CHUNK_Y);
}

void chunkAddLocation(struct Chunk_t* _chunk, struct Location_t* _loc) {
  _chunk->m_locations[ _chunk->m_nLocations ] = _loc;
  ++(_chunk->m_nLocations);
}

void chunkRemoveLocation(struct Chunk_t* _chunk, struct Location_t* _loc) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_locations[i] == _loc) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveLocation!");
  #endif
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_locations[i] = _chunk->m_locations[i + 1];
  }
  --(_chunk->m_nLocations);
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
  }
  --(_chunk->m_nCargos);
}

void chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tick, uint8_t _zoom) {
  //pd->system->logToConsole("Asked to tick %i for %i", _tick, _chunk);
  for (uint32_t i = 0; i < _chunk->m_nLocations; ++i) {
    struct Location_t* loc = _chunk->m_locations[i];
    (*loc->m_updateFn)(loc, _tick, _zoom);
  }
}

void initChunk() {
  m_chunks = pd->system->realloc(NULL, TOT_CHUNKS * sizeof(struct Chunk_t));
  memset(m_chunks, 0, TOT_CHUNKS * sizeof(struct Chunk_t));

  for (uint32_t x = 0; x < WORLD_CHUNKS_X; ++x) {
    for (uint32_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
      struct Chunk_t* chunk = getChunk_noCheck(x, y);
      chunk->m_x = x;
      chunk->m_y = y;
      chunk->m_pix_x = CHUNK_PIX_X*x + CHUNK_PIX_X/2.0;
      chunk->m_pix_y = CHUNK_PIX_Y*y + CHUNK_PIX_Y/2.0;
    }
  }

  m_edgeChunks = pd->system->realloc(NULL, (WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4) * sizeof(struct Chunk_t));
  memset(m_edgeChunks, 0, (WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4) * sizeof(struct Chunk_t));
}