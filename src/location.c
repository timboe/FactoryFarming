#include "location.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"


struct Location_t* m_locations = NULL;

const int32_t SIZE_LOCATION = TOT_TILES * sizeof(struct Location_t);

/// ///

struct Location_t* getLocation_noCheck(int32_t _x, int32_t _y) {
  const uint32_t l = (TOT_TILES_X * _y) + _x;
  return &m_locations[l];
}

struct Location_t* getLocation(int32_t _x, int32_t _y) {
  if (_x < 0) _x += TOT_TILES_X;
  else if (_x >= TOT_TILES_X) _x -= TOT_TILES_X;
  if (_y < 0) _y += TOT_TILES_Y;
  else if (_y >= TOT_TILES_Y) _y -= TOT_TILES_Y;
  return getLocation_noCheck(_x, _y);
}

void clearLocation(struct Location_t* _loc) {
  bool removed = false;

  if (_loc->m_cargo) {
    chunkRemoveCargo(_loc->m_chunk, _loc->m_cargo);
    cargoManagerFreeCargo(_loc->m_cargo);
    _loc->m_cargo = NULL;
    removed = true;
  }

  if (_loc->m_building) {
    chunkRemoveLocation(_loc->m_chunk, _loc);
    buildingManagerFreeBuilding(_loc->m_building);
    renderChunkBackgroundImage(_loc->m_chunk);
    _loc->m_building = NULL;
    removed = true;
  }

  if (removed) {
    updateRenderList();
  }
}


void initLocation() {
  m_locations = pd->system->realloc(NULL, SIZE_LOCATION);
  pd->system->logToConsole("malloc: for location %i", SIZE_LOCATION/1024);


}

void resetLocation() {
  memset(m_locations, 0, SIZE_LOCATION);
  for (uint32_t x = 0; x < TOT_TILES_X; ++x) {
    for (uint32_t y = 0; y < TOT_TILES_Y; ++y) {
      struct Location_t* loc = getLocation_noCheck(x, y);
      loc->m_x = x;
      loc->m_y = y;
      loc->m_chunk = getChunk_noCheck(x / TILES_PER_CHUNK_X, y / TILES_PER_CHUNK_Y);
    }
  }
}

void serialiseLocation(struct json_encoder* je) {
  je->addTableMember(je, "location", 8);
  je->startArray(je);

  for (uint32_t x = 0; x < TOT_TILES_X; ++x) {
    for (uint32_t y = 0; y < TOT_TILES_Y; ++y) {
      struct Location_t* loc = getLocation_noCheck(x, y);
      if (loc->m_cargo == NULL && loc->m_building == NULL) {
        continue;
      }

      int16_t idCargo = -1, idBuilding = -1;
      if (loc->m_cargo) idCargo = loc->m_cargo->m_index;
      if (loc->m_building) idBuilding = loc->m_building->m_index;

      je->startTable(je);
      je->addTableMember(je, "x", 1);
      je->writeInt(je, x);
      je->addTableMember(je, "y", 1);
      je->writeInt(je, y);
      je->addTableMember(je, "cargo", 5);
      je->writeInt(je, idCargo);
      je->addTableMember(je, "build", 5);
      je->writeInt(je, idBuilding);
      je->endTable(je);
    }
  }

  je->endArray(je);
}