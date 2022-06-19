#include "conveyor.h"
#include "../chunk.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"

/// ///

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo == NULL) return;
  if (_building->m_progress < TILE_PIX) {
    _building->m_progress += _tick * _building->m_stored[0]; // Stored[0] used to hold conveyor speed

    // Handle filters vs. splitters
    enum kDir direction;
    if (_building->m_subType.conveyor == kFilterL) {
      // First encounter with an object? TODO can everything which can place an item on the chunk do this instead?
      if (_building->m_mode == kNoCargo) {
        _building->m_mode = loc->m_cargo->m_type; // Note: This CANNOT be undone without re-writing the building
      }
      direction = (_building->m_mode == loc->m_cargo->m_type ? _building->m_nextDir[1] :  _building->m_nextDir[0]);  
    } else {
      direction = _building->m_nextDir[_building->m_mode];
    }

    // All of this only needs to be done if we are rendering
    if (_tick == NEAR_TICK_AMOUNT) {
      uint16_t hide = 0;
      if (_building->m_subType.conveyor == kTunnelIn) {
        hide = (TOT_WORLD_PIX_X * 2);
      }

      switch (direction) {
        case SN:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], (_building->m_pix_x + hide)*_zoom, (_building->m_pix_y - _building->m_progress + hide)*_zoom); break;
        case NS:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], (_building->m_pix_x + hide)*_zoom, (_building->m_pix_y + _building->m_progress + hide)*_zoom); break;
        case EW:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], (_building->m_pix_x - _building->m_progress + hide)*_zoom, (_building->m_pix_y + hide)*_zoom); break;
        case WE:; pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], (_building->m_pix_x + _building->m_progress + hide)*_zoom, (_building->m_pix_y + hide)*_zoom); break;
        case kDirN:; break;
      }
    }
    //pd->system->logToConsole("MOVE %i %i, %i", loc->m_pix_x, loc->m_pix_y, loc->m_progress);
  }

  // Handle filters vs. splitters
  struct Location_t* nextLoc = NULL;
  if (_building->m_subType.conveyor == kFilterL) {
    nextLoc = (_building->m_mode == loc->m_cargo->m_type ? _building->m_next[1] :  _building->m_next[0]);  
  } else {
    nextLoc = _building->m_next[_building->m_mode];
  }

  if (_building->m_progress >= TILE_PIX && nextLoc->m_cargo == NULL) {
    struct Cargo_t* theCargo = loc->m_cargo;

    // Move cargo
    nextLoc->m_cargo = theCargo;
    loc->m_cargo = NULL;
    // Carry over any excess ticks
    if (nextLoc->m_building) {
      nextLoc->m_building->m_progress = _building->m_progress - TILE_PIX;
    }
    // Cargo moves between chunks?
    if (nextLoc->m_chunk != loc->m_chunk) {
      //pd->system->logToConsole("CHANGE CHUNK");
      chunkRemoveCargo(loc->m_chunk, theCargo);
      chunkAddCargo(nextLoc->m_chunk, theCargo);
      queueUpdateRenderList();
    }
    // Cycle outputs
    switch (_building->m_subType.conveyor) {
      case kSplitI:; case kSplitL:; _building->m_mode = (_building->m_mode + 1) % 2; break;
      case kSplitT:; _building->m_mode = (_building->m_mode + 1) % 3; break;
      case kBelt:; case kFilterL:; case kTunnelIn:; case kTunnelOut:; case kNConvSubTypes:; break;
    }
  }
}

bool canBePlacedConveyor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  bool floor = false;
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile < FLOOR_TILES) floor = true;
  else if (t->m_tile >= getSprite16_idx(8, 2) && t->m_tile < getSprite16_idx(8+4, 2)) floor = true;

  if (_subType.conveyor == kTunnelIn) {
    t = getTunnelOutTile(_loc, _dir);
    if (t->m_tile < FLOOR_TILES) floor &= true;
    else if (t->m_tile >= getSprite16_idx(8, 2) && t->m_tile < getSprite16_idx(8+4, 2)) floor &= true;
  }

  if (!floor) return false;

  // We can place a conveyor on top of an existing one
  bool existingGood = false;
  if (_loc->m_building == NULL || _loc->m_building->m_type == kConveyor) existingGood = true;

  if (_subType.conveyor == kTunnelIn) {
    struct Location_t* tunnelOut = getTunnelOutLocation(_loc, _dir);
    if (tunnelOut->m_building == NULL || tunnelOut->m_building->m_type == kConveyor) existingGood &= true;
  }

  if (!existingGood) return false;

  return true;
}

#define TUNNEL_HOPS 2
struct Location_t* getTunnelOutLocation(struct Location_t* _in, enum kDir _dir) {
  switch (_dir) {
    case SN: return getLocation(_in->m_x, _in->m_y - TUNNEL_HOPS); break;
    case WE: return getLocation(_in->m_x + TUNNEL_HOPS, _in->m_y); break;
    case NS: return getLocation(_in->m_x, _in->m_y + TUNNEL_HOPS); break;
    case EW: return getLocation(_in->m_x - TUNNEL_HOPS, _in->m_y); break;
    case kDirN: break;
  }
  return NULL;
}

struct Tile_t* getTunnelOutTile(struct Location_t* _in, enum kDir _dir) {
  switch (_dir) {
    case SN: return getTile(_in->m_x, _in->m_y - TUNNEL_HOPS); break;
    case WE: return getTile(_in->m_x + TUNNEL_HOPS, _in->m_y); break;
    case NS: return getTile(_in->m_x, _in->m_y + TUNNEL_HOPS); break;
    case EW: return getTile(_in->m_x - TUNNEL_HOPS, _in->m_y); break;
    case kDirN: break;
  }
  return NULL;
}

void assignNeighborsConveyor(struct Building_t* _building) {
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, 1, &above, &below, &left, &right);

  struct Location_t* aboveTunnel;
  struct Location_t* belowTunnel;
  struct Location_t* leftTunnel;
  struct Location_t* rightTunnel;
  getBuildingNeighbors(_building, TUNNEL_HOPS, &aboveTunnel, &belowTunnel, &leftTunnel, &rightTunnel);

  if (_building->m_subType.conveyor == kBelt || _building->m_subType.conveyor == kTunnelOut) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = above;
                _building->m_nextDir[0] = SN; break;
      case NS:; _building->m_next[0]    = below;
                _building->m_nextDir[0] = NS; break;
      case WE:; _building->m_next[0]    = right;
                _building->m_nextDir[0] = WE; break;
      case EW:; _building->m_next[0]    = left;
                _building->m_nextDir[0] = EW; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kTunnelIn) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = aboveTunnel;
                _building->m_nextDir[0] = SN; break;
      case NS:; _building->m_next[0]    = belowTunnel;
                _building->m_nextDir[0] = NS; break;
      case WE:; _building->m_next[0]    = rightTunnel;
                _building->m_nextDir[0] = WE; break;
      case EW:; _building->m_next[0]    = leftTunnel;
                _building->m_nextDir[0] = EW; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kSplitI) {
    switch (_building->m_dir) {
      case WE:; case EW:; _building->m_next[0]    = above; _building->m_next[1]    = below; 
                          _building->m_nextDir[0] = SN;    _building->m_nextDir[1] = NS; break;
      case SN:; case NS:; _building->m_next[0]    = left;  _building->m_next[1]    = right;
                          _building->m_nextDir[0] = EW;    _building->m_nextDir[1] = WE; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kSplitL || _building->m_subType.conveyor == kFilterL) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = above; _building->m_next[1]    = right;
                _building->m_nextDir[0] = SN;    _building->m_nextDir[1] = WE; break;
      case WE:; _building->m_next[0]    = right; _building->m_next[1]    = below;
                _building->m_nextDir[0] = WE;    _building->m_nextDir[1] = NS; break;
      case NS:; _building->m_next[0]    = below; _building->m_next[1]    = left;
                _building->m_nextDir[0] = NS;    _building->m_nextDir[1] = EW; break;
      case EW:; _building->m_next[0]    = left;  _building->m_next[1]    = above;
                _building->m_nextDir[0] = EW;    _building->m_nextDir[1] = SN; break;
      case kDirN:; break;
    }
  } else if (_building->m_subType.conveyor == kSplitT) {
    switch (_building->m_dir) {
      case SN:; _building->m_next[0]    = left;  _building->m_next[1]    = above;  _building->m_next[2]    = right;
                _building->m_nextDir[0] = EW;    _building->m_nextDir[1] = SN;     _building->m_nextDir[2] = WE; break;
      case WE:; _building->m_next[0]    = above; _building->m_next[1]    = right;  _building->m_next[2]    = below;
                _building->m_nextDir[0] = SN;    _building->m_nextDir[1] = WE;     _building->m_nextDir[2] = NS; break;
      case NS:; _building->m_next[0]    = right; _building->m_next[1]    = below;  _building->m_next[2]    = left; 
                _building->m_nextDir[0] = WE;    _building->m_nextDir[1] = NS;     _building->m_nextDir[2] = EW; break;
      case EW:; _building->m_next[0]    = below; _building->m_next[1]    = left;   _building->m_next[2]    = above;
                _building->m_nextDir[0] = NS;    _building->m_nextDir[1] = EW;     _building->m_nextDir[2] = SN; break;
      case kDirN:; break;
    }
  }
}

void buildingSetupConveyor(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {

    switch (_building->m_subType.conveyor) {
      case kBelt:;     _building->m_image[zoom] = getSprite16(0, CONV_START_Y + _building->m_dir, zoom); break;
      case kSplitI:;   _building->m_image[zoom] = getSprite16(0, 11 + _building->m_dir, zoom); break;
      case kSplitL:;   _building->m_image[zoom] = getSprite16(1, 11 + _building->m_dir, zoom); break;
      case kSplitT:;   _building->m_image[zoom] = getSprite16(2, 11 + _building->m_dir, zoom); break;
      case kFilterL:;  _building->m_image[zoom] = getSprite16(3, 11 + _building->m_dir, zoom); break;
      case kTunnelIn:; _building->m_image[zoom] = getSprite16(8, 11 + _building->m_dir, zoom); break;
      case kTunnelOut:;_building->m_image[zoom] = getSprite16(9, 11 + _building->m_dir, zoom); break;
      case kNConvSubTypes:; break;
    }

    // Animate the belt at zoom=2
    if (_building->m_subType.conveyor == kBelt) {
      PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
      if (_building->m_sprite[zoom] == NULL) {
        _building->m_sprite[zoom] = pd->sprite->newSprite();
      }
      pd->sprite->setBounds(_building->m_sprite[zoom], bound);
      pd->sprite->setImage(_building->m_sprite[zoom], getConveyorMaster(zoom, _building->m_dir), kBitmapUnflipped);
      pd->sprite->moveTo(_building->m_sprite[zoom], _building->m_pix_x*zoom, _building->m_pix_y*zoom);
      pd->sprite->setZIndex(_building->m_sprite[zoom], Z_INDEX_CONVEYOR);
    } else {
      if (_building->m_sprite[zoom] == NULL) {
        pd->sprite->freeSprite(_building->m_sprite[zoom]);
        _building->m_sprite[zoom] = NULL;
      }
    }
  }

  // Set the starting speed (TODO: Put this back to 1 and impliment an upgrade mechanic)
  _building->m_stored[0] = 2;
}