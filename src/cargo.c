#include "cargo.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"

const int32_t SIZE_CARGO = TOT_CARGO_OR_BUILDINGS * sizeof(struct Cargo_t);

uint16_t m_nCargo = 0;

uint16_t m_cargoSearchLocation = 0;

struct Cargo_t* m_cargos;

void cargoSpriteSetup(struct Cargo_t* _cargo, struct Location_t* _loc, uint16_t ix, uint16_t _iy, uint8_t _zoom);

/// ///

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

void cargoManagerFreeCargo(struct Cargo_t* _cargo) {
  _cargo->m_type = kNoCargo;
  m_cargoSearchLocation = _cargo->m_index;
  --m_nCargo;
}

void cargoSpriteSetup(struct Cargo_t* _cargo, struct Location_t* _loc, uint16_t _ix, uint16_t _iy, uint8_t _zoom) {
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*_zoom, .height = TILE_PIX*_zoom};
  pd->sprite->setBounds(_cargo->m_sprite[_zoom], bound);
  pd->sprite->setImage(_cargo->m_sprite[_zoom], getSprite16(_ix, _iy, _zoom), kBitmapUnflipped);
  pd->sprite->moveTo(_cargo->m_sprite[_zoom], (TILE_PIX*_loc->m_x + TILE_PIX/2.0)*_zoom, (TILE_PIX*_loc->m_y + TILE_PIX/2.0)*_zoom);
  pd->sprite->setZIndex(_cargo->m_sprite[_zoom], Z_INDEX_CARGO);
}

bool newCargo(struct Location_t* _loc, enum kCargoType _type, bool _addedByPlayer) {
  bool addedByPlayer = true;
  if (_loc->m_cargo != NULL) return false;

  struct Chunk_t* chunk = _loc->m_chunk;

  //pd->system->logToConsole("Set cargo, current %i", (int)loc->m_cargo);

  uint16_t ix = 0, iy = 0;
  switch (_type) {
    case kApple:; ix = 1; iy = 1; break;
    case kCheese:; ix = 2; iy = 1; break;
    case kNoCargo:;
  }

  struct Cargo_t* cargo = cargoManagerNewCargo(_type);
  if (!cargo) { // Run out of slots
    return false;
  }

  if (cargo->m_sprite[1] == NULL) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      cargo->m_sprite[zoom] = pd->sprite->newSprite();
    }
  }


  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    cargoSpriteSetup(cargo, _loc, ix, iy, zoom);
  }

  _loc->m_cargo = cargo;
  if (_loc->m_building) {
    _loc->m_building->m_progress = 0;
  }

  chunkAddCargo(chunk, cargo);

  if (_addedByPlayer) updateRenderList(); // Do we want to do this here?
  else queueUpdateRenderList();
  return true;
}

void resetCargo() {
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      pd->sprite->freeSprite(m_cargos[i].m_sprite[zoom]);
    }
  }
  memset(m_cargos, 0, SIZE_CARGO);
  for (uint32_t i = 0; i < TOT_CARGO_OR_BUILDINGS; ++i) {
    m_cargos[i].m_index = i;
  }
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