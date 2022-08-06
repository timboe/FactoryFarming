#include "cargo.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "input.h"

//{kNoCargo, kApple, kCheese, kCarrot, kWheat, kWaterBarrel, kChalk, kVitamin, 
// kSunflowe, kOil, kPotato, kSalt, kNCargoType};
const struct CargoDescriptor_t CargoDesc[] = {
  {.subType = kNoCargo, .price = 0, .UIIcon = SID(0,0)},
  {.subType = kCarrot, .price = 1, .UIIcon = SID(10,7)},
  {.subType = kChalk, .price = 1, .UIIcon = SID(13,7)},
  {.subType = kVitamin, .price = 16, .UIIcon = SID(12,8)},
  {.subType = kPotato, .price = 4, .UIIcon = SID(14,7)},
  {.subType = kSunflower, .price = 4, .UIIcon = SID(15,7)},
  {.subType = kOil, .price = 6, .UIIcon = SID(12,7)},
  {.subType = kSalt, .price = 1, .UIIcon = SID(13,8)},
  {.subType = kCrisps, .price = 10, .UIIcon = SID(0,21)},
  {.subType = kApple, .price = 2, .UIIcon = SID(8,7)},
  {.subType = kWaterBarrel, .price = 1, .UIIcon = SID(12,7)},
  {.subType = kEthanol, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCorn, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kSeaweed, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kMeat, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kHydrogen, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kHOil, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCornDog, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kWheat, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kBeer, .price = 1, .UIIcon = SID(11,7)},
  {.subType = kCactus, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kTequila, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kLime, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kMexBeer, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kEmulsifiers, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kHFCS, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kStrawberry, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kIceCream, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kEggs, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kHemp, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kProtein, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kGelatin, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kMeatPie, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kMSG, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kReadyMeal, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kJelly, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCoffeeBean, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCO2, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCaffeine, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kEnergyDrink, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kRaveJuice, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kPerkPills, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kSoftCake, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kDessert, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kPackaging, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCateringKit, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kDrinksKit, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kCocoBean, .price = 1, .UIIcon = SID(2,4)},
  {.subType = kSeaCucumber, .price = 1, .UIIcon = SID(2,4)},
};

const int32_t SIZE_CARGO = TOT_CARGO_OR_BUILDINGS * sizeof(struct Cargo_t);

uint16_t m_nCargo = 0;

uint16_t m_cargoSearchLocation = 0;

uint16_t m_deserialiseIndexCargo = 0;
uint16_t m_deserialiseXCargo = 0, m_deserialiseYCargo = 0;

struct Cargo_t* m_cargos;

void cargoSpriteSetup(struct Cargo_t* _cargo, uint16_t _x, uint16_t _y, uint16_t _idx);

/// ///

const char* toStringCargoByType(enum kCargoType _type) {
  switch(_type) {
    case kNoCargo: return "NOCARGO!";
    case kCarrot: return "Carrot";
    case kChalk: return "Chalk";
    case kVitamin: return "Vitamins";
    case kPotato: return "Potato";
    case kSunflower: return "Sunflower";
    case kOil: return "Oil";
    case kSalt: return "Salt";
    case kCrisps: return "Chips";
    case kApple: return "Apple";
    case kWaterBarrel: return "Water Bucket";
    case kEthanol: return "Ethanol";
    case kCorn: return "Corn";
    case kSeaweed: return "Seaweed";
    case kMeat: return "Meat";
    case kHydrogen: return "Hydrogen";
    case kHOil: return "Hydrogenated Oil";
    case kCornDog: return "Corn Dog";
    case kWheat: return "Wheat";
    case kBeer: return "Beer";
    case kCactus: return "Cactus";
    case kTequila: return "Tequila";
    case kLime: return "Lime";
    case kMexBeer: return "Mexican Beer";
    case kEmulsifiers: return "Emulsifiers";
    case kHFCS: return "H.F.C.S.";
    case kStrawberry: return "Strawberry";
    case kIceCream: return "Ice Cream";
    case kEggs: return "Eggs";
    case kHemp: return "Hemp Fibers";
    case kProtein: return "Protein Powder";
    case kGelatin: return "Gelatin";
    case kMeatPie: return "Meat Pie";
    case kMSG: return "M.S.G.";
    case kReadyMeal: return "Ready Meal";
    case kJelly: return "Jelly";
    case kCoffeeBean: return "Coffee Bean";
    case kCO2: return "CO2";
    case kCaffeine: return "Caffeine";
    case kEnergyDrink: return "Energy Drink";
    case kRaveJuice: return "Rave Juice";
    case kPerkPills: return "Perk Pills";
    case kSoftCake: return "Soft Cake";
    case kDessert: return "Dessert";
    case kPackaging: return "Packaging";
    case kCateringKit: return "Catering Kit";
    case kDrinksKit: return "Drinks Kit";
    case kCocoBean: return "Coco Bean";
    case kSeaCucumber: return "Sea Cucumber";
    //case kCa0: case kCa1: case kCa2: case kCa3: case kCa4: case kCa5: case kCa6: case kCa7: 
    //case kCa8: case kCa9: case kCa10: case kCa11: case kCa12: case kCa13: case kCa14: case kCa15: 
    case kNCargoType: return "CARGO???";
  }
  return "CARGO????";
}

const char* toStringCargo(struct Cargo_t* _cargo) {
  if (!_cargo) return "";
  return toStringCargoByType(_cargo->m_type);
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

  //pd->system->logToConsole("Set cargo, current %i", (int)loc->m_cargo);

  struct Cargo_t* cargo = cargoManagerNewCargo(_type);
  if (!cargo) { // Run out of slots
    return false;
  }

  cargoSpriteSetup(cargo, 
    TILE_PIX*_loc->m_x + _loc->m_pix_off_x + TILE_PIX/2.0,
    TILE_PIX*_loc->m_y + _loc->m_pix_off_y + TILE_PIX/2.0,
    CargoDesc[_type].UIIcon);

  _loc->m_cargo = cargo;
  if (_loc->m_building) {
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
  pd->system->logToConsole("malloc: for cargo %i", SIZE_CARGO/1024);
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
