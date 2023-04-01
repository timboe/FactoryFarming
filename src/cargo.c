#include "cargo.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "input.h"
#include "ui.h"
#include "buildings/conveyor.h"

#define DIS UINT32_MAX

struct CargoDescriptor_t CargoDesc[] = {
  {.subType = kNoCargo, .price = 0, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kCarrot, .price = 0, .multi = 1.0f, .UIIcon = SID(10,7)}, // Set by hand
  {.subType = kChalk, .price = 0, .multi = 1.0f, .UIIcon = SID(13,7)}, // Set by hand
  {.subType = kVitamin, .price = 0, .multi = 4.0f, .UIIcon = SID(12,8)}, // Set by hand
  {.subType = kSunflower, .price = 0, .multi = 2.0f, .UIIcon = SID(14,24)}, // CROP
  {.subType = kPotato, .price = 0, .multi = 2.0f, .UIIcon = SID(12,24)}, // CROP
  {.subType = kOil, .price = 0, .multi = 2.0f, .UIIcon = SID(12,7)},
  {.subType = kSalt, .price = 0, .multi = 2.0f, .UIIcon = SID(13,8)}, // Set by hand
  {.subType = kCrisps, .price = 0, .multi = 2.0f, .UIIcon = SID(0,21)},
  {.subType = kApple, .price = 0, .multi = 2.0f, .UIIcon = SID(8,7)}, // CROP
  {.subType = kWaterBarrel, .price = 0, .multi = 1.0f, .UIIcon = SID(0,24)}, // Set by hand
  {.subType = kEthanol, .price = 0, .multi = 2.0f, .UIIcon = SID(6,23)},
  {.subType = kHardCider, .price = 0, .multi = 2.0f, .UIIcon = SID(10,24)},
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
  {.subType = kJelloShot, .price = 0, .multi = 1.5f, .UIIcon = SID(9,24)},
  {.subType = kStrawberry, .price = 0, .multi = 1.5f, .UIIcon = SID(5,22)}, // CROP
  {.subType = kEmulsifiers, .price = 0, .multi = 1.5f, .UIIcon = SID(6,24)},
  {.subType = kIceCream, .price = 0, .multi = 1.5f, .UIIcon = SID(8,25)},
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
  {.subType = kEnergyDrink, .price = 0, .multi = 1.3f, .UIIcon = SID(13,6)},
  {.subType = kRaveJuice, .price = 0, .multi = 1.3f, .UIIcon = SID(11,24)},
  {.subType = kPerkPills, .price = 0, .multi = 1.5f, .UIIcon = SID(9,5)},
  {.subType = kBamboo, .price = 0, .multi = 0.1f, .UIIcon = SID(7,25)}, // CROP
  {.subType = kPackaging, .price = 0, .multi = 1.5f, .UIIcon = SID(11,5)},
  {.subType = kDessert, .price = 0, .multi = 1.5f, .UIIcon = SID(10,5)},
  {.subType = kCateringKit, .price = 0, .multi = 1.5f, .UIIcon = SID(12,5)},
  {.subType = kSpareCargo8, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo9, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo10, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo11, .price = DIS, .multi = 0.0f, .UIIcon = SID(0,0)},
  {.subType = kFooYoung, .price = 0, .multi = 7.5f, .UIIcon = SID(4,25)},
  {.subType = kPartyPack, .price = 0, .multi = 1.75f, .UIIcon = SID(13,5)},
  {.subType = kSeaCucumber, .price = 0, .multi = 50.0f, .UIIcon = SID(8,22)}, // CROP
  {.subType = kParfume, .price = 0, .multi = 4.0f, .UIIcon = SID(7,24)},
  {.subType = kMiraclePowder, .price = 0, .multi = 4.0f, .UIIcon = SID(5,25)},
  {.subType = kRose, .price = 0, .multi = 2.5f, .UIIcon = SID(0,25)}, // CROP
  {.subType = kTurkishDelight, .price = 0, .multi = 5.0f, .UIIcon = SID(6,25)},
  {.subType = kSpareCargo12, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo13, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo14, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
  {.subType = kSpareCargo15, .price = DIS, .multi = 1.0f, .UIIcon = SID(0,0)},
};

const int32_t SIZE_CARGO = CARGO_LIMIT * sizeof(struct Cargo_t);

uint16_t m_nCargo = 0;

uint16_t m_cargoSearchLocation = 0;

uint16_t m_deserialiseIndexCargo = 0;
uint16_t m_deserialiseXCargo = 0, m_deserialiseYCargo = 0;

struct Cargo_t* m_cargos;

void cargoSpriteSetup(struct Cargo_t* _cargo, int16_t _x, int16_t _y, uint16_t _idx);

/// ///

const char* toStringCargoByType(enum kCargoType _type, bool _plural) {
  switch(_type) {
    case kNoCargo: return "NOCARGO!";
    case kCarrot: return  _plural ? tr(kTRCarrotPlural) : tr(kTRCarrot);
    case kChalk: return _plural ? tr(kTRChalkPlural) : tr(kTRChalk);
    case kVitamin: return _plural ? tr(kTRVitaminPlural) : tr(kTRVitamin);
    case kPotato: return _plural ? tr(kTRPotatoPlural) : tr(kTRPotato);
    case kSunflower: return _plural ? tr(kTRSunflowerPlural) : tr(kTRSunflower);
    case kOil: return _plural ? tr(kTROilPlural) : tr(kTROil);
    case kSalt: return _plural ? tr(kTRSaltPlural) : tr(kTRSalt);
    case kCrisps: return _plural ? tr(kTRCrispsPlural) : tr(kTRCrisps);
    case kApple: return _plural ? tr(kTRApplePlural) : tr(kTRApple);
    case kWaterBarrel: return _plural ? tr(kTRWaterBarrelPlural) : tr(kTRWaterBarrel);
    case kEthanol: return _plural ? tr(kTREthanolPlural) : tr(kTREthanol);
    case kHardCider: return _plural ? tr(kTRHardCiderPlural) : tr(kTRHardCider);
    case kCorn: return _plural ? tr(kTRCornPlural) : tr(kTRCorn);
    case kSeaweed: return _plural ? tr(kTRSeaweedPlural) : tr(kTRSeaweed);
    case kMeat: return _plural ? tr(kTRMeatPlural) : tr(kTRMeat);
    case kHydrogen: return _plural ? tr(kTRHydrogenPlural) : tr(kTRHydrogen);
    case kHOil: return _plural ? tr(kTRHOilPlural) : tr(kTRHOil);
    case kCornDog: return _plural ? tr(kTRCornDogPlural) : tr(kTRCornDog);
    case kWheat: return _plural ? tr(kTRWheatPlural) : tr(kTRWheat);
    case kBeer: return _plural ? tr(kTRBeerPlural) : tr(kTRBeer);
    case kCactus: return _plural ? tr(kTRCactusPlural) : tr(kTRCactus);
    case kTequila: return _plural ? tr(kTRTequilaPlural) : tr(kTRTequila);
    case kLime: return _plural ? tr(kTRLimePlural) : tr(kTRLime);
    case kHFCS: return _plural ? tr(kTRHFCSPlural) : tr(kTRHFCS);
    case kGelatin: return _plural ? tr(kTRGelatinPlural) : tr(kTRGelatin);
    case kJelly: return _plural ? tr(kTRJellyPlural) : tr(kTRJelly);
    case kJelloShot: return _plural ? tr(kTRJelloShotPlural) : tr(kTRJelloShot);
    case kStrawberry: return _plural ? tr(kTRStrawberryPlural) : tr(kTRStrawberry);
    case kEmulsifiers: return _plural ? tr(kTREmulsifiersPlural) : tr(kTREmulsifiers);
    case kIceCream: return _plural ? tr(kTRIceCreamPlural) : tr(kTRIceCream);
    case kEggs: return _plural ? tr(kTREggsPlural) : tr(kTREggs);
    case kProtein: return _plural ? tr(kTRProteinPlural) : tr(kTRProtein);
    case kCocoBean: return _plural ? tr(kTRCocoBeanPlural) : tr(kTRCocoBean);
    case kChocolate: return _plural ? tr(kTRChocolatePlural) : tr(kTRChocolate);
    case kMeatPie: return _plural ? tr(kTRMeatPiePlural) : tr(kTRMeatPie);
    case kMSG: return _plural ? tr(kTRMSGPlural) : tr(kTRMSG);
    case kTVDinner: return _plural ? tr(kTRTVDinnerPlural) : tr(kTRTVDinner);
    case kCake: return _plural ? tr(kTRCakTRePlural) : tr(kTRCake);
    case kCoffeeBean: return _plural ? tr(kTRCoffeeBeanPlural) : tr(kTRCoffeeBean);
    case kCO2: return _plural ? tr(kTRCO2Plural) : tr(kTRCO2);
    case kCaffeine: return _plural ? tr(kTRCaffeinePlural) : tr(kTRCaffeine);
    case kEnergyDrink: return _plural ? tr(kTREnergyDrinkPlural) : tr(kTREnergyDrink);
    case kRaveJuice: return _plural ? tr(kTRRaveJuicePlural) : tr(kTRRaveJuice);
    case kPerkPills: return _plural ? tr(kTRPerkPillsPlural) : tr(kTRPerkPills);
    case kBamboo: return _plural ? tr(kTRBambooPlural) : tr(kTRBamboo);
    case kPackaging: return _plural ? tr(kTRPackagingPlural) : tr(kTRPackaging);
    case kDessert: return _plural ? tr(kTRDessertPlural) : tr(kTRDessert);
    case kCateringKit: return _plural ? tr(kTRCateringKitPlural) : tr(kTRCateringKit);
    case kFooYoung: return _plural ? tr(kTRFooYoungPlural) : tr(kTRFooYoung);
    case kPartyPack: return _plural ? tr(kTRPartyPackPlural) : tr(kTRPartyPack);
    case kSeaCucumber: return _plural ? tr(kTRSeaCucumberPlural) : tr(kTRSeaCucumber);
    case kParfume: return _plural ? tr(kTRParfumePlural) : tr(kTRParfume);
    case kMiraclePowder: return _plural ? tr(kTRMiraclePowderPlural) : tr(kTRMiraclePowder);
    case kRose: return _plural ? tr(kTRRosePlural) : tr(kTRRose);
    case kTurkishDelight: return _plural ? tr(kTRTurkishDelightPlural) : tr(kTRTurkishDelight);
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
    const uint32_t stop  = (try == 0 ? CARGO_LIMIT : m_cargoSearchLocation);
    for (uint32_t i = start; i < stop; ++i) {
      if (m_cargos[i].m_type == kNoCargo) {
        ++m_nCargo;
        m_cargoSearchLocation = i+1;
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
  ///////
  #ifdef AGGRESSIVE_FREE_CARGO_SPRITES
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    if (_cargo->m_sprite[zoom]) {
      pd->sprite->freeSprite(_cargo->m_sprite[zoom]);
      _cargo->m_sprite[zoom] = NULL;
    }
  }
  #endif
  /////
  m_cargoSearchLocation = _cargo->m_index;
  --m_nCargo;
}

void cargoSpriteSetup(struct Cargo_t* _cargo, int16_t _x, int16_t _y, uint16_t _idx) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    
    /////
    #ifndef AGGRESSIVE_ALLOCATE_CARGO_SPRITES
    PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
    if (_cargo->m_sprite[zoom] == NULL) {
      _cargo->m_sprite[zoom] = pd->sprite->newSprite();
    }
    pd->sprite->setBounds(_cargo->m_sprite[zoom], bound);
    pd->sprite->setZIndex(_cargo->m_sprite[zoom], Z_INDEX_CARGO_FLOOR);
    #endif
    /////

    pd->sprite->setImage(_cargo->m_sprite[zoom], getSprite16_byidx(_idx, zoom), kBitmapUnflipped);
    pd->sprite->moveTo(_cargo->m_sprite[zoom], _x * zoom, _y * zoom);
  }
}

bool newCargo(struct Location_t* _loc, enum kCargoType _type, bool _addToDisplay) {
  bool addedByPlayer = true;
  if (_loc->m_cargo != NULL) return false;

  struct Cargo_t* cargo = cargoManagerNewCargo(_type);
  if (!cargo) { // Run out of slots
    pd->system->error("NO MORE CARGO SLOTS");
    return false;
  }

  cargoSpriteSetup(cargo, 
    TILE_PIX*_loc->m_x + _loc->m_pix_off_x + TILE_PIX/2.0,
    TILE_PIX*_loc->m_y + _loc->m_pix_off_y + TILE_PIX/2.0,
    CargoDesc[_type].UIIcon);

  _loc->m_cargo = cargo;
  if (_loc->m_building && _loc->m_building->m_type == kConveyor) {
    _loc->m_building->m_progress = 0;
    // No longer doing conveyor update on IN, doing it on OUT instead 
    //updateConveyorDirection(_loc->m_building);
    pd->sprite->setZIndex(cargo->m_sprite[1], Z_INDEX_CARGO_BELT);
    pd->sprite->setZIndex(cargo->m_sprite[2], Z_INDEX_CARGO_BELT);
    // Note: We need to setZIndex in deserialisation too - done in Location
  }

  struct Chunk_t* chunk = _loc->m_chunk;
  chunkAddCargo(chunk, cargo);

  if (_addToDisplay && getGameMode() != kMenuCredits) pd->sprite->addSprite(cargo->m_sprite[getZoom()]);
  return true;
}

void resetCargo() {
  for (uint32_t i = 0; i < CARGO_LIMIT; ++i) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      if (m_cargos[i].m_sprite[zoom]) pd->sprite->freeSprite(m_cargos[i].m_sprite[zoom]);
    }
  }
  memset(m_cargos, 0, SIZE_CARGO);

  for (uint32_t i = 0; i < CARGO_LIMIT; ++i) {
    m_cargos[i].m_index = i;
    /////
    #ifdef AGGRESSIVE_ALLOCATE_CARGO_SPRITES
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
      m_cargos[i].m_sprite[zoom] = pd->sprite->newSprite();
      pd->sprite->setBounds(m_cargos[i].m_sprite[zoom], bound);
      pd->sprite->setZIndex(m_cargos[i].m_sprite[zoom], Z_INDEX_CARGO_FLOOR);
    }
    #endif
    /////
  }
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

  for (uint32_t i = 0; i < CARGO_LIMIT; ++i) {
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
