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
    switch (_loc->m_nextDir[_loc->m_mode]) {
      case SN:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], _loc->m_pix_x*_zoom, (_loc->m_pix_y - _loc->m_progress)*_zoom); break;
      case NS:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], _loc->m_pix_x*_zoom, (_loc->m_pix_y + _loc->m_progress)*_zoom); break;
      case EW:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], (_loc->m_pix_x - _loc->m_progress)*_zoom, _loc->m_pix_y*_zoom); break;
      case WE:; pd->sprite->moveTo(_loc->m_cargo->m_sprite[_zoom], (_loc->m_pix_x + _loc->m_progress)*_zoom, _loc->m_pix_y*_zoom); break;
      case kConvDirN:; break;
    }
    //pd->system->logToConsole("MOVE %i %i, %i", _loc->m_pix_x, _loc->m_pix_y, _loc->m_progress);
  }
  if (_loc->m_progress >= TILE_PIX && _loc->m_next[_loc->m_mode] != NULL && _loc->m_next[_loc->m_mode]->m_cargo == NULL) {
    struct Cargo_t* theCargo = _loc->m_cargo;
    _loc->m_next[_loc->m_mode]->m_cargo = theCargo;
    _loc->m_cargo = NULL;
    _loc->m_next[_loc->m_mode]->m_progress = _loc->m_progress - TILE_PIX;
    // Cargo moves between chunks?
    if (_loc->m_next[_loc->m_mode]->m_chunk != _loc->m_chunk) {
      //pd->system->logToConsole("CHANGE CHUNK");
      chunkRemoveCargo(_loc->m_chunk, theCargo);
      chunkAddCargo(_loc->m_next[_loc->m_mode]->m_chunk, theCargo);
      queueUpdateRenderList();
    }
    switch (_loc->m_convSubType) {
      case kSplitI:; case kSplitL:; case kFilterL:; _loc->m_mode = (_loc->m_mode + 1) % 2; break;
      case kSplitT:; _loc->m_mode = (_loc->m_mode + 1) % 3; break;
      case kBelt:; default: break;
    }
  }
}

void clearLocation(struct Location_t* _loc) {
  bool removed = false;

  if (_loc->m_cargo) {
    chunkRemoveCargo(_loc->m_chunk, _loc->m_cargo);
    cargoManagerFreeCargo(_loc->m_cargo);
    _loc->m_cargo = NULL;
    removed = true;
  }

  if (_loc->m_type == kConveyor) {
    _loc->m_type = kEmpty;
    chunkRemoveLocation(_loc->m_chunk, _loc);
    renderChunkBackgroundImage(_loc->m_chunk);
    --m_nConveyors;
    removed = true;
  }

  if (removed) {
    updateRenderList();
  }
}

void locationSpriteSetup(struct Location_t* _loc, enum kConvDir _dir, uint8_t _zoom) {
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*_zoom, .height = TILE_PIX*_zoom};
  pd->sprite->setBounds(_loc->m_sprite[_zoom], bound);
  pd->sprite->setImage(_loc->m_sprite[_zoom], getConveyorMaster(_zoom, _dir), kBitmapUnflipped);
  pd->sprite->moveTo(_loc->m_sprite[_zoom], _loc->m_pix_x*_zoom, _loc->m_pix_y*_zoom);
  pd->sprite->setZIndex(_loc->m_sprite[_zoom], Z_INDEX_CONVEYOR);
}

bool newConveyor(struct Location_t* _loc, enum kConvDir _dir, enum kConvSubType _subType) {
  bool newToChunk = true;
  if (_loc->m_type == kConveyor) { // TODO - cover any other options here
    newToChunk = false;
  } else if (_loc->m_type == kEmpty) {
    ++m_nConveyors;
  } else {
    return false;
  }

  if (_loc->m_sprite[1] == NULL) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      _loc->m_sprite[zoom] = pd->sprite->newSprite();
    }
  }

  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_subType) {
      case kBelt:;   _loc->m_image[zoom] = getSprite16(0, CONV_START_Y + _dir, zoom); break;
      case kSplitI:; _loc->m_image[zoom] = getSprite16(0, 11 + _dir, zoom); break;
      case kSplitL:; _loc->m_image[zoom] = getSprite16(1, 11 + _dir, zoom); break;
      case kSplitT:; _loc->m_image[zoom] = getSprite16(2, 11 + _dir, zoom); break;
      case kFilterL:; default: break;
    }
  }
  _loc->m_type = kConveyor;
  _loc->m_convSubType = _subType;
  _loc->m_convDir = _dir;
  _loc->m_updateFn = &conveyorUpdateFn;
  _loc->m_progress = 0;
  _loc->m_mode = 0;


  if (_subType == kBelt) {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      locationSpriteSetup(_loc, _dir, zoom);
    }
  }

  struct Location_t* above = getLocation(_loc->m_x, _loc->m_y - 1);
  struct Location_t* below = getLocation(_loc->m_x, _loc->m_y + 1);
  struct Location_t* left  = getLocation(_loc->m_x - 1, _loc->m_y);
  struct Location_t* right = getLocation(_loc->m_x + 1, _loc->m_y);
  if (_subType == kBelt) {
    switch (_dir) {
      case SN:; _loc->m_next[0]    = above;
                _loc->m_nextDir[0] = SN; break;
      case NS:; _loc->m_next[0]    = below;
                _loc->m_nextDir[0] = NS; break;
      case WE:; _loc->m_next[0]    = right;
                _loc->m_nextDir[0] = WE; break;
      case EW:; _loc->m_next[0]    = left;
                _loc->m_nextDir[0] = EW; break;
      case kConvDirN:; break;
    }
  } else if (_subType == kSplitI) {
    switch (_dir) {
      case WE:; case EW:; _loc->m_next[0]    = above; _loc->m_next[1]    = below; 
                          _loc->m_nextDir[0] = SN;    _loc->m_nextDir[1] = NS; break;
      case SN:; case NS:; _loc->m_next[0]    = left;  _loc->m_next[0]    = right;
                          _loc->m_nextDir[0] = EW;    _loc->m_nextDir[1] = WE; break;
      case kConvDirN:; break;
    }
  } else if (_subType == kSplitL) {
    switch (_dir) {
      case SN:; _loc->m_next[0]    = above; _loc->m_next[1]    = right;
                _loc->m_nextDir[0] = SN;    _loc->m_nextDir[1] = WE; break;
      case WE:; _loc->m_next[0]    = right; _loc->m_next[1]    = below;
                _loc->m_nextDir[0] = WE;    _loc->m_nextDir[1] = NS; break;
      case NS:; _loc->m_next[0]    = below; _loc->m_next[1]    = left;
                _loc->m_nextDir[0] = NS;    _loc->m_nextDir[1] = EW; break;
      case EW:; _loc->m_next[0]    = left;  _loc->m_next[1]    = above;
                _loc->m_nextDir[0] = EW;    _loc->m_nextDir[1] = SN; break;
      case kConvDirN:; break;
    }
  } else if (_subType == kSplitT) {
    switch (_dir) {
      case SN:; _loc->m_next[0]    = left;  _loc->m_next[1]    = above;  _loc->m_next[2]    = right;
                _loc->m_nextDir[0] = EW;    _loc->m_nextDir[1] = SN;     _loc->m_nextDir[2] = WE; break;
      case WE:; _loc->m_next[0]    = above; _loc->m_next[1]    = right;  _loc->m_next[2]    = below;
                _loc->m_nextDir[0] = SN;    _loc->m_nextDir[1] = WE;     _loc->m_nextDir[2] = NS; break;
      case NS:; _loc->m_next[0]    = right; _loc->m_next[1]    = below;  _loc->m_next[2]    = left; 
                _loc->m_nextDir[0] = WE;    _loc->m_nextDir[1] = NS;     _loc->m_nextDir[2] = EW; break;
      case EW:; _loc->m_next[0]    = below; _loc->m_next[1]    = left;   _loc->m_next[2]    = above;
                _loc->m_nextDir[0] = NS;    _loc->m_nextDir[1] = EW;     _loc->m_nextDir[2] = SN; break;
      case kConvDirN:; break;
    }
  }

  // Add to the active/render list
  if (newToChunk) {
    chunkAddLocation(_loc->m_chunk, _loc); // Careful, no de-duplication in here, for speed
  }

  //pd->system->logToConsole("ADD TO CHUNK: %i %i with %i sprites", chunk->m_x, chunk->m_y, chunk->m_nLocations);

  // Bake into the background 
  renderChunkBackgroundImage(_loc->m_chunk);
  updateRenderList();

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