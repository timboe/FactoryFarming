#include "building.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "constants.h"
#include "player.h"
#include "ui.h"
#include "buildings/conveyor.h"
#include "buildings/plant.h"
#include "buildings/extractor.h"
#include "buildings/special.h"
#include "buildings/factory.h"
#include "buildings/utility.h"

const int32_t SIZE_BUILDING = TOT_CARGO_OR_BUILDINGS * sizeof(struct Building_t);

uint16_t m_nBuildings = 0;

uint16_t m_nByType[kNBuildingTypes] = {0};

uint16_t m_buildingSearchLocation = 0;

uint16_t m_deserialiseIndexBuilding = 0;

struct Building_t* m_buildings;

// Note: User can never themselves build a kTunnelOut
//                             {kBelt,    kSplitI,  kSplitL,  kSplitT,  kTunnelIn, kTunnelOut, kFilterI,  kFilterL, kNConvSubTypes};
const uint32_t kConvUnlock[] = {0,        0,        0,        0,        0,         UINT32_MAX, 0,         0};
const uint16_t kConvPrice[]  = {1,        1,        1,        1,        1,         1,          1,         1};
const uint16_t kConvUIIcon[] = {SID(0,3), SID(0,4), SID(4,3), SID(4,4), SID(8,3),  SID(8,3),   SID(12,4), SID(8,4)};

//                                          {kCarrotPlant, kAppleTree,    kWheatPlant,  kP4,          klP5,         kP6,          kP7,          kP8,          kP9,          kP10,         kP11,         kP12};
const uint32_t kPlantUnlock[]             = {0,            0,             0,            0,            0,            0,            0,            0,            0,            0,            0,            0};
const uint16_t kPlantPrice[]              = {0,            1,             1,            1,            1,            1,            1,            1,            1,            1,            1,            1};
const uint16_t kPlantUIIcon[]             = {SID(10,7),    SID(8,7),      SID(11,7),    SID(9,7),     SID(9,7),     SID(9,7),     SID(9,7),     SID(9,7),     SID(9,7),     SID(9,7),     SID(9,7),     SID(9,7)};
const enum kGroundWetness kPlantWetness[] = {kWet,         kMoist,        kDry,         kWet,         kMoist,       kDry,         kWet,         kMoist,       kDry,         kWet,         kMoist,       kDry};
const enum kGroundType kPlantSoil[]       = {kSiltyGround, kChalkyGround, kPeatyGround, kSandyGround, kSandyGround, kSandyGround, kSandyGround, kSandyGround, kSandyGround, kSandyGround, kSandyGround, kSandyGround};

//                                {kWell,     kStorageBox, kConveyorGrease, kNUtilitySubTypes};
const uint32_t kUtilityUnlock[] = {0,         0,           0};
const uint16_t kUtilityPrice[]  = {1,         1,           1};
const uint16_t kUtilityUIIcon[] = {SID(0,15), SID(4,15),   SID(8,15)};

//                                  {kCropHarvesterSmall, kPump,      kChalkQuarry, kkCropHarvesterLarge, kNExtractorSubTypes};
const uint32_t kExtractorUnlock[] = {0,                   0,          0,            0};
const uint16_t kExtractorPrice[]  = {1,                   1,          1,            1};
const uint16_t kExtractorUIIcon[] = {SID(4,2),            SID(12,11), SID(12,12),   SID(4,2)};
const uint16_t kExtractorSprite[] = {BID(0,0),            BID(0,3),   BID(0,5),     BID(0,0)};

//                                  {kVitiminFac, kNFactorySubTypes};
const uint32_t kFactoryUnlock[]     = {0};
const uint16_t kFactoryPrice[]      = {1};
const uint16_t kFactoryTime[]       = {1};
const uint16_t kFactorySprite[]     = {BID(0,4)};
const enum kCargoType kFactoryOut[] = {kVitamin};
const enum kCargoType kFactoryIn1[] = {kCarrot};
const enum kCargoType kFactoryIn2[] = {kChalk};
const enum kCargoType kFactoryIn3[] = {kNoCargo};
const enum kCargoType kFactoryIn4[] = {kNoCargo};
const enum kCargoType kFactoryIn5[] = {kNoCargo};

void buildingSetup(struct Building_t* _building);

void assignNeighbors(struct Building_t* _building);

void assignUpdate(struct Building_t* _building);

void setBuildingSubType(struct Building_t* _building, union kSubType _subType);


/// ///

const char* toStringBuilding(enum kBuildingType _type, union kSubType _subType, bool _inworld) {
  switch(_type) {
    case kNoBuilding: return "NoBuilding";
    case kConveyor: switch (_subType.conveyor) {
      case kBelt: return _inworld ? "Belt" : "Conveyor Belt";
      case kSplitI: return _inworld ? "'I' Spliter": "Conveyor Belt 'I' Spliter";
      case kSplitL: return _inworld ? "'L' Splitter" : "Conveyor Belt 'L' Splitter";
      case kSplitT: return _inworld ? "'T' Splitter" : "Conveyor Belt 'T' Splitter";
      case kFilterI: return _inworld ? "'I' Filter" : "Conveyor Belt 'I' Filter";
      case kFilterL: return _inworld ? "'L' Filter" : "Conveyor Belt 'L' Filter";
      case kTunnelIn: case kTunnelOut: return _inworld ? "Tunnel" : "Conveyor Belt Tunnel";
      case kNConvSubTypes: return "";
    }
    case kPlant: switch (_subType.plant) {
      case kCarrotPlant: return _inworld ? "Carrot Plant" : "Carrot Seeds";
      case kAppleTree: return _inworld ? "Apple Tree" : "Apple Sapling";
      case kWheatPlant: return _inworld ? "Wheat Plant" : "Wheet Seeds";
      case kP4:; case kP5:; case kP6:; case kP7:; case kP8:; case kP9:; case kP10:; case kP11:; case kP12:; return "Placeholder";
      case kNPlantSubTypes: return "";
    }
    case kUtility: switch (_subType.utility) {
      case kWell: return "Well";
      case kStorageBox: return "Storage Box";
      case kConveyorGrease: return "Conveyor Grease";
      case kNUtilitySubTypes: return "";
    }
    case kExtractor: switch (_subType.extractor) {
      case kCropHarvesterSmall: return _inworld ? "Harvester 7x7" : "Automatic Harvester (7x7)";
      case kPump: return "Water Pump";
      case kChalkQuarry: return "Chalk Quarry";
      case kCropHarvesterLarge: return _inworld ? "Harvester 9x9 " : "Automatic Harvester (9x9)";
      case kNExtractorSubTypes: return "";
    }
    case kFactory: switch (_subType.factory) {
      case kVitiminFac: return "Vitamin Factory";
      case kNFactorySubTypes: return "";
    }
    case kSpecial: switch (_subType.special) {
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
    case kFactory:; return assignNeighborsFactory(_building);;
    case kUtility:; return assignNeighborsUtility(_building);
    case kSpecial:; return assignNeighborsSpecial(_building);
    case kNBuildingTypes:  case kNoBuilding:; break;
  };
}

void buildingSetup(struct Building_t* _building) {
  switch (_building->m_type) {
    case kConveyor:; return buildingSetupConveyor(_building);
    case kPlant:; return buildingSetupPlant(_building);
    case kUtility:; return buildingSetupUtility(_building);
    case kExtractor:; return buildingSetupExtractor(_building);
    case kFactory:; return buildingSetupFactory(_building);
    case kSpecial:; return buildingSetupSpecial(_building);
    case kNBuildingTypes:; case kNoBuilding:; break;
  };
}

void assignUpdate(struct Building_t* _building) {
  switch (_building->m_type) {
    case kConveyor:; _building->m_updateFn = &conveyorUpdateFn; break;
    case kPlant:; _building->m_updateFn = &plantUpdateFn; break;
    case kUtility:; _building->m_updateFn = &utilityUpdateFn; break;
    case kExtractor:; _building->m_updateFn = &extractorUpdateFn; break;
    case kFactory:; _building->m_updateFn = &factoryUpdateFn; break;
    case kSpecial:; _building->m_updateFn = &specialUpdateFn; break;
    case kNBuildingTypes:; case kNoBuilding:; break;
  }
}

void setBuildingSubType(struct Building_t* _building, union kSubType _subType) {
  switch (_building->m_type) {
    case kConveyor:; _building->m_subType.conveyor = _subType.conveyor; break;
    case kPlant:; _building->m_subType.plant = _subType.plant; break;
    case kUtility:; _building->m_subType.utility = _subType.utility; break;
    case kExtractor:; _building->m_subType.extractor = _subType.extractor; break;
    case kSpecial:; _building->m_subType.special = _subType.special; break;
    case kFactory:; _building->m_subType.factory = _subType.factory; break;
    case kNoBuilding:; case kNBuildingTypes:; break;
  }
}

bool newBuilding(struct Location_t* _loc, enum kDir _dir, enum kBuildingType _type, union kSubType _subType) {
  bool canBePlaced = false;
  switch (_type) {
    case kConveyor:; canBePlaced = canBePlacedConveyor(_loc, _dir, _subType); break;
    case kPlant:;  canBePlaced = canBePlacedPlant(_loc, _dir, _subType); break;
    case kUtility:; canBePlaced = canBePlacedUtility(_loc, _dir, _subType); break;
    case kExtractor:;  canBePlaced = canBePlacedExtractor(_loc, _dir, _subType); break;
    case kSpecial:;  canBePlaced = canBePlacedSpecial(_loc, _dir, _subType); break;
    case kFactory:;  canBePlaced = canBePlacedFactory(_loc, _dir, _subType); break;
    case kNoBuilding:; case kNBuildingTypes:; break;
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

  setBuildingSubType(building, _subType);
  building->m_dir = _dir;
  building->m_progress = 0;
  building->m_mode = 0;
  for (int32_t i = 0; i < MAX_STORE; ++i) {
    // Don't reset this for speedy conveyors, we only apply the auto-speed boost once
    if (_type == kConveyor && i == 0 && building->m_stored[0] == 2) continue;
    building->m_stored[i] = 0;
  }
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

    // Special - test auto upgrade of conveyor belts
    if (_type == kConveyor && getPlayer()->m_autoUseConveyorBooster && getOwned(kUICatUtility, kConveyorGrease)) {
      modOwned(kUICatUtility, kConveyorGrease, /*add*/ false);
      building->m_stored[0] = 2;
    }
  }

  // Special - well
  bool wideRedraw = false;
  if (_type == kUtility && _subType.utility == kWell) {
    struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
    building->m_mode = t->m_tile;
    t->m_tile = SPRITE16_ID(4,14);
    doWetness();
    wideRedraw = true;
  }

  // Bake into the background
  if (_type >= kExtractor || wideRedraw) {
    renderChunkBackgroundImageAround(_loc->m_chunk);
  } else {
    renderChunkBackgroundImage(_loc->m_chunk);
  }

  // Special - If placing an input tunnel, then also place the output tunnel too (canBePlacedConveyor will have checked that this is all OK)
  if (_type == kConveyor && _subType.conveyor == kTunnelIn) {
    struct Location_t* tunnelOut = getTunnelOutLocation(_loc, _dir);
    newBuilding(tunnelOut, _dir, kConveyor, (union kSubType) {.conveyor = kTunnelOut});
  }

  updateRenderList();

  return newToChunk;
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
      je->addTableMember(je, "s5", 2);
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
  //  m_nBuildings, m_deserialiseIndexBuilding, toStringBuilding(building->m_type, building->m_subType, true),
  //  building->m_pix_x, building->m_pix_y, building->m_dir, building->m_subType.conveyor, building->m_progress, building->m_mode,
  //  building->m_stored[0], building->m_stored[1], building->m_stored[2], building->m_stored[3], building->m_stored[4]
  //);

  return NULL;
}
