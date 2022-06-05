#include "building.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "constants.h"

const int32_t SIZE_BUILDING = TOT_CARGO_OR_BUILDINGS * sizeof(struct Building_t);

uint16_t m_nBuildings = 0;

uint16_t m_nByType[kNBuildingTypes] = {0};

uint16_t m_buildingSearchLocation = 0;

uint16_t m_deserialiseIndexBuilding = 0;

struct Building_t* m_buildings;

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void buildingSpriteSetup(struct Building_t* _building);

void assignNeighbors(struct Building_t* _building);

void assignUpdate(struct Building_t* _building);

const char* toStringBuilding(enum kCargoType _type);

/// ///

const char* toStringBuilding(enum kCargoType _type) {
  switch(_type) {
    case kNoBuilding: return "NoBuilding";
    case kConveyor: return "Conveyor";
    default: return "Building???";
  }
}

uint16_t getNBuildings() {
  return m_nBuildings;
}

uint16_t getNConveyors() {
  return m_nByType[kConveyor];
}

struct Building_t* buildingManagerNewBuilding(enum kBuildingType _asType) {
  for (uint8_t try = 0; try < 2; ++try) {
    const uint32_t start = (try == 0 ? m_buildingSearchLocation : 0);
    const uint32_t stop  = (try == 0 ? TOT_CARGO_OR_BUILDINGS : m_buildingSearchLocation);
    for (uint32_t i = start; i < stop; ++i) {
      if (m_buildings[i].m_type == kNoBuilding) {
        ++m_nBuildings;
        ++(m_nByType[_asType]);
        ++m_buildingSearchLocation;
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

struct Building_t* buildingManagerGetByIndex(uint16_t _index) {
  return &(m_buildings[_index]);
}

void buildingManagerFreeBuilding(struct Building_t* _building) {
  --(m_nByType[_building->m_type]);
  --m_nBuildings;
  _building->m_type = kNoBuilding;
  _building->m_updateFn = NULL;
  m_buildingSearchLocation = _building->m_index;
}

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo == NULL) return;
  if (_building->m_progress < TILE_PIX) {
    _building->m_progress += _tick;
    switch (_building->m_nextDir[_building->m_mode]) {
      case SN:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], _building->m_pix_x*_zoom, (_building->m_pix_y - _building->m_progress)*_zoom); break;
      case NS:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], _building->m_pix_x*_zoom, (_building->m_pix_y + _building->m_progress)*_zoom); break;
      case EW:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], (_building->m_pix_x - _building->m_progress)*_zoom, _building->m_pix_y*_zoom); break;
      case WE:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], (_building->m_pix_x + _building->m_progress)*_zoom, _building->m_pix_y*_zoom); break;
      case kDirN:; break;
    }
    //pd->system->logToConsole("MOVE %i %i, %i", loc->m_pix_x, loc->m_pix_y, loc->m_progress);
  }
  if (_building->m_progress >= TILE_PIX && _building->m_next[_building->m_mode]->m_cargo == NULL) {
    struct Cargo_t* theCargo = loc->m_cargo;
    struct Location_t* nextLoc = _building->m_next[_building->m_mode]; 
    // Move cargo
    nextLoc->m_cargo = theCargo;
    loc->m_cargo = NULL;
    // Carry over any excess ticks
    if (nextLoc->m_building) {
      nextLoc->m_building->m_progress = _building->m_progress - TILE_PIX;
    }
    // Cargo moves between chunks?
    if (nextLoc->m_chunk != loc->m_chunk) {
      //pd->system->logToConsole("CHANGE CHUNK");
      chunkRemoveCargo(loc->m_chunk, theCargo);
      chunkAddCargo(nextLoc->m_chunk, theCargo);
      queueUpdateRenderList();
    }
    // Cycle outputs
    switch (_building->m_subType.conveyor) {
      case kSplitI:; case kSplitL:; case kFilterL:; _building->m_mode = (_building->m_mode + 1) % 2; break;
      case kSplitT:; _building->m_mode = (_building->m_mode + 1) % 3; break;
      case kBelt:; default: break;
    }
  }
}

void buildingSpriteSetup(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {

    if (_building->m_type == kConveyor) {
      switch (_building->m_subType.conveyor) {
        case kBelt:;   _building->m_image[zoom] = getSprite16(0, CONV_START_Y + _building->m_dir, zoom); break;
        case kSplitI:; _building->m_image[zoom] = getSprite16(0, 11 + _building->m_dir, zoom); break;
        case kSplitL:; _building->m_image[zoom] = getSprite16(1, 11 + _building->m_dir, zoom); break;
        case kSplitT:; _building->m_image[zoom] = getSprite16(2, 11 + _building->m_dir, zoom); break;
        case kFilterL:; default: break;
      }
    }

    if (_building->m_type == kConveyor && _building->m_subType.conveyor == kBelt) {
      PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
      if (_building->m_sprite[zoom] == NULL) {
        _building->m_sprite[zoom] = pd->sprite->newSprite();
      }
      pd->sprite->setBounds(_building->m_sprite[zoom], bound);
      pd->sprite->setImage(_building->m_sprite[zoom], getConveyorMaster(zoom, _building->m_dir), kBitmapUnflipped);
      pd->sprite->moveTo(_building->m_sprite[zoom], _building->m_pix_x*zoom, _building->m_pix_y*zoom);
      pd->sprite->setZIndex(_building->m_sprite[zoom], Z_INDEX_CONVEYOR);
    }
  }
}

void assignNeighbors(struct Building_t* _building) {
  uint16_t locX, locY;
  locX = pixToLoc(_building->m_pix_x);
  locY = pixToLoc(_building->m_pix_y);
  _building->m_location = getLocation(locX, locY);
  struct Location_t* above = getLocation(locX, locY - 1);
  struct Location_t* below = getLocation(locX, locY + 1);
  struct Location_t* left  = getLocation(locX - 1, locY);
  struct Location_t* right = getLocation(locX + 1, locY);
  if (_building->m_subType.conveyor == kBelt) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = above;
                _building->m_nextDir[0] = SN; break;
      case NS:; _building->m_next[0]    = below;
                _building->m_nextDir[0] = NS; break;
      case WE:; _building->m_next[0]    = right;
                _building->m_nextDir[0] = WE; break;
      case EW:; _building->m_next[0]    = left;
                _building->m_nextDir[0] = EW; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kSplitI) {
    switch (_building->m_dir) {
      case WE:; case EW:; _building->m_next[0]    = above; _building->m_next[1]    = below; 
                          _building->m_nextDir[0] = SN;    _building->m_nextDir[1] = NS; break;
      case SN:; case NS:; _building->m_next[0]    = left;  _building->m_next[1]    = right;
                          _building->m_nextDir[0] = EW;    _building->m_nextDir[1] = WE; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kSplitL) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = above; _building->m_next[1]    = right;
                _building->m_nextDir[0] = SN;    _building->m_nextDir[1] = WE; break;
      case WE:; _building->m_next[0]    = right; _building->m_next[1]    = below;
                _building->m_nextDir[0] = WE;    _building->m_nextDir[1] = NS; break;
      case NS:; _building->m_next[0]    = below; _building->m_next[1]    = left;
                _building->m_nextDir[0] = NS;    _building->m_nextDir[1] = EW; break;
      case EW:; _building->m_next[0]    = left;  _building->m_next[1]    = above;
                _building->m_nextDir[0] = EW;    _building->m_nextDir[1] = SN; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kSplitT) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = left;  _building->m_next[1]    = above;  _building->m_next[2]    = right;
                _building->m_nextDir[0] = EW;    _building->m_nextDir[1] = SN;     _building->m_nextDir[2] = WE; break;
      case WE:; _building->m_next[0]    = above; _building->m_next[1]    = right;  _building->m_next[2]    = below;
                _building->m_nextDir[0] = SN;    _building->m_nextDir[1] = WE;     _building->m_nextDir[2] = NS; break;
      case NS:; _building->m_next[0]    = right; _building->m_next[1]    = below;  _building->m_next[2]    = left; 
                _building->m_nextDir[0] = WE;    _building->m_nextDir[1] = NS;     _building->m_nextDir[2] = EW; break;
      case EW:; _building->m_next[0]    = below; _building->m_next[1]    = left;   _building->m_next[2]    = above;
                _building->m_nextDir[0] = NS;    _building->m_nextDir[1] = EW;     _building->m_nextDir[2] = SN; break;
      case kDirN:; break;
    }
  }
}

void assignUpdate(struct Building_t* _building) {
  switch (_building->m_type) {
    case kConveyor:; _building->m_updateFn = &conveyorUpdateFn; break;
    default: break;
  }
}

bool newConveyor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
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

  building->m_subType.conveyor = _subType.conveyor;
  building->m_dir = _dir;
  building->m_progress = 0;
  building->m_mode = 0;
  building->m_pix_x = locToPix(_loc->m_x);
  building->m_pix_y = locToPix(_loc->m_y);  

  buildingSpriteSetup(building);
  assignNeighbors(building);
  assignUpdate(building);

  // Add to the active/render list
  if (newToChunk) {
    chunkAddBuilding(_loc->m_chunk, building); // Careful, no de-duplication in here, for speed
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
      if (m_buildings[i].m_sprite[zoom]) pd->sprite->freeSprite(m_buildings[i].m_sprite[zoom]);
    }
  }
  memset(m_buildings, 0, SIZE_BUILDING);
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    m_buildings[i].m_index = i;
  }
  m_deserialiseIndexBuilding = 0;
  m_buildingSearchLocation = 0;
  m_nBuildings = 0;
  for (uint8_t i = 0; i < kNBuildingTypes; ++i) m_nByType[i] = 0;
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
    if (m_buildings[i].m_dir) {
      je->addTableMember(je, "dir", 3);
      je->writeInt(je, m_buildings[i].m_dir);
    }
    if (m_buildings[i].m_subType.conveyor) { // Which union member we access does not matter here
      je->addTableMember(je, "stype", 5);
      je->writeInt(je, m_buildings[i].m_subType.conveyor);
    }
    je->addTableMember(je, "x", 1);
    je->writeInt(je, m_buildings[i].m_pix_x);
    je->addTableMember(je, "y", 1);
    je->writeInt(je, m_buildings[i].m_pix_y);
    if (m_buildings[i].m_progress) {
      je->addTableMember(je, "prog", 4);
      je->writeInt(je, m_buildings[i].m_progress);
    }
    if (m_buildings[i].m_mode) {
      je->addTableMember(je, "mode", 4);
      je->writeInt(je, m_buildings[i].m_mode);
    }
    if (m_buildings[i].m_stored[0]) {
      je->addTableMember(je, "s0", 2);
      je->writeInt(je, m_buildings[i].m_stored[0]);
    }
    if (m_buildings[i].m_stored[0]) {
      je->addTableMember(je, "s1", 2);
      je->writeInt(je, m_buildings[i].m_stored[1]);
    }
    if (m_buildings[i].m_stored[0]) {
      je->addTableMember(je, "s2", 2);
      je->writeInt(je, m_buildings[i].m_stored[2]);
    }
    if (m_buildings[i].m_stored[0]) {
      je->addTableMember(je, "s3", 2);
      je->writeInt(je, m_buildings[i].m_stored[3]);
    }
    if (m_buildings[i].m_stored[0]) {
      je->addTableMember(je, "s4", 2);
      je->writeInt(je, m_buildings[i].m_stored[4]);
    }
    je->endTable(je);
  }

  je->endArray(je);
}


void deserialiseValueBuilding(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "idx") == 0) {
    m_deserialiseIndexBuilding = json_intValue(_value);
  } else if (strcmp(_key, "type") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_type = json_intValue(_value);
  } else if (strcmp(_key, "dir") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_dir = json_intValue(_value);
  } else if (strcmp(_key, "stype") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_subType.conveyor = json_intValue(_value); // The union type does not matter here
  } else if (strcmp(_key, "x") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_pix_x = json_intValue(_value);
  } else if (strcmp(_key, "y") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_pix_y = json_intValue(_value);
  } else if (strcmp(_key, "prog") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_progress = json_intValue(_value);
  } else if (strcmp(_key, "mode") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_mode = json_intValue(_value);
  } else if (strcmp(_key, "s0") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_stored[0] = json_intValue(_value);
  } else if (strcmp(_key, "s1") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_stored[1] = json_intValue(_value);
  } else if (strcmp(_key, "s2") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_stored[2] = json_intValue(_value);
  } else if (strcmp(_key, "s3") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_stored[3] = json_intValue(_value);
  } else if (strcmp(_key, "s4") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_stored[4] = json_intValue(_value);
  } else {
    pd->system->error("BUILDING DECODE ISSUE, %s", _key);
  }
}

void* deserialiseStructDoneBuilding(json_decoder* jd, const char* _name, json_value_type _type) {
  struct Building_t* building = &(m_buildings[m_deserialiseIndexBuilding]);

  buildingSpriteSetup(building);
  assignNeighbors(building);
  assignUpdate(building);

  ++m_nBuildings;
  ++(m_nByType[building->m_type]);

  pd->system->logToConsole("-- Building #%i [%i] decoded to %s, (%i, %i) dir:%i stype:%i prog:%i mode:%i store:[%i][%i, %i, %i, %i]",
    m_nBuildings, m_deserialiseIndexBuilding, toStringBuilding(building->m_type),
    building->m_pix_x, building->m_pix_y, building->m_dir, building->m_subType.conveyor, building->m_progress, building->m_mode,
    building->m_stored[0], building->m_stored[1], building->m_stored[2], building->m_stored[3], building->m_stored[4]
  );

  return NULL;
}
