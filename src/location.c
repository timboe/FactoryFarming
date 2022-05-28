#include "location.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"

uint16_t m_nConveyors = 0;

struct Location_t* m_locations = NULL;

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick);

/// ///

uint16_t getNConveyors() {
  return m_nConveyors;
}

struct Location_t* getLocation(int32_t _x, int32_t _y) {
  if (_x < 0) _x += TOT_TILES_X;
  else if (_x >= TOT_TILES_X) _x -= TOT_TILES_X;
  if (_y < 0) _y += TOT_TILES_Y;
  else if (_y >= TOT_TILES_Y) _y -= TOT_TILES_Y;
  const uint32_t l = (TOT_TILES_X * _y) + _x;
  return &m_locations[l];
}

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick) {
  if (_loc->m_cargo == NULL) return;
  if (_loc->m_progress < TILE_PIX) {
    _loc->m_progress += _tick;
    switch (_loc->m_convDir) {
      case SN:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_pix_x, _loc->m_pix_y - _loc->m_progress); break;
      case NS:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_pix_x, _loc->m_pix_y + _loc->m_progress); break;
      case EW:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_pix_x + _loc->m_progress, _loc->m_pix_y); break;
      case WE:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_pix_x - _loc->m_progress, _loc->m_pix_y); break;
      case kConvDirN:; break;
    }
    //pd->system->logToConsole("MOVE %i %i, %i", _loc->m_pix_x, _loc->m_pix_y, _loc->m_progress);
  }
  if (_loc->m_progress >= TILE_PIX && _loc->m_next != NULL && _loc->m_next->m_cargo == NULL) {
    struct Cargo_t* theCargo = _loc->m_cargo;
    _loc->m_next->m_cargo = theCargo;
    _loc->m_cargo = NULL;
    _loc->m_next->m_progress = _loc->m_progress - TILE_PIX;
    // Cargo moves between chunks?
    if (_loc->m_next->m_chunk != _loc->m_chunk) {
      //pd->system->logToConsole("CHANGE CHUNK");
      chunkRemoveCargo(_loc->m_chunk, theCargo);
      chunkAddCargo(_loc->m_next->m_chunk, theCargo);
    }
  }
}

bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir) {
  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_type != kEmpty) return false;

  loc->m_x = _x;
  loc->m_y = _y;
  struct Chunk_t* chunk = getChunk_fromLocation(loc);

  loc->m_sprite = pd->sprite->newSprite();
  loc->m_image = getSprite16(0, CONV_START_Y + _dir);
  loc->m_cargo = NULL;
  loc->m_type = kConveyor;
  loc->m_chunk = chunk;
  loc->m_convDir = _dir;
  loc->m_updateFn = &conveyorUpdateFn;
  loc->m_progress = 0;

  loc->m_pix_x = TILE_PIX*_x + TILE_PIX/2.0;
  loc->m_pix_y = TILE_PIX*_y + TILE_PIX/2.0;
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TILE_PIX};
  pd->sprite->setBounds(loc->m_sprite, bound);
  pd->sprite->setImage(loc->m_sprite, getConveyorMaster(_dir), kBitmapUnflipped);
  pd->sprite->moveTo(loc->m_sprite, loc->m_pix_x, loc->m_pix_y);
  pd->sprite->setZIndex(loc->m_sprite, 0);

  struct Location_t* above = getLocation(_x, _y - 1);
  struct Location_t* below = getLocation(_x, _y + 1);
  struct Location_t* left  = getLocation(_x - 1, _y);
  struct Location_t* right = getLocation(_x + 1, _y);
  switch (_dir) {
    case SN:;
      if (above->m_type == kConveyor) loc->m_next = above;
      if (below->m_type == kConveyor) below->m_next = loc;
      break;
    case NS:;
      if (above->m_type == kConveyor) above->m_next = loc;
      if (below->m_type == kConveyor) loc->m_next = below;
      break;
    case EW:;
      if (left->m_type  == kConveyor) left->m_next = loc;
      if (right->m_type == kConveyor) loc->m_next = right;
      break;
    case WE:;
      if (left->m_type  == kConveyor) loc->m_next = left;
      if (right->m_type == kConveyor) right->m_next = loc;
      break;
    case kConvDirN:;
  }

  chunkAddLocation(chunk, loc);

  //pd->system->logToConsole("ADD TO CHUNK: %i %i with %i sprites", chunk->m_x, chunk->m_y, chunk->m_nLocations);

  // Bake into the background 
  renderChunkBackgroundImage(chunk);
  updateRenderList();

  ++m_nConveyors;
  return true;
}

void initLocation() {
  m_locations = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Location_t));
  memset(m_locations, 0, TOT_TILES * sizeof(struct Location_t));
}