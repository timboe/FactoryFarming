#include "cargo.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "input.h"

#define DIS UINT32_MAX

struct CargoDescriptor_t CargoDesc[] = {
  {.subType = kNoCargo, .price = 0, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kCarrot, .price = 0, .multi = 1.0f, .UIIcon = SID(10,7)}, // Set by hand
  {.subType = kChalk, .price = 0, .multi = 1.0f, .UIIcon = SID(13,7)}, // Set by hand
  {.subType = kVitamin, .price = 0, .multi = 4.0f, .UIIcon = SID(12,8)}, // Set by hand
  {.subType = kSunflower, .price = 0, .multi = 2.0f, .UIIcon = SID(15,7)}, // CROP
  {.subType = kPotato, .price = 0, .multi = 2.0f, .UIIcon = SID(14,7)}, // CROP
  {.subType = kOil, .price = 0, .multi = 2.0f, .UIIcon = SID(12,7)},
  {.subType = kSalt, .price = 0, .multi = 2.0f, .UIIcon = SID(13,8)}, // Set by hand
  {.subType = kCrisps, .price = 0, .multi = 2.0f, .UIIcon = SID(0,21)},
  {.subType = kApple, .price = 0, .multi = 2.0f, .UIIcon = SID(8,7)}, // CROP
  {.subType = kWaterBarrel, .price = 0, .multi = 1.0f, .UIIcon = SID(0,24)}, // Set by hand
  {.subType = kEthanol, .price = 0, .multi = 2.0f, .UIIcon = SID(6,23)},
  {.subType = kHardCider, .price = 0, .multi = 2.0f, .UIIcon = SID(14,5)},
  {.subType = kCorn, .price = 0, .multi = 1.5f, .UIIcon = SID(2,22)}, // CROP
  {.subType = kSeaweed, .price = 0, .multi = 0.5f, .UIIcon = SID(3,22)}, // CROP
  {.subType = kMeat, .price = 0, .multi = 2.0f, .UIIcon = SID(9,8)},
  {.subType = kSpareCargo0, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo1, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo2, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo3, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kHydrogen, .price = 0, .multi = 2.0f, .UIIcon = SID(1,23)},
  {.subType = kHOil, .price = 0, .multi = 2.0f, .UIIcon = SID(12,6)},
  {.subType = kCornDog, .price = 0, .multi = 1.5f, .UIIcon = SID(4,24)},
  {.subType = kWheat, .price = 0, .multi = 1.5f, .UIIcon = SID(11,7)}, // CROP
  {.subType = kBeer, .price = 0, .multi = 2.0f, .UIIcon = SID(7,23)},
  {.subType = kCactus, .price = 0, .multi = 1.5f, .UIIcon = SID(1,22)}, // CROP
  {.subType = kTequila, .price = 0, .multi = 2.0f, .UIIcon = SID(5,24)},
  {.subType = kLime, .price = 0, .multi = 1.5f, .UIIcon = SID(4,22)}, // CROP
  {.subType = kHFCS, .price = 0, .multi = 1.5f, .UIIcon = SID(1,24)},
  {.subType = kGelatin, .price = 0, .multi = 1.5f, .UIIcon = SID(8,24)},
  {.subType = kJelly, .price = 0, .multi = 1.5f, .UIIcon = SID(10,6)},
  {.subType = kJelloShot, .price = 0, .multi = 1.5f, .UIIcon = SID(15,5)},
  {.subType = kStrawberry, .price = 0, .multi = 1.5f, .UIIcon = SID(5,22)}, // CROP
  {.subType = kEmulsifiers, .price = 0, .multi = 1.5f, .UIIcon = SID(6,24)},
  {.subType = kIceCream, .price = 0, .multi = 1.5f, .UIIcon = SID(8,23)},
  {.subType = kEggs, .price = 0, .multi = 1.5f, .UIIcon = SID(2,24)},
  {.subType = kSpareCargo4, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo5, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo6, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo7, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kProtein, .price = 0, .multi = 1.5f, .UIIcon = SID(3,24)},
  {.subType = kCocoBean, .price = 0, .multi = 2.0f, .UIIcon = SID(6,22)}, // CROP
  {.subType = kChocolate, .price = 0, .multi = 1.5f, .UIIcon = SID(3,25)},
  {.subType = kMeatPie, .price = 0, .multi = 1.5f, .UIIcon = SID(8,6)},
  {.subType = kMSG, .price = 0, .multi = 1.5f, .UIIcon = SID(1,21)},
  {.subType = kTVDinner, .price = 0, .multi = 1.5f, .UIIcon = SID(9,6)},
  {.subType = kCake, .price = 0, .multi = 1.5f, .UIIcon = SID(2,25)},
  {.subType = kCoffeeBean, .price = 0, .multi = 2.5f, .UIIcon = SID(11,6)}, // CROP
  {.subType = kCO2, .price = 0, .multi = 1.0f, .UIIcon = SID(2,23)}, // Set by hand
  {.subType = kCaffeine, .price = 0, .multi = 1.5f, .UIIcon = SID(3,23)},
  {.subType = kEnergyDrink, .price = 0, .multi = 1.5f, .UIIcon = SID(13,6)},
  {.subType = kRaveJuice, .price = 0, .multi = 1.5f, .UIIcon = SID(14,6)},
  {.subType = kPerkPills, .price = 0, .multi = 1.5f, .UIIcon = SID(15,6)},
  {.subType = kBamboo, .price = 0, .multi = 0.1f, .UIIcon = SID(7,25)}, // CROP
  {.subType = kPackaging, .price = 0, .multi = 1.5f, .UIIcon = SID(11,5)},
  {.subType = kDessert, .price = 0, .multi = 1.5f, .UIIcon = SID(10,5)},
  {.subType = kCateringKit, .price = 0, .multi = 1.5f, .UIIcon = SID(12,5)},
  {.subType = kSpareCargo8, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo9, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo10, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo11, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kFooYoung, .price = 0, .multi = 1.5f, .UIIcon = SID(4,25)},
  {.subType = kPartyPack, .price = 0, .multi = 1.5f, .UIIcon = SID(13,5)},
  {.subType = kSeaCucumber, .price = 0, .multi = 50.0f, .UIIcon = SID(8,22)}, // CROP
  {.subType = kParfume, .price = 0, .multi = 1.5f, .UIIcon = SID(7,24)},
  {.subType = kMiraclePowder, .price = 0, .multi = 1.5f, .UIIcon = SID(5,25)},
  {.subType = kRose, .price = 0, .multi = 2.5f, .UIIcon = SID(0,25)}, // CROP
  {.subType = kTurkishDelight, .price = 0, .multi = 1.5f, .UIIcon = SID(6,25)},
  {.subType = kSpareCargo12, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo13, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo14, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo15, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
};

const int32_t SIZE_CARGO = TOT_CARGO_OR_BUILDINGS * sizeof(struct Cargo_t);

uint16_t m_nCargo = 0;

uint16_t m_cargoSearchLocation = 0;

uint16_t m_deserialiseIndexCargo = 0;
uint16_t m_deserialiseXCargo = 0, m_deserialiseYCargo = 0;

struct Cargo_t* m_cargos;

void cargoSpriteSetup(struct Cargo_t* _cargo, uint16_t _x, uint16_t _y, uint16_t _idx);

/// ///

const char* toStringCargoByType(enum kCargoType _type, bool _plural) {
  switch(_type) {
    case kNoCargo: return "NOCARGO!";
    case kCarrot: return  _plural ? "Carrots" : "Carrot";
    case kChalk: return _plural ? "Chalks" : "Chalk";
    case kVitamin: return "Vitamins";
    case kPotato: return _plural ? "Potatoes" : "Potato";
    case kSunflower: return _plural ? "Sunflowers" : "Sunflower";
    case kOil: return _plural ? "Vegetable Oils" : "Vegetable Oil";
    case kSalt: return "Salt";
    case kCrisps: return "Chips";
    case kApple: return _plural ? "Apples" : "Apple";
    case kWaterBarrel: return "Water";
    case kEthanol: return "Ethanol";
    case kHardCider: return _plural ? "Hard Ciders" : "Hard Cider";
    case kCorn: return _plural ? "Corns" : "Corn";
    case kSeaweed: return _plural ? "Seaweeds" : "Seaweed";
    case kMeat: return "Meat";
    case kHydrogen: return "Hydrogen";
    case kHOil: return _plural ? "H. Oils" : "H. Oil";
    case kCornDog: return _plural ? "Corn Dogs" : "Corn Dog";
    case kWheat: return "Wheat";
    case kBeer: return _plural ? "Beers" : "Beer";
    case kCactus: return _plural ? "Cacti" : "Cactus";
    case kTequila: return _plural ? "Tequilas" : "Tequila";
    case kLime: return _plural ? "Limes" : "Lime";
    case kHFCS: return "H.F.C.S.";
    case kGelatin: return "Gelatin";
    case kJelly: return _plural ? "Jellies" : "Jelly";
    case kJelloShot: return _plural ? "Jello-Shots" : "Jello-Shot";
    case kStrawberry: return _plural ? "Strawberries" : "Strawberry";
    case kEmulsifiers: return _plural ? "Emulsifiers" : "Emulsifier";
    case kIceCream: return _plural ? "Ice Creams" : "Ice Cream";
    case kEggs: return _plural ? "Eggs" : "Egg";
    case kProtein: return _plural ? "Protein Powders" : "Protein Powder";
    case kCocoBean: return _plural ? "Coco Beans" : "Coco Bean";
    case kChocolate: return "Chocolate";
    case kMeatPie: return _plural ? "Meat Pies" : "Meat Pie";
    case kMSG: return "M.S.G.";
    case kTVDinner: return _plural ? "TV-Dinners" : "TV-Dinner";
    case kCake: return _plural ? "Cakes" : "Cake";
    case kCoffeeBean: return _plural ? "Coffee Beans" : "Coffee Bean";
    case kCO2: return "CO2";
    case kCaffeine: return "Caffeine";
    case kEnergyDrink: return _plural ? "Energy Drinks" : "Energy Drink";
    case kRaveJuice: return _plural ? "Rave Juices" : "Rave Juice";
    case kPerkPills: return "Perk Pills";
    case kBamboo: return _plural ? "Bamboos" : "Bamboo";
    case kPackaging: return "Packaging";
    case kDessert: return _plural ? "Desserts" : "Dessert";
    case kCateringKit: return _plural ? "Catering Kits" : "Catering Kit";
    case kFooYoung: return _plural ? "Chinese Ready Meals" : "Chinese Ready Meal";
    case kPartyPack: return _plural ? "Party Packs" : "Party Pack";
    case kSeaCucumber: return _plural ? "Sea Cucumbers" : "Sea Cucumber";
    case kParfume: return "Parfume";
    case kMiraclePowder: return "Miracle Powder";
    case kRose: return _plural ? "Roses" : "Rose";
    case kTurkishDelight: return "Turkish Delight";
    case kSpareCargo0: case kSpareCargo1: case kSpareCargo2: case kSpareCargo3:
    case kSpareCargo4: case kSpareCargo5: case kSpareCargo6: case kSpareCargo7: 
    case kSpareCargo8: case kSpareCargo9: case kSpareCargo10: case kSpareCargo11:
    case kSpareCargo12: case kSpareCargo13: case kSpareCargo14: case kSpareCargo15: 
    case kNCargoType:;
  }
  return "C_PLACEHOLDER";
}

const char* toStringCargo(struct Cargo_t* _cargo, bool _plural) {
  if (!_cargo) return "";
  return toStringCargoByType(_cargo->m_type, _plural);
}

uint16_t getNCargo() {
  return m_nCargo;
}

bool canBePlacedCargo(struct Location_t* loc) {
  return (loc->m_cargo == NULL);
}

struct Cargo_t* cargoManagerNewCargo(enum kCargoType _type) {
  for (uint8_t try = 0; try < 2; ++try) {
    const uint32_t start = (try == 0 ? m_cargoSearchLocation : 0);
    const uint32_t stop  = (try == 0 ? TOT_CARGO_OR_BUILDINGS : m_cargoSearchLocation);
    for (uint32_t i = start; i < stop; ++i) {
      if (m_cargos[i].m_type == kNoCargo) {
        ++m_nCargo;
        ++m_cargoSearchLocation;
        m_cargos[i].m_type = _type;
        return &(m_cargos[i]);
      }
    }
  }
  #ifdef DEV
  pd->system->error("Cannot allocate any more cargo!");
  #endif
  return NULL;
}

struct Cargo_t* cargoManagerGetByIndex(uint16_t _index) {
  return &(m_cargos[_index]);
}

void cargoManagerFreeCargo(struct Cargo_t* _cargo) {
  _cargo->m_type = kNoCargo;
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    if (_cargo->m_sprite[zoom]) {
      pd->sprite->freeSprite(_cargo->m_sprite[zoom]);
      _cargo->m_sprite[zoom] = NULL;
    }
  }
  m_cargoSearchLocation = _cargo->m_index;
  --m_nCargo;
}

void cargoSpriteSetup(struct Cargo_t* _cargo, uint16_t _x, uint16_t _y, uint16_t _idx) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
    if (_cargo->m_sprite[zoom] == NULL) {
      _cargo->m_sprite[zoom] = pd->sprite->newSprite();
    }
    pd->sprite->setBounds(_cargo->m_sprite[zoom], bound);
    pd->sprite->setImage(_cargo->m_sprite[zoom], getSprite16_byidx(_idx, zoom), kBitmapUnflipped);
    pd->sprite->moveTo(_cargo->m_sprite[zoom], _x * zoom, _y * zoom);
    pd->sprite->setZIndex(_cargo->m_sprite[zoom], Z_INDEX_CARGO);
  }
}

bool newCargo(struct Location_t* _loc, enum kCargoType _type, bool _addToDisplay) {
  bool addedByPlayer = true;
  if (_loc->m_cargo != NULL) return false;

  struct Cargo_t* cargo = cargoManagerNewCargo(_type);
  if (!cargo) { // Run out of slots
    return false;
  }

  cargoSpriteSetup(cargo, 
    TILE_PIX*_loc->m_x + _loc->m_pix_off_x + TILE_PIX/2.0,
    TILE_PIX*_loc->m_y + _loc->m_pix_off_y + TILE_PIX/2.0,
    CargoDesc[_type].UIIcon);

  _loc->m_cargo = cargo;
  if (_loc->m_building && _loc->m_building->m_type == kConveyor) {
    _loc->m_building->m_progress = 0;
  }

  struct Chunk_t* chunk = _loc->m_chunk;
  chunkAddCargo(chunk, cargo);

  if (_addToDisplay) pd->sprite->addSprite(cargo->m_sprite[getZoom()]);
  return true;
}

void resetCargo() {
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      if (m_cargos[i].m_sprite[zoom]) pd->sprite->freeSprite(m_cargos[i].m_sprite[zoom]);
    }
  }
  memset(m_cargos, 0, SIZE_CARGO);
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    m_cargos[i].m_index = i;
  }
  // Max alocate?
//  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
//    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
//      m_cargos[i].m_sprite[zoom] = pd->sprite->newSprite();
//    }
//  }
  m_deserialiseIndexCargo = 0;
  m_cargoSearchLocation = 0;
  m_nCargo = 0;
}

void initCargo() {
  m_cargos = pd->system->realloc(NULL, SIZE_CARGO);
  memset(m_cargos, 0, SIZE_CARGO);
  #ifdef DEV
  pd->system->logToConsole("malloc: for cargo %i", SIZE_CARGO/1024);
  #endif
}

void serialiseCargo(struct json_encoder* je) {
  je->addTableMember(je, "cargo", 5);
  je->startArray(je);

  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    if (m_cargos[i].m_type == kNoCargo) {
      continue;
    }
    float x, y;
    pd->sprite->getPosition(m_cargos[i].m_sprite[/*zoom=*/1], &x, &y);

    je->addArrayMember(je);
    je->startTable(je);
    je->addTableMember(je, "idx", 3);
    je->writeInt(je, i);
    je->addTableMember(je, "type", 4);
    je->writeInt(je, m_cargos[i].m_type);
    je->addTableMember(je, "x", 1);
    je->writeInt(je, (int32_t)x);
    je->addTableMember(je, "y", 1);
    je->writeInt(je, (int32_t)y);
    je->endTable(je);
  }

  je->endArray(je);
}

void deserialiseValueCargo(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "idx") == 0) {
    m_deserialiseIndexCargo = json_intValue(_value);
  } else if (strcmp(_key, "type") == 0) {
    m_cargos[m_deserialiseIndexCargo].m_type = json_intValue(_value);
  } else if (strcmp(_key, "x") == 0) {
    m_deserialiseXCargo = json_intValue(_value);
  } else if (strcmp(_key, "y") == 0) {
    m_deserialiseYCargo = json_intValue(_value);
  } else {
    pd->system->error("CARGO DECODE ISSUE, %s", _key);
  }
}

void* deserialiseStructDoneCargo(json_decoder* jd, const char* _name, json_value_type _type) {
  struct Cargo_t* cargo = &(m_cargos[m_deserialiseIndexCargo]);

  cargoSpriteSetup(cargo, 
    m_deserialiseXCargo,
    m_deserialiseYCargo,
    CargoDesc[cargo->m_type].UIIcon);
  ++m_nCargo;

  //pd->system->logToConsole("-- Cargo #%i, [%i] decoded to  %s, (%i, %i)", m_nCargo, m_deserialiseIndexCargo, toStringCargo(cargo), m_deserialiseXCargo, m_deserialiseYCargo);
  return NULL;
}
