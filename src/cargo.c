#include "cargo.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "input.h"

//{kNoCargo, kApple, kCheese, kCarrot, kWheat, kWaterBarrel, kChalk, kVitamin, kNCargoType};
const struct CargoDescriptor_t CargoDesc[] = {
  {.subType = kNoCargo, .price = 0, .UIIcon = SID(0,0)},
  {.subType = kApple, .price = 2, .UIIcon = SID(8,7)},
  {.subType = kCheese, .price = 8, .UIIcon = SID(9,7)},
  {.subType = kCarrot, .price = 1, .UIIcon = SID(10,7)},
  {.subType = kWheat, .price = 4, .UIIcon = SID(11,7)},
  {.subType = kWaterBarrel, .price = 1, .UIIcon = SID(12,7)},
  {.subType = kChalk, .price = 1, .UIIcon = SID(13,7)},
  {.subType = kVitamin, .price = 16, .UIIcon = SID(12,8)}
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
    case kNoCargo: return "NoCargo";
    case kApple: return "Apple";
    case kCheese: return "Cheese";
    case kWheat: return "Wheat";
    case kCarrot: return "Carrot";
    case kWaterBarrel: return "Water Barrel";
    case kChalk: return "Chalk";
    case kVitamin: return "Vitamins";
    default: return "Cargo???";
  }
}

const char* toStringCargo(struct Cargo_t* _cargo) {
  if (!_cargo) return "";
  return toStringCargoByType(_cargo->m_type);
}

uint16_t getNCargo() {
  return m_nCargo;
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
