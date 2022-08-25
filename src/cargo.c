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
  {.subType = kVitamin, .price = 8, .UIIcon = SID(12,8)},
  {.subType = kSunflower, .price = 2, .UIIcon = SID(15,7)},
  {.subType = kPotato, .price = 2, .UIIcon = SID(14,7)},
  {.subType = kOil, .price = 3, .UIIcon = SID(12,7)},
  {.subType = kSalt, .price = 1, .UIIcon = SID(13,8)},
  {.subType = kCrisps, .price = 12, .UIIcon = SID(0,21)},
  {.subType = kApple, .price = 4, .UIIcon = SID(8,7)},
  {.subType = kWaterBarrel, .price = 1, .UIIcon = SID(0,24)},
  {.subType = kEthanol, .price = 15, .UIIcon = SID(6,23)},
  {.subType = kHardCider, .price = 20, .UIIcon = SID(14,5)},
  {.subType = kCorn, .price = 6, .UIIcon = SID(2,22)},
  {.subType = kSeaweed, .price = 5, .UIIcon = SID(3,22)},
  {.subType = kMeat, .price = 25, .UIIcon = SID(9,8)},
  {.subType = kHydrogen, .price = 5, .UIIcon = SID(1,23)},
  {.subType = kHOil, .price = 9, .UIIcon = SID(12,6)},
  {.subType = kCornDog, .price = 50, .UIIcon = SID(4,24)},
  {.subType = kWheat, .price = 12, .UIIcon = SID(11,7)},
  {.subType = kBeer, .price = 40, .UIIcon = SID(7,23)},
  {.subType = kCactus, .price = 16, .UIIcon = SID(1,22)},
  {.subType = kTequila, .price = 60, .UIIcon = SID(5,24)},
  {.subType = kLime, .price = 22, .UIIcon = SID(4,22)},
  {.subType = kMexBeer, .price = 70, .UIIcon = SID(7,24)},
  {.subType = kEmulsifiers, .price = 13, .UIIcon = SID(6,24)},
  {.subType = kHFCS, .price = 27, .UIIcon = SID(1,24)},
  {.subType = kStrawberry, .price = 32, .UIIcon = SID(5,22)},
  {.subType = kIceCream, .price = 85, .UIIcon = SID(8,23)},
  {.subType = kEggs, .price = 10, .UIIcon = SID(2,24)},
  {.subType = kBamboo, .price = 20, .UIIcon = SID(4,23)},
  {.subType = kProtein, .price = 100, .UIIcon = SID(3,24)},
  {.subType = kGelatin, .price = 30, .UIIcon = SID(8,24)},
  {.subType = kMeatPie, .price = 125, .UIIcon = SID(8,6)},
  {.subType = kMSG, .price = 10, .UIIcon = SID(1,21)},
  {.subType = kReadyMeal, .price = 150, .UIIcon = SID(9,6)},
  {.subType = kJelly, .price = 140, .UIIcon = SID(10,6)},
  {.subType = kCoffeeBean, .price = 35, .UIIcon = SID(11,6)},
  {.subType = kCO2, .price = 25, .UIIcon = SID(2,23)},
  {.subType = kCaffeine, .price = 40, .UIIcon = SID(3,23)},
  {.subType = kEnergyDrink, .price = 200, .UIIcon = SID(13,6)},
  {.subType = kRaveJuice, .price = 250, .UIIcon = SID(14,6)},
  {.subType = kPerkPills, .price = 80, .UIIcon = SID(15,6)},
  {.subType = kSoftCake, .price = 270, .UIIcon = SID(9,5)},
  {.subType = kDessert, .price = 300, .UIIcon = SID(10,5)},
  {.subType = kPackaging, .price = 20, .UIIcon = SID(11,5)},
  {.subType = kCateringKit, .price = 320, .UIIcon = SID(12,5)},
  {.subType = kDrinksKit, .price = 350, .UIIcon = SID(13,5)},
  {.subType = kCocoBean, .price = 45, .UIIcon = SID(6,22)},
  {.subType = kSeaCucumber, .price = 70, .UIIcon = SID(8,22)},
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
    case kOil: return _plural ? "Oils" : "Oil";
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
    case kMexBeer: return _plural ? "Mexican Beers" : "Mexican Beer";
    case kEmulsifiers: return _plural ? "Emulsifiers" : "Emulsifier";
    case kHFCS: return "H.F.C.S.";
    case kStrawberry: return _plural ? "Strawberries" : "Strawberry";
    case kIceCream: return _plural ? "Ice Creams" : "Ice Cream";
    case kEggs: return _plural ? "Eggs" : "Egg";
    case kBamboo: return _plural ? "Bamboos" : "Bamboo";
    case kProtein: return _plural ? "Protein Powders" : "Protein Powder";
    case kGelatin: return "Gelatin";
    case kMeatPie: return _plural ? "Meat Pies" : "Meat Pie";
    case kMSG: return "M.S.G.";
    case kReadyMeal: return _plural ? "Ready Meals" : "Ready Meal";
    case kJelly: return _plural ? "Jellies" : "Jelly";
    case kCoffeeBean: return _plural ? "Coffee Beans" : "Coffee Bean";
    case kCO2: return "CO2";
    case kCaffeine: return "Caffeine";
    case kEnergyDrink: return _plural ? "Energy Drinks" : "Energy Drink";
    case kRaveJuice: return _plural ? "Rave Juices" : "Rave Juice";
    case kPerkPills: return "Perk Pills";
    case kSoftCake: return _plural ? "Soft Cakes" : "Soft Cake";
    case kDessert: return _plural ? "Desserts" : "Dessert";
    case kPackaging: return "Packaging";
    case kCateringKit: return _plural ? "Catering Kits" : "Catering Kit";
    case kDrinksKit: return _plural ? "Drinks Kits" : "Drinks Kit";
    case kCocoBean: return _plural ? "Coco Beans" : "Coco Bean";
    case kSeaCucumber: return _plural ? "Sea Cucumbers" : "Sea Cucumber";
    //case kCa0: case kCa1: case kCa2: case kCa3: case kCa4: case kCa5: case kCa6: case kCa7: 
    //case kCa8: case kCa9: case kCa10: case kCa11: case kCa12: case kCa13: case kCa14: case kCa15: 
    case kNCargoType: return "CARGO???";
  }
  return "CARGO????";
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
