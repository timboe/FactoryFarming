#include "building.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "constants.h"
#include "buildings/conveyor.h"
#include "buildings/plant.h"
#include "buildings/extractor.h"
#include "buildings/special.h"

const int32_t SIZE_BUILDING = TOT_CARGO_OR_BUILDINGS * sizeof(struct Building_t);

uint16_t m_nBuildings = 0;

uint16_t m_nByType[kNBuildingTypes] = {0};

uint16_t m_buildingSearchLocation = 0;

uint16_t m_deserialiseIndexBuilding = 0;

struct Building_t* m_buildings;


void buildingSetup(struct Building_t* _building);

void assignNeighbors(struct Building_t* _building);

void assignUpdate(struct Building_t* _building);


/// ///

const char* toStringBuilding(struct Building_t* _building) {
  if (!_building) return "";
  switch(_building->m_type) {
    case kNoBuilding: return "NoBuilding";
    case kConveyor: switch (_building->m_subType.conveyor) {
      case kBelt: return "Conveyor Belt";
      case kSplitI: return "Conveyor 'I' Spliter";
      case kSplitL: return "Conveyor 'L' Splitter";
      case kSplitT: return "Conveyor 'T' Splitter";
      case kFilterL: return "Conveyor 'L' Filter";
      case kNConvSubTypes: return "";
    }
    case kPlant: switch (_building->m_subType.plant) {
      case kCarrotPlant: return "Carrot Plant";
      case kAppleTree: return "Apple Tree";
      case kWheatPlant: return "Wheat Plant";
      case kNPlantSubTypes: return "";
    }
    case kExtractor: switch (_building->m_subType.extractor) {
      case kCropHarvester: return "Automatic Harvester";
      case kPump: return "Water Pump";
      case kNExtractorSubTypes: return "";
    }
    case kFactory: switch (_building->m_subType.factory) {
      case kMakeSmall: return "Placeholder";
      case kNFactorySubTypes: return "";
    }
    case kSpecial: switch (_building->m_subType.special) {
      case kShop: return "The Shop";
      case kSellBox: return "Sell Box";
      case kExportBox: return "Item Exporter";
      case kImportBox: return "Item Importer";
      case kWarp: return "Warp Portal";
      case kNSpecialSubTypes: return "";
    }
    case kNBuildingTypes: return "";
  }
  return "";
}

uint16_t getNBuildings() {
  return m_nBuildings;
}

uint16_t getNByType(enum kBuildingType _type) {
  return m_nByType[_type];
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

void getBuildingNeighbors(struct Building_t* _building, int8_t _offset, struct Location_t** _above, struct Location_t** _below, struct Location_t** _left, struct Location_t** _right) {
  uint16_t locX = _building->m_location->m_x;
  uint16_t locY = _building->m_location->m_y;;
  (*_above) = getLocation(locX, locY - _offset);
  (*_below) = getLocation(locX, locY + _offset);
  (*_left)  = getLocation(locX - _offset, locY);
  (*_right) = getLocation(locX + _offset, locY);
}

void assignNeighbors(struct Building_t* _building) {
  _building->m_location = getLocation(pixToLoc(_building->m_pix_x), pixToLoc(_building->m_pix_y));
  switch (_building->m_type) {
    case kConveyor:; return assignNeighborsConveyor(_building);
    case kPlant:; return assignNeighborsPlant(_building);
    case kExtractor:; return assignNeighborsExtractor(_building);
    case kSpecial:; return assignNeighborsSpecial(_building);
    default: break;
  };
}

void buildingSetup(struct Building_t* _building) {
  switch (_building->m_type) {
    case kConveyor:; return buildingSetupConveyor(_building);
    case kPlant:; return buildingSetupPlant(_building);
    case kExtractor:; return buildingSetupExtractor(_building);
    case kSpecial:; return buildingSetupSpecial(_building);
    default: break;
  };
}

void assignUpdate(struct Building_t* _building) {
  switch (_building->m_type) {
    case kConveyor:; _building->m_updateFn = &conveyorUpdateFn; break;
    case kPlant:; _building->m_updateFn = &plantUpdateFn; break;
    case kExtractor:; _building->m_updateFn = &extractorUpdateFn; break;
    case kSpecial:; _building->m_updateFn = &specialUpdateFn; break;
    default: break;
  }
}

bool newBuilding(struct Location_t* _loc, enum kDir _dir, enum kBuildingType _type, union kSubType _subType) {
  bool canBePlaced = false;
  switch (_type) {
    case kConveyor:; canBePlaced = canBePlacedConveyor(_loc); break;
    case kPlant:;  canBePlaced = canBePlacedPlant(_loc); break;
    case kExtractor:;  canBePlaced = canBePlacedExtractor(_loc); break;
    case kSpecial:;  canBePlaced = canBePlacedSpecial(_loc); break;
    case kFactory:;  break;
    case kNoBuilding:; case kNBuildingTypes:; canBePlaced = false; break;
  }
  if (!canBePlaced) return false;

  bool newToChunk = false;
  struct Building_t* building = _loc->m_building;
  if (!building) {
    building = buildingManagerNewBuilding(_type);
    newToChunk = true;
    if (!building) {
      // Run out of buildings
      return false;
    }
    _loc->m_building = building;
    _loc->m_notOwned = false;
    if (_type >= kExtractor) { // Add to neighbors too
      for (int32_t x = -1; x < 2; ++x) {
        for (int32_t y = -1; y < 2; ++y) {
          if (!x && !y) continue;
          struct Location_t* otherLoc = getLocation(_loc->m_x + x, _loc->m_y + y);
          otherLoc->m_building = building;
          otherLoc->m_notOwned = true;
        }
      }
    }
  }


  switch (building->m_type) {
    case kConveyor:; building->m_subType.conveyor = _subType.conveyor; break;
    case kPlant:; building->m_subType.plant = _subType.plant; break;
    case kExtractor:; building->m_subType.extractor = _subType.extractor; break;
    case kSpecial:; building->m_subType.special = _subType.special; break;
    case kFactory:; building->m_subType.factory = _subType.factory; break;
    case kNoBuilding:; case kNBuildingTypes:; break;
  }
  building->m_dir = _dir;
  building->m_progress = 0;
  building->m_mode = 0;
  for (int32_t i = 0; i < MAX_STORE; ++i) building->m_stored[i] = 0;
  for (int32_t i = 0; i < 4; ++i) {
    building->m_next[i] = NULL;
    building->m_nextDir[i] = SN;
  }
  building->m_pix_x = locToPix(_loc->m_x);
  building->m_pix_y = locToPix(_loc->m_y);  
  // building->m_location = _loc; This is done by assignUpdate, as it needs to be done for deserialised buildings too

  assignNeighbors(building); // Do this first
  buildingSetup(building);
  assignUpdate(building);

  // Add to the active/render list
  if (newToChunk) {
    chunkAddBuilding(_loc->m_chunk, building); // Careful, no de-duplication in here, for speed
  }

  // Bake into the background
  if (_type >= kExtractor) {
    renderChunkBackgroundImageAround(_loc->m_chunk);
  } else {
    renderChunkBackgroundImage(_loc->m_chunk);
  }

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
    if (m_buildings[i].m_stored[1]) {
      je->addTableMember(je, "s1", 2);
      je->writeInt(je, m_buildings[i].m_stored[1]);
    }
    if (m_buildings[i].m_stored[2]) {
      je->addTableMember(je, "s2", 2);
      je->writeInt(je, m_buildings[i].m_stored[2]);
    }
    if (m_buildings[i].m_stored[3]) {
      je->addTableMember(je, "s3", 2);
      je->writeInt(je, m_buildings[i].m_stored[3]);
    }
    if (m_buildings[i].m_stored[4]) {
      je->addTableMember(je, "s4", 2);
      je->writeInt(je, m_buildings[i].m_stored[4]);
    }
    if (m_buildings[i].m_stored[5]) {
      je->addTableMember(je, "s4", 2);
      je->writeInt(je, m_buildings[i].m_stored[5]);
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
  } else if (strcmp(_key, "s5") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_stored[5] = json_intValue(_value); 
  } else {
    pd->system->error("BUILDING DECODE ISSUE, %s", _key);
  }
}

void* deserialiseStructDoneBuilding(json_decoder* jd, const char* _name, json_value_type _type) {
  struct Building_t* building = &(m_buildings[m_deserialiseIndexBuilding]);

  assignNeighbors(building);
  buildingSetup(building);
  assignUpdate(building);

  ++m_nBuildings;
  ++(m_nByType[building->m_type]);

  //pd->system->logToConsole("-- Building #%i [%i] decoded to %s, (%i, %i) dir:%i stype:%i prog:%i mode:%i store:[%i][%i, %i, %i, %i]",
  //  m_nBuildings, m_deserialiseIndexBuilding, toStringBuilding(building),
  //  building->m_pix_x, building->m_pix_y, building->m_dir, building->m_subType.conveyor, building->m_progress, building->m_mode,
  //  building->m_stored[0], building->m_stored[1], building->m_stored[2], building->m_stored[3], building->m_stored[4]
  //);

  return NULL;
}
