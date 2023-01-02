#include "location.h"
#include "generate.h"
#include "chunk.h"
#include "building.h"
#include "sound.h"
#include "sprite.h"
#include "render.h"
#include "player.h"
#include "buildings/special.h"
#include "buildings/factory.h"
#include "buildings/utility.h"

struct Location_t* m_locations = NULL;

int16_t m_deserialiseXLocation = -1, m_deserialiseYLocation = -1, m_deserialiseIDBuilding = -1, m_deserialiseIDCargo = -1;
bool m_deserialiseNotOwned = false;

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

bool clearLocation(struct Location_t* _loc, bool _clearCargo, bool _clearBuilding) {

  bool cleared = false;

  if (_clearCargo && _loc->m_cargo) {
    chunkRemoveCargo(_loc->m_chunk, _loc->m_cargo);
    cargoManagerFreeCargo(_loc->m_cargo);
    _loc->m_cargo = NULL;
    cleared = true;
  }

  if (_clearBuilding && _loc->m_building) {
    cleared = true;
    const enum kBuildingType bt = _loc->m_building->m_type;
    const union kSubType bst = _loc->m_building->m_subType;

    bool wideRedraw = false;
    bool checkFactoryUpgrades = false;
    if (bt == kUtility && bst.utility == kWell) {
      // Special - well
      pauseMusic();
      setTile( getTile_idx(_loc->m_x, _loc->m_y), _loc->m_building->m_mode.mode16 ); // Undo before destroying
      doWetnessAroundLoc(_loc);
      wideRedraw = true;
    } else if (bt == kUtility && bst.utility == kFactoryUpgrade) {
      // Special - factory upgrade
      checkFactoryUpgrades = true;
    } else if (bt == kUtility && bst.utility == kRotavator) {
      // Special - rotavator
      destroyRotavator(_loc);
    } else if (getPlayer()->m_enableExtractorOutlines 
      && bt == kExtractor && (bst.extractor == kCropHarvesterSmall || bst.extractor == kCropHarvesterLarge))
    {
      // Special, harvester with outline
      pauseMusic();
      wideRedraw = true;
    }

    // If a non-owned part of a multi block - then defer to the actual owned location
    if (_loc->m_notOwned) {
      return clearLocation(_loc->m_building->m_location, _clearCargo, _clearBuilding);
    }

    const bool ilb = isLargeBuilding(bt, bst); 

    // If multi-block, first clear the other non-owning links
    if (ilb) {
      for (int32_t x = -1; x < 2; ++x) {
        for (int32_t y = -1; y < 2; ++y) {
          if (!x && !y) continue;
          struct Location_t* otherLoc = getLocation(_loc->m_x + x, _loc->m_y + y);
          if (otherLoc->m_notOwned == false) {
            pd->system->error("LOCATION %i %i SHOULD BE NOT OWNED", _loc->m_x + x, _loc->m_y + y);
          }
          otherLoc->m_building = NULL;
          otherLoc->m_notOwned = false;
        }
      }
    }

    const bool isPathOrFenceOrRotavator = bt == kUtility && (bst.utility == kPath || bst.utility == kFence || bst.utility == kRotavator);

    chunkRemoveBuildingRender(_loc->m_chunk, _loc->m_building);
    chunkRemoveBuildingUpdate(_loc->m_chunk, _loc->m_building);
    buildingManagerFreeBuilding(_loc->m_building);

    if (wideRedraw) {
      renderChunkBackgroundImageAround(_loc->m_chunk);
      resumeMusic();
    } else if (ilb || isPathOrFenceOrRotavator) {
      renderChunkBackgroundImageAround3x3(_loc->m_chunk, _loc);
    } else {
      renderChunkBackgroundImage(_loc->m_chunk);
    }

    _loc->m_building = NULL;
    _loc->m_notOwned = false;

    if (checkFactoryUpgrades) {
      checkUpdateFactoryUpgradeAroundLoc(_loc);
    }

  }

  return cleared;
}


void initLocation() {
  m_locations = pd->system->realloc(NULL, SIZE_LOCATION);
  #ifdef DEV
  pd->system->logToConsole("malloc: for location %i", SIZE_LOCATION/1024);
  #endif
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

      je->addArrayMember(je);
      je->startTable(je);
      je->addTableMember(je, "x", 1);
      je->writeInt(je, x);
      je->addTableMember(je, "y", 1);
      je->writeInt(je, y);
      if (loc->m_cargo) {
        je->addTableMember(je, "cargo", 5);
        je->writeInt(je, loc->m_cargo->m_index);
      }
      if (loc->m_building) {
        je->addTableMember(je, "build", 5);
        je->writeInt(je, loc->m_building->m_index);
      }
      if (loc->m_notOwned) {
        je->addTableMember(je, "nowned", 6);
        je->writeInt(je, 1);
      }
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
  } else if (strcmp(_key, "nowned") == 0) {
    m_deserialiseNotOwned = json_boolValue(_value);
  } else {
    pd->system->error("LOCATION DECODE ISSUE, %s", _key);
  }
}

void* deserialiseStructDoneLocation(json_decoder* jd, const char* _name, json_value_type _type) {
  struct Location_t* loc = getLocation_noCheck(m_deserialiseXLocation, m_deserialiseYLocation);

  if (m_deserialiseIDBuilding >= 0) {
    loc->m_building = buildingManagerGetByIndex(m_deserialiseIDBuilding);
    loc->m_notOwned = m_deserialiseNotOwned;
    if (!loc->m_notOwned) { // If owned
      chunkAddBuildingRender(loc->m_chunk, loc->m_building);
      if (buildingHasUpdateFunction(loc->m_building->m_type, loc->m_building->m_subType)) {
        chunkAddBuildingUpdate(loc->m_chunk, loc->m_building);
      }
    }
  }
  if (m_deserialiseIDCargo >= 0) {
    loc->m_cargo = cargoManagerGetByIndex(m_deserialiseIDCargo);
    chunkAddCargo(loc->m_chunk, loc->m_cargo);
    if (loc->m_building && loc->m_building->m_type == kConveyor) {
      pd->sprite->setZIndex(loc->m_cargo->m_sprite[1], Z_INDEX_CARGO_BELT);
      pd->sprite->setZIndex(loc->m_cargo->m_sprite[2], Z_INDEX_CARGO_BELT);
    }
  }

  //pd->system->logToConsole("-- Location (%i, %i) building:#%i (not owned? %i), cargo:#%i",
  //  m_deserialiseXLocation, m_deserialiseYLocation, m_deserialiseIDBuilding, (int)m_deserialiseNotOwned, m_deserialiseIDCargo);

  m_deserialiseXLocation = -1;
  m_deserialiseYLocation = -1;
  m_deserialiseIDBuilding = -1;
  m_deserialiseIDCargo = -1;
  m_deserialiseNotOwned = false;

  return NULL;
}
