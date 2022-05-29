#include "cargo.h"
#include "location.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"

uint16_t m_nCargo = 0;

uint16_t m_cargoSearchLocation = 0;

struct Cargo_t* m_worldCargos;

void cargoSpriteSetup(struct Cargo_t* _cargo, struct Location_t* _loc, uint16_t ix, uint16_t _iy, uint8_t _zoom);

/// ///

uint16_t getNCargo() {
  return m_nCargo;
}

struct Cargo_t* cargoManagerNewCargo() {
  for (uint8_t try = 0; try < 2; ++try) {
    const uint32_t start = (try == 0 ? m_cargoSearchLocation : 0);
    for (uint32_t i = start; i < TOT_TILES; ++i) {
      if (m_worldCargos[i].m_inUse == false) {
        ++m_nCargo;
        ++m_cargoSearchLocation;
        m_worldCargos[i].m_inUse = true;
        return &(m_worldCargos[i]);
      }
    }
  }
  #ifdef DEV
  pd->system->error("Cannot allocate any more cargo!");
  #endif
  return NULL;
}

void cargoManagerFreeCargo(struct Cargo_t* _cargo) {
  _cargo->m_inUse = false;
  m_cargoSearchLocation = _cargo->m_index;
  --m_nCargo;
}

void cargoSpriteSetup(struct Cargo_t* _cargo, struct Location_t* _loc, uint16_t _ix, uint16_t _iy, uint8_t _zoom) {
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*_zoom, .height = TILE_PIX*_zoom};
  pd->sprite->setBounds(_cargo->m_sprite[_zoom], bound);
  pd->sprite->setImage(_cargo->m_sprite[_zoom], getSprite16(_ix, _iy, _zoom), kBitmapUnflipped);
  pd->sprite->moveTo(_cargo->m_sprite[_zoom], _loc->m_pix_x*_zoom, _loc->m_pix_y*_zoom);
  pd->sprite->setZIndex(_cargo->m_sprite[_zoom], Z_INDEX_CARGO);
}

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type) {

  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_cargo != NULL) return false;
  if (loc->m_type != kConveyor) return false;

  struct Chunk_t* chunk = getChunk_fromLocation(loc);

  //pd->system->logToConsole("Set cargo, current %i", (int)loc->m_cargo);

  uint16_t ix = 0, iy = 0;
  switch (_type) {
    case kApple:; ix = 1; iy = 1; break;
    case kNoCargo:;
  }

  struct Cargo_t* cargo = cargoManagerNewCargo();
  if (cargo->m_sprite[1] == NULL) {
    cargo->m_sprite[1] = pd->sprite->newSprite();
    cargo->m_sprite[2] = pd->sprite->newSprite();
    cargo->m_sprite[4] = pd->sprite->newSprite();
  }
  cargo->m_type = _type;

  cargoSpriteSetup(cargo, loc, ix, iy, 1);
  cargoSpriteSetup(cargo, loc, ix, iy, 2);
  cargoSpriteSetup(cargo, loc, ix, iy, 4);

  loc->m_cargo = cargo;

  chunkAddCargo(chunk, cargo);

  updateRenderList(); // Do we want to do this here?
  return true;
}

void initCargo() {
  m_worldCargos = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Cargo_t));
  memset(m_worldCargos, 0, TOT_TILES * sizeof(struct Cargo_t));
  for (uint32_t i = 0; i < TOT_TILES; ++i) {
    m_worldCargos[i].m_index = i;
  }
}