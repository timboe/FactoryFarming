#include "cargo.h"
#include "location.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"

uint16_t m_nCargo = 0;

uint16_t m_cargoSearchLocation = 0;

struct Cargo_t* m_worldCargos;

uint16_t getNCargo() {
  return m_nCargo;
}

struct Cargo_t* cargoManagerNewCargo() {
  for (uint8_t try = 0; try < 2; ++try) {
    const uint32_t start = (try == 0 ? m_cargoSearchLocation : 0);
    for (uint32_t i = start; i < TOT_TILES; ++i) {
      if (m_worldCargos[i].m_inUse == false) {
        ++m_nCargo;
        ++m_cargoSearchLocation; // Search from the start next time
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


bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type) {

  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_cargo != NULL) return false;
  if (loc->m_type != kConveyor) return false;

  loc->m_x = _x;
  loc->m_y = _y; // Should be assured already by loc->m_type == kConveyor check
  struct Chunk_t* chunk = getChunk_fromLocation(loc);

  //pd->system->logToConsole("Set cargo, current %i", (int)loc->m_cargo);

  LCDBitmap* image = NULL;
  switch (_type) {
    case kApple: image = getSprite16(1,1); break;
    case kNoCargo:;
  }

  struct Cargo_t* cargo = cargoManagerNewCargo();
  if (cargo->m_sprite == NULL) cargo->m_sprite = pd->sprite->newSprite();
  cargo->m_type = _type;

  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TILE_PIX};
  pd->sprite->setBounds(cargo->m_sprite, bound);
  pd->sprite->setImage(cargo->m_sprite, image, kBitmapUnflipped);
  pd->sprite->moveTo(cargo->m_sprite, loc->m_pix_x, loc->m_pix_y);
  pd->sprite->setZIndex(cargo->m_sprite, 32767);

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