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
      case kSplitI: return _inworld ? "'I' Splitter": "Conveyor Belt 'I' Splitter";
      case kSplitL: return _inworld ? "'L' Splitter" : "Conveyor Belt 'L' Splitter";
      case kSplitT: return _inworld ? "'T' Splitter" : "Conveyor Belt 'T' Splitter";
      case kFilterI: return _inworld ? "'I' Filter" : "Conveyor Belt 'I' Filter";
      case kFilterL: return _inworld ? "'L' Filter" : "Conveyor Belt 'L' Filter";
      case kTunnelIn: case kTunnelOut: return _inworld ? "Tunnel" : "Conveyor Belt Tunnel";
      case kSpareConv0: case kSpareConv1: case kSpareConv2: case kSpareConv3:
      case kSpareConv4: case kSpareConv5: case kSpareConv6: case kSpareConv7: 
      case kNConvSubTypes: return "C_PLACEHOLDER";
    }
    case kPlant: switch (_subType.plant) {
      case kCarrotPlant: return _inworld ? "Carrot Plant" : "Carrot Seeds";
      case kPotatoPlant: return _inworld ? "Potato Plant" : "Seed Potato";
      case kSunflowerPlant: return _inworld ? "Sunflower Plant" : "Sunflower Seeds";
      case kAppleTree: return _inworld ? "Apple Tree" : "Apple Sapling";
      case kCornPlant: return _inworld ? "Corn Plant" : "Corn Seeds";
      case kSeaweedPlant: return _inworld ? "Seaweed Plant" : "Seaweed Spores";
      case kWheatPlant: return _inworld ? "Wheat Plant" : "Wheat Seeds";
      case kCactusPlant: return _inworld ? "Cactus Plant" : "Cactus Seeds";
      case kLimeTree: return _inworld ? "Lime Tree" : "Lime Sapling";
      case kStrawberryPlant: return _inworld ? "Strawberry Plant" : "Strawberry Seeds";
      case kBambooPlant: return _inworld ? "Bamboo Plant" : "Bamboo Sapling";
      case kCoffeePlant: return _inworld ? "Coffee Plant" : "Coffee Seeds";
      case kCocoaPlant: return _inworld ? "Cocoa Plant" : "Cocoa Seeds";
      case kSeaCucumberPlant: return _inworld ? "Sea Cucumber Farm" : "Sea Cucumber Trap";
      case kRosePlant: return _inworld ? "Rose Plant" : "Rose Seeds";
      case kSparePlant0: case kSparePlant1: case kSparePlant2: case kSparePlant3:
      case kSparePlant4: case kSparePlant5: case kSparePlant6: case kSparePlant7: 
      case kSparePlant8: case kSparePlant9: case kSparePlant10: case kSparePlant11:
      case kSparePlant12: case kSparePlant13: case kSparePlant14: case kSparePlant15: 
      case kNPlantSubTypes: return "P_PLACEHOLDER";
    }
    case kUtility: switch (_subType.utility) {
      case kPath: return "Path";
      case kSign: return "Sign";
      case kBin: return "Trash Bin";
      case kWell: return "Well";
      case kStorageBox: return "Storage Box";
      case kBuffferBox: return "Buffer Box";
      case kConveyorGrease: return "Conveyor Grease";
      case kObstructionRemover: return "Obstruction Remover";
      case kLandfill: return "Landfill";
      case kRetirement: return "Retirement Cottage";
      case kFence: return "Fence"; 
      case kSpareUtil0: case kSpareUtil1: case kSpareUtil2: case kSpareUtil3:
      case kSpareUtil4: case kSpareUtil5: case kSpareUtil6: case kSpareUtil7: 
      case kNUtilitySubTypes: return "U_PLACEHOLDER";
    }
    case kExtractor: switch (_subType.extractor) {
      case kCropHarvesterSmall: return _inworld ? "Harvester 7x7" : "Automatic Harvester (7x7)";
      case kPump: return "Water Pump";
      case kChalkQuarry: return "Chalk Quarry";
      case kSaltMine: return "Salt Mine";
      case kCropHarvesterLarge: return _inworld ? "Harvester 9x9 " : "Automatic Harvester (9x9)";
      case kCO2Extractor: return "CO2 Extractor";
      case kSpareExtractor0: case kSpareExtractor1: case kSpareExtractor2: case kSpareExtractor3:
      case kSpareExtractor4: case kSpareExtractor5: case kSpareExtractor6: case kSpareExtractor7: 
      case kNExtractorSubTypes: return "E_PLACEHOLDER";
    }
    case kFactory: switch (_subType.factory) {
      case kVitiminFac: return "Vitamin Factory";
      case kVegOilFac: return "Vegetable Oil Factory";
      case kCrispsFac: return "Potato Chip Factory";
      case kEthanolFac: return "Ethanol Factory";
      case kHardCiderFac: return "Hard Cider Factory";
      case kAbattoir: return "Abattoir";
      case kHydrogenFac: return "Hydrogen Factory";
      case kHOILFac: return "Hydrogenated Oil Factory";
      case kCornDogFac: return "Corn Dog Factory";
      case kBeerFac: return "Beer Brewery";
      case kTequilaFac: return "Tequila Factory";
      case kHFCSFac: return "High Fructose Corn Syrup Factory";
      case kGelatinFac: return "Gelatin Factory";
      case kJellyFac: return "Jelly Factory";
      case kJelloShotFac: return "Jello-Shots Factory";
      case kEmulsifierFac: return "Emulsifiers Factory";
      case kIceCreamFac: return "Ice Cream Factory"; 
      case kBatteryFarm: return "Battery Chicken Farm";
      case kProteinFac: return "Protein Powder Factory";
      case kChocolateFac: return "Chocolate Factory"; 
      case kPieFac: return "Meat Pie Factory";
      case kMSGFac: return "Monosodium Glutamate Factory";
      case kTVDinnerFac: return "TV-Dinner Factory";
      case kCakeFac: return "Chocolate Cake Factory"; 
      case kCaffeineFac: return "Caffeine Powder Factory";
      case kEnergyDrinkFac: return "Energy Drink Factory";
      case kRaveJuiceFac: return "Rave Juice Factory";
      case kPerkPillFac: return "Perk Pills Factory";
      case kPackagingFac: return "Packaging Manufacturer";
      case kDessertFac: return "Dessert Factory";
      case kCateringKitFac: return "Catering Pack Factory";
      case kPartyPackFac: return "Party Pack Factory";
      case kParfumeFac: return "Parfume Factory";
      case kMiraclePowderFac: return "Mirical Powder Factory";
      case kTurkishDelightFac: return "Turkish Delight Factory";
      case kSpareFac0: case kSpareFac1: case kSpareFac2: case kSpareFac3:
      case kSpareFac4: case kSpareFac5: case kSpareFac6: case kSpareFac7: 
      case kSpareFac8: case kSpareFac9: case kSpareFac10: case kSpareFac11:
      case kSpareFac12: case kSpareFac13: case kSpareFac14: case kSpareFac15: 
      case kNFactorySubTypes: return "F_PLACEHOLDER";
    }
    case kSpecial: switch (_subType.special) {
      case kShop: return "The Shop";
      case kSellBox: return "Sales";
      case kExportBox: return isCamouflaged() ? "A Suspicious Rock" : "Cargo Exporter";
      case kImportBox: return isCamouflaged() ? "A Suspicious Rock" : "Cargo Importer";
      case kWarp: return isCamouflaged() ? "A Suspicious Rock" : "Plot Manager";
      case kNSpecialSubTypes: return "S_PLACEHOLDER";
    }
    case kNBuildingTypes:;
  }
  return "B_CAT_PLACEHOLDER";
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
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    if (_building->m_sprite[zoom]) { 
      pd->sprite->freeSprite(_building->m_sprite[zoom]);
      _building->m_sprite[zoom] = NULL;
    }
  }
  _building->m_type = kNoBuilding;
  _building->m_updateFn = NULL;
  m_buildingSearchLocation = _building->m_index;
}

void growAtAll() {
  for (uint32_t b = 0; b < TOT_CARGO_OR_BUILDINGS; ++b) {
    if (m_buildings[b].m_type == kPlant) m_buildings[b].m_progress = rand() % (TICKS_PER_SEC * 2);
  }
}


void getBuildingNeighbors(struct Building_t* _building, int8_t _offset, struct Location_t** _above, struct Location_t** _below, struct Location_t** _left, struct Location_t** _right) {
  uint16_t locX = _building->m_location->m_x;
  uint16_t locY = _building->m_location->m_y;;
  (*_above) = getLocation(locX, locY - _offset);
  (*_below) = getLocation(locX, locY + _offset);
  (*_left)  = getLocation(locX - _offset, locY);
  (*_right) = getLocation(locX + _offset, locY);
}

bool isLargeBuilding(enum kBuildingType _type, union kSubType _subType) {
  return _type >= kExtractor || (_type >= kUtility && _subType.utility == kRetirement);
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

bool buildingHasUpdateFunction(enum kBuildingType _type, union kSubType _subType) {
  if (_type == kUtility && _subType.utility == kWell) return false;
  if (_type == kUtility && _subType.utility == kPath) return false;
  if (_type == kUtility && _subType.utility == kSign) return false;
  if (_type == kUtility && _subType.utility == kRetirement) return false;
  if (_type == kUtility && _subType.utility == kFence) return false;
  if (_type == kSpecial && _subType.special == kWarp) return false;
  if (_type == kSpecial && _subType.special == kShop) return false;
  return true;
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
    case kPlant:;  canBePlaced = canBePlacedPlant(_loc, _subType); break;
    case kUtility:; canBePlaced = canBePlacedUtility(_loc, _subType); break;
    case kExtractor:;  canBePlaced = canBePlacedExtractor(_loc, _subType); break;
    case kSpecial:;  canBePlaced = true; break;
    case kFactory:;  canBePlaced = canBePlacedFactory(_loc); break;
    case kNoBuilding:; case kNBuildingTypes:; break;
  }
  if (!canBePlaced) return false;

  struct Building_t* building = _loc->m_building;
  if (building) {
    pd->system->error("BUILDING ALREADY AT THIS LOCATION!");
    return false;
  }

  building = buildingManagerNewBuilding(_type);
  if (!building) {
    // Run out of buildings
    pd->system->error("Run out of buildings!");
    return false;
  }
  _loc->m_building = building;
  _loc->m_notOwned = false;
  if (isLargeBuilding(_type, _subType)) { // Add to neighbors too
    for (int32_t x = -1; x < 2; ++x) {
      for (int32_t y = -1; y < 2; ++y) {
        if (!x && !y) continue;
        struct Location_t* otherLoc = getLocation(_loc->m_x + x, _loc->m_y + y);
        otherLoc->m_building = building;
        otherLoc->m_notOwned = true;
      }
    }
  }

  setBuildingSubType(building, _subType);
  building->m_dir = _dir;
  building->m_progress = 0;
  building->m_mode.mode16 = 0;
  for (int32_t i = 0; i < MAX_STORE; ++i) {
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

  chunkAddBuildingRender(_loc->m_chunk, building); // Careful, no de-duplication in here, for speed
  if (buildingHasUpdateFunction(_type, _subType)) chunkAddBuildingUpdate(_loc->m_chunk, building);

  // Special - well
  bool wideRedraw = false;
  if (_type == kUtility && _subType.utility == kWell) {
    struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
    building->m_mode.mode16 = t->m_tile;
    setTile( getTile_idx(_loc->m_x, _loc->m_y), SPRITE16_ID(4,14));
    doWetness(/*for titles = */ false);
    wideRedraw = true;
  }

  if (_type == kUtility && (_subType.utility == kPath || _subType.utility == kFence)) {
    wideRedraw = true;
  }

  // The Special objects get added during gen - don't redraw for these
  if (_type != kSpecial) {

    // Bake into the background
    if (isLargeBuilding(_type, _subType) || wideRedraw) {
      renderChunkBackgroundImageAround(_loc->m_chunk);
    } else {
      renderChunkBackgroundImage(_loc->m_chunk);
    }

    // We add special at generate, we don't want to be updating the sprite list here
    updateRenderList();
  }

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
  // Max alocate?
//  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
//    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
//    m_buildings[i].m_sprite[zoom] = pd->sprite->newSprite();
//    }
//  }
  m_deserialiseIndexBuilding = 0;
  m_buildingSearchLocation = 0;
  m_nBuildings = 0;
  for (uint8_t i = 0; i < kNBuildingTypes; ++i) m_nByType[i] = 0;
}

void initBuilding() {
  m_buildings = pd->system->realloc(NULL, SIZE_BUILDING);
  memset(m_buildings, 0, SIZE_BUILDING);
  #ifdef DEV
  pd->system->logToConsole("malloc: for buildings %i", SIZE_BUILDING/1024);
  #endif
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
    if (m_buildings[i].m_subType.raw) {
      je->addTableMember(je, "stype", 5);
      je->writeInt(je, m_buildings[i].m_subType.raw);
    }
    je->addTableMember(je, "x", 1);
    je->writeInt(je, m_buildings[i].m_pix_x);
    je->addTableMember(je, "y", 1);
    je->writeInt(je, m_buildings[i].m_pix_y);
    if (m_buildings[i].m_progress) {
      je->addTableMember(je, "prog", 4);
      je->writeInt(je, m_buildings[i].m_progress);
    }
    if (m_buildings[i].m_mode.mode16) {
      je->addTableMember(je, "mode", 4);
      je->writeInt(je, m_buildings[i].m_mode.mode16);
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
    m_buildings[m_deserialiseIndexBuilding].m_subType.raw = json_intValue(_value);
  } else if (strcmp(_key, "x") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_pix_x = json_intValue(_value);
  } else if (strcmp(_key, "y") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_pix_y = json_intValue(_value);
  } else if (strcmp(_key, "prog") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_progress = json_intValue(_value);
  } else if (strcmp(_key, "mode") == 0) {
    m_buildings[m_deserialiseIndexBuilding].m_mode.mode16 = json_intValue(_value);
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
