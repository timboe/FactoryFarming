#include "conveyor.h"
#include "../chunk.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../ui.h"

int8_t getConveyorDirection(enum kConvSubType _subType, enum kDir _dir, uint8_t _progress );

const char* getTransitText(int8_t _d);

/// ///

void conveyorLocationUpdate(struct Building_t* _building, uint8_t _zoom) {
  const int8_t x = (int8_t) _building->m_stored[1];
  const int8_t y = (int8_t) _building->m_stored[2];
  const bool hide  = _building->m_stored[3];
  struct Location_t* loc = _building->m_location;
  if (hide) {
    pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], 65536, 65536);
  } else {
    pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], 
                      (_building->m_pix_x + loc->m_pix_off_x + x)*_zoom, 
                      (_building->m_pix_y + loc->m_pix_off_y + y)*_zoom);
  }
}

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo == NULL) return;
  if (_building->m_progress < TILE_PIX) {
    _building->m_progress += _tick * _building->m_stored[0]; // Stored[0] used to hold conveyor speed

    // Handle filters vs. splitters
    enum kDir direction;
    if (_building->m_subType.conveyor >= kFilterL) {
      // First encounter with an object? TODO can everything which can place an item on the chunk do this instead?
      if (_building->m_mode.mode16 == kNoCargo) {
        _building->m_mode.mode16 = loc->m_cargo->m_type; // Note: This CANNOT be undone without re-writing the building
      }
      direction = (_building->m_mode.mode16 == loc->m_cargo->m_type ? _building->m_nextDir[1] :  _building->m_nextDir[0]);  
    } else {
      direction = _building->m_nextDir[_building->m_mode.mode16];
    }

    _building->m_stored[3] = (_building->m_subType.conveyor == kTunnelIn); // hide 

    switch (direction) {
      case SN:; _building->m_stored[1] = 0; _building->m_stored[2] = (uint8_t) -_building->m_progress; break;
      case NS:; _building->m_stored[1] = 0; _building->m_stored[2] = _building->m_progress; break;
      case EW:; _building->m_stored[1] = (uint8_t) -_building->m_progress; _building->m_stored[2] = 0; break;
      case WE:; _building->m_stored[1] = _building->m_progress; _building->m_stored[2] = 0; break;
      case kDirN:; break;
    }

    // Only needs to be done if we are rendering
    if (_tick == NEAR_TICK_AMOUNT) {
      conveyorLocationUpdate(_building, _zoom);
    }
    //pd->system->logToConsole("MOVE %i %i, %i", loc->m_pix_x, loc->m_pix_y, loc->m_progress);
  }

  // Handle filters vs. splitters
  struct Location_t* nextLoc = NULL;
  if (_building->m_subType.conveyor >= kFilterL) {
    nextLoc = (_building->m_mode.mode16 == loc->m_cargo->m_type ? _building->m_next[1] :  _building->m_next[0]);  
  } else {
    nextLoc = _building->m_next[_building->m_mode.mode16];
  }

  if (_building->m_progress >= TILE_PIX && nextLoc->m_cargo == NULL) {
    struct Cargo_t* theCargo = loc->m_cargo;

    // Move cargo
    nextLoc->m_cargo = theCargo;
    loc->m_cargo = NULL;
    // Carry over any excess ticks
    if (nextLoc->m_building && nextLoc->m_building->m_type == kConveyor) {
      nextLoc->m_building->m_progress = _building->m_progress - TILE_PIX;
    }
    // Cargo moves between chunks?
    if (nextLoc->m_chunk != loc->m_chunk) {
      //pd->system->logToConsole("CHANGE CHUNK");
      chunkRemoveCargo(loc->m_chunk, theCargo);
      chunkAddCargo(nextLoc->m_chunk, theCargo);
    }
    // Cycle outputs
    //switch (_building->m_subType.conveyor) {
    //  case kSplitI:; case kSplitL:; _building->m_mode.mode16 = (_building->m_mode.mode16 + 1) % 2; break;
    //  case kSplitT:; _building->m_mode.mode16 = (_building->m_mode.mode16 + 1) % 3; break;
    //  case kBelt:; case kFilterI:; case kFilterL:; case kTunnelIn:; case kTunnelOut:; case kNConvSubTypes:; break;
    //}

    // Cycle outputs varient 2
    uint16_t next = 0;
    switch (_building->m_subType.conveyor) {
      case kSplitI:; case kSplitL:; next = (_building->m_mode.mode16 + 1) % 2; break;
      case kSplitT:; next = (_building->m_mode.mode16 + 1) % 3; break;
      default: break;
    }
    if (_building->m_next[ next ]->m_cargo == NULL) _building->m_mode.mode16 = next;
  }
}

bool canBePlacedConveyor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  bool floor = false;
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile < TOT_FLOOR_TILES_INC_PAVED) floor = true;
  else if (t->m_tile >= SPRITE16_ID(8, 16) && t->m_tile < SPRITE16_ID(8+4, 16)) floor = true;

  if (_subType.conveyor == kTunnelIn) {
    t = getTunnelOutTile(_loc, _dir);
    if (t->m_tile < TOT_FLOOR_TILES_INC_PAVED) floor &= true;
    else if (t->m_tile >= SPRITE16_ID(8, 16) && t->m_tile < SPRITE16_ID(8+4, 16)) floor &= true;
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

#define N 0
#define E 1
#define S 2
#define W 3

void assignNeighborsConveyor(struct Building_t* _building) {
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  enum kConvSubType bst = _building->m_subType.conveyor;

  if (bst == kTunnelIn) {
    getBuildingNeighbors(_building, TUNNEL_HOPS, &above, &below, &left, &right);
  } else {
    getBuildingNeighbors(_building, 1, &above, &below, &left, &right);
  }

  switch (getConveyorDirection(bst, _building->m_dir, 0)) {
    case N: _building->m_next[0] = above; _building->m_nextDir[0] = SN; break;
    case E: _building->m_next[0] = right; _building->m_nextDir[0] = WE; break;
    case S: _building->m_next[0] = below; _building->m_nextDir[0] = NS; break;
    case W: _building->m_next[0] = left;  _building->m_nextDir[0] = EW; break;
  }

  if (bst == kBelt || bst == kTunnelOut || bst == kTunnelIn) return;

  switch (getConveyorDirection(bst, _building->m_dir, 1)) {
    case N: _building->m_next[1] = above; _building->m_nextDir[1] = SN; break;
    case E: _building->m_next[1] = right; _building->m_nextDir[1] = WE; break;
    case S: _building->m_next[1] = below; _building->m_nextDir[1] = NS; break;
    case W: _building->m_next[1] = left;  _building->m_nextDir[1] = EW; break;
  }

  if (bst == kSplitI || bst == kFilterI || bst == kSplitL || bst == kFilterL ) return;

  switch (getConveyorDirection(bst, _building->m_dir, 2)) {
    case N: _building->m_next[2] = above; _building->m_nextDir[2] = SN; break;
    case E: _building->m_next[2] = right; _building->m_nextDir[2] = WE; break;
    case S: _building->m_next[2] = below; _building->m_nextDir[2] = NS; break;
    case W: _building->m_next[2] = left;  _building->m_nextDir[2] = EW; break;
  }

}

int8_t getConveyorDirection(enum kConvSubType _subType, enum kDir _dir, uint8_t _progress ) {
  if (_subType == kBelt || _subType == kTunnelOut || _subType == kTunnelIn) {
    switch (_dir) {
      case SN:; return N;
      case NS:; return S;
      case WE:; return E;
      case EW:; return W;
      case kDirN:; return 4;
    }
  } else if (_subType == kSplitI || _subType == kFilterI) {
    switch (_dir) {
      case WE:; if (_progress == 0) { return N; } else { return S; }
      case SN:; if (_progress == 0) { return W; } else { return E; }
      case EW:; if (_progress == 0) { return S; } else { return N; }
      case NS:; if (_progress == 0) { return E; } else { return W; }
      case kDirN:; return 4;
    }
  } else if (_subType == kSplitL || _subType == kFilterL) {
    switch (_dir) {
      case SN:; if (_progress == 0) { return N; } else { return E; }
      case WE:; if (_progress == 0) { return E; } else { return S; }
      case NS:; if (_progress == 0) { return S; } else { return W; }
      case EW:; if (_progress == 0) { return W; } else { return N; }
      case kDirN:; return 4;
    }
  } else if (_subType == kSplitT) {
    switch (_dir) {
      case SN:; if (_progress == 0) { return W; } else if (_progress == 1) { return N; } else { return E; }
      case WE:; if (_progress == 0) { return N; } else if (_progress == 1) { return E; } else { return S; }
      case NS:; if (_progress == 0) { return E; } else if (_progress == 1) { return S; } else { return W; }
      case EW:; if (_progress == 0) { return S; } else if (_progress == 1) { return W; } else { return N; }
      case kDirN:; return 4;
    }
  }
  return 4;
}


void upgradeConveyor(struct Building_t* _building) {
  if (!_building) return;
  if (_building->m_type != kConveyor) return;
  if (_building->m_stored[0] == 2) return; // Already upgraded
  if (!getOwned(kUICatUtility, kConveyorGrease)) return;

  modOwned(kUICatUtility, kConveyorGrease, /*add*/ false);
  _building->m_stored[0] = 2;
  pd->sprite->setImage(_building->m_sprite[2], getConveyorMaster(_building->m_dir, 2), kBitmapUnflipped);
  renderChunkBackgroundImage(_building->m_location->m_chunk);
}

void buildingSetupConveyor(struct Building_t* _building) {

  // Set the starting speed (but don't downgrade)
  if (_building->m_stored[0] == 0) _building->m_stored[0] = 1;
  enum kConvSubType cst = _building->m_subType.conveyor;

  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    _building->m_image[zoom] = getSprite16( CDesc[cst].spriteX, CDesc[cst].spriteY + _building->m_dir, zoom);
    /*
    switch (_building->m_subType.conveyor) {
      case kBelt:;     _building->m_image[zoom] = getSprite16(0,  CONV_START_Y + _building->m_dir, zoom); break;
      case kSplitI:;   _building->m_image[zoom] = getSprite16(0,  11 + _building->m_dir, zoom); break;
      case kSplitL:;   _building->m_image[zoom] = getSprite16(1,  11 + _building->m_dir, zoom); break;
      case kSplitT:;   _building->m_image[zoom] = getSprite16(2,  11 + _building->m_dir, zoom); break;
      case kFilterI:;  _building->m_image[zoom] = getSprite16(10, 11 + _building->m_dir, zoom); break;
      case kFilterL:;  _building->m_image[zoom] = getSprite16(3,  11 + _building->m_dir, zoom); break;
      case kTunnelIn:; _building->m_image[zoom] = getSprite16(8,  11 + _building->m_dir, zoom); break;
      case kTunnelOut:;_building->m_image[zoom] = getSprite16(9,  11 + _building->m_dir, zoom); break;
      case kNConvSubTypes:; break;
    }
    */

    // Animate the belt at zoom=2
    if (cst == kBelt && zoom == 2) {
      PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
      if (_building->m_sprite[zoom] == NULL) {
        _building->m_sprite[zoom] = pd->sprite->newSprite();
      }
      pd->sprite->setBounds(_building->m_sprite[zoom], bound);
      pd->sprite->setImage(_building->m_sprite[zoom], getConveyorMaster(_building->m_dir, _building->m_stored[0]), kBitmapUnflipped);
      pd->sprite->moveTo(_building->m_sprite[zoom], 
        (_building->m_pix_x + _building->m_location->m_pix_off_x) * zoom, 
        (_building->m_pix_y + _building->m_location->m_pix_off_y) * zoom);
      pd->sprite->setZIndex(_building->m_sprite[zoom], Z_INDEX_CONVEYOR);
    } else {
      if (_building->m_sprite[zoom] != NULL) {
        pd->sprite->freeSprite(_building->m_sprite[zoom]);
        _building->m_sprite[zoom] = NULL;
      }
    }
  }
}

const char* getTransitText(int8_t _d) {
  switch (_d) {
    case N: return "North";
    case E: return "East";
    case S: return "South";
    case W: return "West";
  }
  return "????";
}

void drawUIInspectConveyor(struct Building_t* _building) {
  static char text[128];
  uint8_t y = 1;
  snprintf(text, 128, "%s (%s)", 
    toStringBuilding(_building->m_type, _building->m_subType, false), 
    getRotationAsString(kUICatConv, _building->m_subType.conveyor, _building->m_dir) );
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  snprintf(text, 128, "Speed Multiplier: %s", _building->m_stored[0] == 1 ? "x1" : "x2");
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  if (_building->m_subType.conveyor >= kFilterL) {
    if (_building->m_mode.mode16 == kNoCargo) {
      snprintf(text, 128, "Filter Not Yet Set");
      pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    } else {
      snprintf(text, 128, "Filters On:      %s", toStringCargoByType(_building->m_mode.mode16, /*plural=*/true));
      pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
      pd->graphics->setDrawMode(kDrawModeCopy);
      pd->graphics->drawBitmap(getSprite16_byidx( CargoDesc[ _building->m_mode.mode16 ].UIIcon, 1), TILE_PIX*6 + 4, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
    }
  } else {
    snprintf(text, 128, "Next Transit: %s", getTransitText( getConveyorDirection(_building->m_subType.conveyor, _building->m_dir, _building->m_mode.mode16) ));
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  }
}