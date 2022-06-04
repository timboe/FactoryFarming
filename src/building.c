#include "building.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "constants.h"

const int32_t SIZE_BUILDING = TOT_CARGO_OR_BUILDINGS * sizeof(struct Building_t);

uint16_t m_nBuildings = 0;

uint16_t m_nConveyors = 0;

uint16_t m_buildingSearchLocation = 0;

struct Building_t* m_buildings;

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick, uint8_t _zoom);

void buildingSpriteSetup(struct Building_t* _building, enum kConvDir _dir, uint8_t _zoom);

/// ///

uint16_t getNBuildings() {
  return m_nBuildings;
}

uint16_t getNConveyors() {
  return m_nConveyors;
}

struct Building_t* buildingManagerNewBuilding(enum kBuildingType _asType) {
  for (uint8_t try = 0; try < 2; ++try) {
    const uint32_t start = (try == 0 ? m_buildingSearchLocation : 0);
    const uint32_t stop  = (try == 0 ? TOT_CARGO_OR_BUILDINGS : m_buildingSearchLocation);
    for (uint32_t i = start; i < stop; ++i) {
      if (m_buildings[i].m_type == kNoBuilding) {
        ++m_nBuildings;
        ++m_buildingSearchLocation;
        if (_asType == kConveyor) {
          ++m_nConveyors;
        }
        m_buildings[i].m_type = _asType;
        return &(m_buildings[i]);
      }
    }
  }
  #ifdef DEV
  pd->system->error("Cannot allocate any more building!");
  #endif
  return NULL;
}

void buildingManagerFreeBuilding(struct Building_t* _building) {
  if (_building->m_type == kConveyor) {
    --m_nConveyors;
  }
  _building->m_type = kNoBuilding;
  _building->m_updateFn = NULL;
  m_buildingSearchLocation = _building->m_index;
  --m_nBuildings;
}

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick, uint8_t _zoom) {
  if (_loc->m_cargo == NULL) return;
  struct Building_t* building = _loc->m_building;
  if (building->m_progress < TILE_PIX) {
    building->m_progress += _tick;
    switch (building->m_nextDir[building->m_mode]) {
      case SN:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], building->m_pix_x*_zoom, (building->m_pix_y - building->m_progress)*_zoom); break;
      case NS:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], building->m_pix_x*_zoom, (building->m_pix_y + building->m_progress)*_zoom); break;
      case EW:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], (building->m_pix_x - building->m_progress)*_zoom, building->m_pix_y*_zoom); break;
      case WE:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], (building->m_pix_x + building->m_progress)*_zoom, building->m_pix_y*_zoom); break;
      case kConvDirN:; break;
    }
    //pd->system->logToConsole("MOVE %i %i, %i", _loc->m_pix_x, _loc->m_pix_y, _loc->m_progress);
  }
  if (building->m_progress >= TILE_PIX && building->m_next[building->m_mode]->m_cargo == NULL) {
    struct Cargo_t* theCargo = _loc->m_cargo;
    struct Location_t* nextLoc = building->m_next[building->m_mode]; 
    // Move cargo
    nextLoc->m_cargo = theCargo;
    _loc->m_cargo = NULL;
    // Carry over any excess ticks
    if (nextLoc->m_building) {
      nextLoc->m_building->m_progress = building->m_progress - TILE_PIX;
    }
    // Cargo moves between chunks?
    if (nextLoc->m_chunk != _loc->m_chunk) {
      //pd->system->logToConsole("CHANGE CHUNK");
      chunkRemoveCargo(_loc->m_chunk, theCargo);
      chunkAddCargo(nextLoc->m_chunk, theCargo);
      queueUpdateRenderList();
    }
    // Cycle outputs
    switch (building->m_convSubType) {
      case kSplitI:; case kSplitL:; case kFilterL:; building->m_mode = (building->m_mode + 1) % 2; break;
      case kSplitT:; building->m_mode = (building->m_mode + 1) % 3; break;
      case kBelt:; default: break;
    }
  }
}

void buildingSpriteSetup(struct Building_t* _building, enum kConvDir _dir, uint8_t _zoom) {
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*_zoom, .height = TILE_PIX*_zoom};
  pd->sprite->setBounds(_building->m_sprite[_zoom], bound);
  pd->sprite->setImage(_building->m_sprite[_zoom], getConveyorMaster(_zoom, _dir), kBitmapUnflipped);
  pd->sprite->moveTo(_building->m_sprite[_zoom], _building->m_pix_x*_zoom, _building->m_pix_y*_zoom);
  pd->sprite->setZIndex(_building->m_sprite[_zoom], Z_INDEX_CONVEYOR);
}

bool newConveyor(struct Location_t* _loc, enum kConvDir _dir, enum kConvSubType _subType) {
  bool newToChunk = false;
  if (_loc->m_building == NULL) {
    newToChunk = true;
    _loc->m_building = buildingManagerNewBuilding(kConveyor);
    if (!_loc->m_building) {
      // Run out of buildings
      return false;
    }
  } else if (_loc->m_building->m_type != kConveyor) {
    return false;
  }
  struct Building_t* building = _loc->m_building;

  if (building->m_sprite[1] == NULL) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      building->m_sprite[zoom] = pd->sprite->newSprite();
    }
  }

  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_subType) {
      case kBelt:;   building->m_image[zoom] = getSprite16(0, CONV_START_Y + _dir, zoom); break;
      case kSplitI:; building->m_image[zoom] = getSprite16(0, 11 + _dir, zoom); break;
      case kSplitL:; building->m_image[zoom] = getSprite16(1, 11 + _dir, zoom); break;
      case kSplitT:; building->m_image[zoom] = getSprite16(2, 11 + _dir, zoom); break;
      case kFilterL:; default: break;
    }
  }
  building->m_convSubType = _subType;
  building->m_convDir = _dir;
  building->m_updateFn = &conveyorUpdateFn;
  building->m_progress = 0;
  building->m_mode = 0;

  building->m_pix_x = TILE_PIX*_loc->m_x + TILE_PIX/2.0;
  building->m_pix_y = TILE_PIX*_loc->m_y + TILE_PIX/2.0;

  if (_subType == kBelt) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      buildingSpriteSetup(building, _dir, zoom);
    }
  }

  struct Location_t* above = getLocation(_loc->m_x, _loc->m_y - 1);
  struct Location_t* below = getLocation(_loc->m_x, _loc->m_y + 1);
  struct Location_t* left  = getLocation(_loc->m_x - 1, _loc->m_y);
  struct Location_t* right = getLocation(_loc->m_x + 1, _loc->m_y);
  if (_subType == kBelt) {
    switch (_dir) {
      case SN:; building->m_next[0]    = above;
                building->m_nextDir[0] = SN; break;
      case NS:; building->m_next[0]    = below;
                building->m_nextDir[0] = NS; break;
      case WE:; building->m_next[0]    = right;
                building->m_nextDir[0] = WE; break;
      case EW:; building->m_next[0]    = left;
                building->m_nextDir[0] = EW; break;
      case kConvDirN:; break;
    }
  } else if (_subType == kSplitI) {
    switch (_dir) {
      case WE:; case EW:; building->m_next[0]    = above; building->m_next[1]    = below; 
                          building->m_nextDir[0] = SN;    building->m_nextDir[1] = NS; break;
      case SN:; case NS:; building->m_next[0]    = left;  building->m_next[1]    = right;
                          building->m_nextDir[0] = EW;    building->m_nextDir[1] = WE; break;
      case kConvDirN:; break;
    }
  } else if (_subType == kSplitL) {
    switch (_dir) {
      case SN:; building->m_next[0]    = above; building->m_next[1]    = right;
                building->m_nextDir[0] = SN;    building->m_nextDir[1] = WE; break;
      case WE:; building->m_next[0]    = right; building->m_next[1]    = below;
                building->m_nextDir[0] = WE;    building->m_nextDir[1] = NS; break;
      case NS:; building->m_next[0]    = below; building->m_next[1]    = left;
                building->m_nextDir[0] = NS;    building->m_nextDir[1] = EW; break;
      case EW:; building->m_next[0]    = left;  building->m_next[1]    = above;
                building->m_nextDir[0] = EW;    building->m_nextDir[1] = SN; break;
      case kConvDirN:; break;
    }
  } else if (_subType == kSplitT) {
    switch (_dir) {
      case SN:; building->m_next[0]    = left;  building->m_next[1]    = above;  building->m_next[2]    = right;
                building->m_nextDir[0] = EW;    building->m_nextDir[1] = SN;     building->m_nextDir[2] = WE; break;
      case WE:; building->m_next[0]    = above; building->m_next[1]    = right;  building->m_next[2]    = below;
                building->m_nextDir[0] = SN;    building->m_nextDir[1] = WE;     building->m_nextDir[2] = NS; break;
      case NS:; building->m_next[0]    = right; building->m_next[1]    = below;  building->m_next[2]    = left; 
                building->m_nextDir[0] = WE;    building->m_nextDir[1] = NS;     building->m_nextDir[2] = EW; break;
      case EW:; building->m_next[0]    = below; building->m_next[1]    = left;   building->m_next[2]    = above;
                building->m_nextDir[0] = NS;    building->m_nextDir[1] = EW;     building->m_nextDir[2] = SN; break;
      case kConvDirN:; break;
    }
  }

  // Add to the active/render list
  if (newToChunk) {
    chunkAddLocation(_loc->m_chunk, _loc); // Careful, no de-duplication in here, for speed
  }

  //pd->system->logToConsole("ADD TO CHUNK: %i %i with %i sprites", chunk->m_x, chunk->m_y, chunk->m_nLocations);

  // Bake into the background 
  renderChunkBackgroundImage(_loc->m_chunk);
  updateRenderList();

  return true;
}

void resetBuilding() {
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      pd->sprite->freeSprite(m_buildings[i].m_sprite[zoom]);
    }
  }
  memset(m_buildings, 0, SIZE_BUILDING);
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    m_buildings[i].m_index = i;
  }
}

void initBuilding() {
  m_buildings = pd->system->realloc(NULL, SIZE_BUILDING);
  memset(m_buildings, 0, SIZE_BUILDING);
  pd->system->logToConsole("malloc: for buildings %i", SIZE_BUILDING/1024);
}

void serialiseBuilding(struct json_encoder* je) {
  je->addTableMember(je, "building", 8);
  je->startArray(je);

  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    if (m_buildings[i].m_type == kNoBuilding) {
      continue;
    }

    je->addArrayMember(je);
    je->startTable(je);
    je->addTableMember(je, "idx", 3);
    je->writeInt(je, i);
    je->addTableMember(je, "type", 4);
    je->writeInt(je, m_buildings[i].m_type);
    je->addTableMember(je, "dir", 3);
    je->writeInt(je, m_buildings[i].m_convDir);
    je->addTableMember(je, "stype", 5);
    je->writeInt(je, m_buildings[i].m_convSubType);
    je->addTableMember(je, "x", 1);
    je->writeInt(je, m_buildings[i].m_pix_x);
    je->addTableMember(je, "y", 1);
    je->writeInt(je, m_buildings[i].m_pix_y);
    je->addTableMember(je, "prog", 4);
    je->writeInt(je, m_buildings[i].m_progress);
    je->addTableMember(je, "mode", 4);
    je->writeInt(je, m_buildings[i].m_mode);
    je->endTable(je);
  }

  je->endArray(je);
}