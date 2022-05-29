#include "location.h"
#include "generate.h"
#include "chunk.h"
#include "sprite.h"
#include "render.h"

uint16_t m_nConveyors = 0;

struct Location_t* m_locations = NULL;

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick, uint8_t _zoom);

void locationSpriteSetup(struct Location_t* _loc, enum kConvDir _dir, uint8_t _zoom);

/// ///

uint16_t getNConveyors() {
  return m_nConveyors;
}

struct Location_t* getLocation_noCheck(int32_t _x, int32_t _y) {
  const uint32_t l = (TOT_TILES_X * _y) + _x;
  return &m_locations[l];
}

struct Location_t* getLocation(int32_t _x, int32_t _y) {
  if (_x < 0) _x += TOT_TILES_X;
  else if (_x >= TOT_TILES_X) _x -= TOT_TILES_X;
  if (_y < 0) _y += TOT_TILES_Y;
  else if (_y >= TOT_TILES_Y) _y -= TOT_TILES_Y;
  return getLocation_noCheck(_x, _y);
}

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick, uint8_t _zoom) {
  if (_loc->m_cargo == NULL) return;
  if (_loc->m_progress < TILE_PIX) {
    _loc->m_progress += _tick;
    switch (_loc->m_convDir) {
      case SN:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], _loc->m_pix_x*_zoom, (_loc->m_pix_y - _loc->m_progress)*_zoom); break;
      case NS:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], _loc->m_pix_x*_zoom, (_loc->m_pix_y + _loc->m_progress)*_zoom); break;
      case EW:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], (_loc->m_pix_x + _loc->m_progress)*_zoom, _loc->m_pix_y*_zoom); break;
      case WE:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], (_loc->m_pix_x - _loc->m_progress)*_zoom, _loc->m_pix_y*_zoom); break;
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

void locationSpriteSetup(struct Location_t* _loc, enum kConvDir _dir, uint8_t _zoom) {
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*_zoom, .height = TILE_PIX*_zoom};
  pd->sprite->setBounds(_loc->m_sprite[_zoom], bound);
  pd->sprite->setImage(_loc->m_sprite[_zoom], getConveyorMaster(_zoom, _dir), kBitmapUnflipped);
  pd->sprite->moveTo(_loc->m_sprite[_zoom], _loc->m_pix_x*_zoom, _loc->m_pix_y*_zoom);
  pd->sprite->setZIndex(_loc->m_sprite[_zoom], Z_INDEX_CONVEYOR);
}

bool newConveyor(struct Location_t* _loc, enum kConvDir _dir) {
  if (_loc->m_type != kEmpty) return false;

  if (_loc->m_sprite[1] == NULL) {
    _loc->m_sprite[1] = pd->sprite->newSprite();
    _loc->m_sprite[2] = pd->sprite->newSprite();
    _loc->m_sprite[4] = pd->sprite->newSprite();
  }
  _loc->m_image[1] = getSprite16(0, CONV_START_Y + _dir, 1);
  _loc->m_image[2] = getSprite16(0, CONV_START_Y + _dir, 2);
  _loc->m_image[4] = getSprite16(0, CONV_START_Y + _dir, 4);
  _loc->m_cargo = NULL;
  _loc->m_type = kConveyor;
  _loc->m_convDir = _dir;
  _loc->m_updateFn = &conveyorUpdateFn;
  _loc->m_progress = 0;

  locationSpriteSetup(_loc, _dir, 1);
  locationSpriteSetup(_loc, _dir, 2);
  locationSpriteSetup(_loc, _dir, 4);

  struct Location_t* above = getLocation(_loc->m_x, _loc->m_y - 1);
  struct Location_t* below = getLocation(_loc->m_x, _loc->m_y + 1);
  struct Location_t* left  = getLocation(_loc->m_x - 1, _loc->m_y);
  struct Location_t* right = getLocation(_loc->m_x + 1, _loc->m_y);
  switch (_dir) {
    case SN:;
      if (above->m_type == kConveyor) _loc->m_next = above;
      if (below->m_type == kConveyor) below->m_next = _loc;
      break;
    case NS:;
      if (above->m_type == kConveyor) above->m_next = _loc;
      if (below->m_type == kConveyor) _loc->m_next = below;
      break;
    case EW:;
      if (left->m_type  == kConveyor) left->m_next = _loc;
      if (right->m_type == kConveyor) _loc->m_next = right;
      break;
    case WE:;
      if (left->m_type  == kConveyor) _loc->m_next = left;
      if (right->m_type == kConveyor) right->m_next = _loc;
      break;
    case kConvDirN:;
  }

  // Add to the active/render list
  chunkAddLocation(_loc->m_chunk, _loc);

  //pd->system->logToConsole("ADD TO CHUNK: %i %i with %i sprites", chunk->m_x, chunk->m_y, chunk->m_nLocations);

  // Bake into the background 
  renderChunkBackgroundImage(_loc->m_chunk);
  updateRenderList();

  ++m_nConveyors;
  return true;
}

void initLocation() {
  m_locations = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Location_t));
  memset(m_locations, 0, TOT_TILES * sizeof(struct Location_t));
  for (uint32_t x = 0; x < TOT_TILES_X; ++x) {
    for (uint32_t y = 0; y < TOT_TILES_Y; ++y) {
      struct Location_t* loc = getLocation_noCheck(x, y);
      loc->m_x = x;
      loc->m_y = y;
      loc->m_pix_x = TILE_PIX*x + TILE_PIX/2.0;
      loc->m_pix_y = TILE_PIX*y + TILE_PIX/2.0;
      loc->m_chunk = getChunk_fromLocation(loc);
    }
  }
}