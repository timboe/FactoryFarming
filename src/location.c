#include "location.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"

struct Location_t* m_locations = NULL;

int16_t m_deserialiseXLocation = -1, m_deserialiseYLocation = -1, m_deserialiseIDBuilding = -1, m_deserialiseIDCargo = -1;

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

void clearLocation(struct Location_t* _loc, bool _clearCargo, bool _clearBuilding) {
  bool removed = false;

  if (_clearCargo && _loc->m_cargo) {
    chunkRemoveCargo(_loc->m_chunk, _loc->m_cargo);
    cargoManagerFreeCargo(_loc->m_cargo);
    _loc->m_cargo = NULL;
    removed = true;
  }

  if (_clearBuilding && _loc->m_building) {
    chunkRemoveBuilding(_loc->m_chunk, _loc->m_building);
    buildingManagerFreeBuilding(_loc->m_building);
    renderChunkBackgroundImage(_loc->m_chunk);
    _loc->m_building = NULL;
    removed = true;
  }

  if (removed) {
    queueUpdateRenderList();
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
  m_deserialiseIDBuilding = -1;
  m_deserialiseIDCargo = -1;
  m_deserialiseXLocation = -1;
  m_deserialiseYLocation = -1;
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

      je->addArrayMember(je);
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

void deserialiseValueLocation(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "x") == 0) {
    m_deserialiseXLocation = json_intValue(_value);
  } else if (strcmp(_key, "y") == 0) {
    m_deserialiseYLocation = json_intValue(_value);
  } else if (strcmp(_key, "cargo") == 0) {
    m_deserialiseIDCargo = json_intValue(_value);
  } else if (strcmp(_key, "build") == 0) {
    m_deserialiseIDBuilding = json_intValue(_value);
  } else {
    pd->system->error("LOCATION DECODE ISSUE, %s", _key);
  }
}

void* deserialiseStructDoneLocation(json_decoder* jd, const char* _name, json_value_type _type) {
  struct Location_t* loc = getLocation_noCheck(m_deserialiseXLocation, m_deserialiseYLocation);

  if (m_deserialiseIDBuilding >= 0) {
    loc->m_building = buildingManagerGetByIndex(m_deserialiseIDBuilding);
    chunkAddBuilding(loc->m_chunk, loc->m_building);
  }
  if (m_deserialiseIDCargo >= 0) {
    loc->m_cargo = cargoManagerGetByIndex(m_deserialiseIDCargo);
    chunkAddCargo(loc->m_chunk, loc->m_cargo);
  }

  pd->system->logToConsole("-- Location (%i, %i) building:#%i, cargo:#%i",
    m_deserialiseXLocation, m_deserialiseYLocation, m_deserialiseIDBuilding, m_deserialiseIDCargo);

  m_deserialiseXLocation = -1;
  m_deserialiseYLocation = -1;
  m_deserialiseIDBuilding = -1;
  m_deserialiseIDCargo = -1;


  return NULL;
}
