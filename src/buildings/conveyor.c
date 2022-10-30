#include "conveyor.h"
#include "../chunk.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../ui.h"
#include "../sound.h"

int8_t getConveyorDirection(enum kConvSubType _subType, enum kDir _dir, uint8_t _progress );

const char* getTransitText(int8_t _d);

void conveyorSetLocation(struct Building_t* _building, enum kDir _direction, bool _nearTick, uint8_t _zoom);

void moveCargo(struct Location_t* _loc, struct Location_t* _nextLoc, uint8_t _tick, uint8_t _zoom);

int16_t doDesireToMoveA(uint8_t _tick, uint8_t _zoom);

int16_t doDesireToMoveB(uint8_t _tick, uint8_t _zoom);

uint8_t m_tickCounter = 0;

#define CONV_SPEED 0
#define CONV_X 1
#define CONV_Y 2
#define CONV_HIDE 3
#define CONV_MOVEDTO 4

#define MAX_DESIRES_TO_MOVE 2048

#define MOVE_CUTOFF_THRESHOLD 2
#define MAX_ITERATIONS 8

uint16_t m_nDesiresToMoveA;
uint16_t m_nDesiresToMoveB;
struct Location_t* m_desiresToMoveToA[MAX_DESIRES_TO_MOVE];
struct Location_t* m_desiresToMoveToB[MAX_DESIRES_TO_MOVE];

/// ///

void newConveyorTick() {
  ++m_tickCounter;
  m_nDesiresToMoveA = 0;
}

void regDesireToMove(struct Location_t* _from, struct Location_t* _to) {
  if (m_nDesiresToMoveA == MAX_DESIRES_TO_MOVE) {
    #ifdef DEV
    pd->system->error("More conveyor desires to move than we can support!");
    #endif
    return;
  }

  m_desiresToMoveToA[m_nDesiresToMoveA++] = _to;
  _to->m_desiredToMoveFrom = _from;
}

int16_t doDesireToMoveA(uint8_t _tick, uint8_t _zoom) {
  int16_t moved = 0;
  m_nDesiresToMoveB = 0;
  for (int16_t i = m_nDesiresToMoveA - 1; i >=0; --i) {
    if ( m_desiresToMoveToA[i]->m_cargo == NULL) {
      ++moved;
      moveCargo(/*from*/ m_desiresToMoveToA[i]->m_desiredToMoveFrom, /*to*/ m_desiresToMoveToA[i], _tick, _zoom);
    } else {
      m_desiresToMoveToB[m_nDesiresToMoveB++] = m_desiresToMoveToA[i];
    }
  }
  //pd->system->logToConsole("A Iteration: moved %i of %i", moved, m_nDesiresToMoveA);
  return moved;
}

int16_t doDesireToMoveB(uint8_t _tick, uint8_t _zoom) {
  int16_t moved = 0;
  m_nDesiresToMoveA = 0;
  for (int16_t i = 0; i < m_nDesiresToMoveB; ++i) {
    if ( m_desiresToMoveToB[i]->m_cargo == NULL) {
      ++moved;
      moveCargo(/*from*/ m_desiresToMoveToB[i]->m_desiredToMoveFrom, /*to*/ m_desiresToMoveToB[i], _tick, _zoom);
    } else {
      m_desiresToMoveToA[m_nDesiresToMoveA++] = m_desiresToMoveToB[i];
    }
  }
  //pd->system->logToConsole("B Iteration: moved %i of %i", moved, m_nDesiresToMoveB);
  return moved;
}

void processDesiresToMove(uint8_t _tick, uint8_t _zoom) {
  if (!m_nDesiresToMoveA) return;

  int16_t moved = INT16_MAX;
  int16_t iterations = 0;
  bool flipFlop = true;
  while (moved > MOVE_CUTOFF_THRESHOLD && iterations++ < MAX_ITERATIONS) {
    moved = flipFlop ? doDesireToMoveA(_tick, _zoom) : doDesireToMoveB(_tick, _zoom);
    flipFlop = !flipFlop;
  }
}

void conveyorLocationUpdate(struct Building_t* _building, uint8_t _zoom) {
  const int8_t x = (int8_t) _building->m_stored[CONV_X];
  const int8_t y = (int8_t) _building->m_stored[CONV_Y];
  struct Location_t* loc = _building->m_location;
  if (_building->m_stored[CONV_HIDE]) {
    pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], 65536, 65536);
  } else {
    pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], 
                      (_building->m_pix_x + loc->m_pix_off_x + x)*_zoom, 
                      (_building->m_pix_y + loc->m_pix_off_y + y)*_zoom);
  }
}

void conveyorSetLocation(struct Building_t* _building, enum kDir _direction, bool _nearTick, uint8_t _zoom) {
  _building->m_stored[CONV_HIDE] = (_building->m_subType.conveyor == kTunnelIn); // hide 
  switch (_direction) {
    case SN:; _building->m_stored[CONV_X] = 0; _building->m_stored[CONV_Y] = (uint8_t) -_building->m_progress; break;
    case NS:; _building->m_stored[CONV_X] = 0; _building->m_stored[CONV_Y] = _building->m_progress; break;
    case EW:; _building->m_stored[CONV_X] = (uint8_t) -_building->m_progress; _building->m_stored[CONV_Y] = 0; break;
    case WE:; _building->m_stored[CONV_X] = _building->m_progress; _building->m_stored[CONV_Y] = 0; break;
    case kDirN:; break;
  }
  // Only needs to be done if we are rendering
  if (_nearTick) {
    conveyorLocationUpdate(_building, _zoom);
  }
}


void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo == NULL || _building->m_stored[CONV_MOVEDTO] == m_tickCounter) return;
  #ifdef ONLY_SLOW_TICKS
  const bool nearTick = true;
  #else
  const bool nearTick = _tick == NEAR_TICK_AMOUNT;
  #endif
  
  // if (_building->m_subType.conveyor == kTunnelIn) _tick /= 2; // As travelling two tiles. But tricky, near tick amount is 1...
  if (_building->m_progress < TILE_PIX) {
    _building->m_progress += _tick * _building->m_stored[0]; // Stored[0] used to hold conveyor speed

    // Handle filters vs. splitters
    enum kDir direction;
    if (_building->m_subType.conveyor >= kFilterL) {
      // First encounter with an object? TODO can everything which can place an item on the chunk do this instead?
      if (_building->m_mode.mode16 == kNoCargo) {
        _building->m_mode.mode16 = loc->m_cargo->m_type; // Note: This CANNOT be undone without destroying the building
      }
      direction = (_building->m_mode.mode16 == loc->m_cargo->m_type ? _building->m_nextDir[1] :  _building->m_nextDir[0]);  
    } else {
      direction = _building->m_nextDir[_building->m_mode.mode16];
    }

    conveyorSetLocation(_building, direction, nearTick, _zoom);
  }
  //////////////////////////////////////////
  #ifdef ONLY_SLOW_TICKS
  else { 
    enum kDir direction;
    if (_building->m_subType.conveyor >= kFilterL) {
      if (_building->m_mode.mode16 == kNoCargo) {
        _building->m_mode.mode16 = _building->m_location->m_cargo->m_type;
      }
      direction = (_building->m_mode.mode16 == _building->m_location->m_cargo->m_type ? _building->m_nextDir[1] :  _building->m_nextDir[0]);  
    } else {
      direction = _building->m_nextDir[_building->m_mode.mode16];
    }
    conveyorSetLocation(_building, direction, nearTick, _zoom);
  }
  #endif
  //////////////////////////////////////////

  // Handle filters vs. splitters
  struct Location_t* nextLoc = NULL;
  if (_building->m_subType.conveyor >= kFilterL) {
    nextLoc = (_building->m_mode.mode16 == loc->m_cargo->m_type ? _building->m_next[1] :  _building->m_next[0]);  
  } else {
    nextLoc = _building->m_next[_building->m_mode.mode16];
  }

  if (_building->m_progress < TILE_PIX) return;

  if (nextLoc->m_cargo != NULL) {

    // Register desire to move
    regDesireToMove(/*from*/ loc, /*to*/ nextLoc);

  } else {

    moveCargo(loc, nextLoc, _tick, _zoom);
  
  }
}

void updateConveyorDirection(struct Building_t* _building) {
  // Cycle outputs variant 2 (keep this one, it is nicer on the player)
  uint16_t next = 0;
  switch (_building->m_subType.conveyor) {
    case kSplitI:; case kSplitL:; next = (_building->m_mode.mode16 + 1) % 2; break;
    case kSplitT:; next = (_building->m_mode.mode16 + 1) % 3; break;
    default: return; // NOTE: We catch kFilterX here and do NOT update m_mode for filters
  }
  if (_building->m_next[ next ]->m_cargo == NULL) _building->m_mode.mode16 = next;
}


void moveCargo(struct Location_t* _loc, struct Location_t* _nextLoc, uint8_t _tick, uint8_t _zoom) {

  struct Building_t* building = _loc->m_building;
  struct Building_t* nextBuilding = _nextLoc->m_building;
  struct Cargo_t* theCargo = _loc->m_cargo;

  // Move cargo
  _nextLoc->m_cargo = theCargo;
  _loc->m_cargo = NULL;

  // Carry over any excess ticks
  if (nextBuilding && nextBuilding->m_type == kConveyor) {
    nextBuilding->m_stored[CONV_MOVEDTO] = m_tickCounter;

    // Only need to comment out if non-conveyors start to register move orders
    //if (building && building->m_type == kConveyor) {
      nextBuilding->m_progress = building->m_progress - TILE_PIX;
    //}

    // Note: The direction does not matter here as we are the 1st tick into the new tile 
    // if we are actually looking at the sprite and hence calling conveyorLocationUpdate
    // hence we'll be drawn in the centre of the tile for any of the 4 possible directions
    conveyorSetLocation(nextBuilding, NS, _tick, _zoom);

    updateConveyorDirection(nextBuilding);

  } else {
    // Dump the cargo in the centre of the tile
    pd->sprite->moveTo(theCargo->m_sprite[_zoom], // TODO pass down zoom
                      (_nextLoc->m_x*TILE_PIX + _nextLoc->m_pix_off_x + TILE_PIX/2)* _zoom, 
                      (_nextLoc->m_y*TILE_PIX + _nextLoc->m_pix_off_y + TILE_PIX/2)* _zoom);
  }

  // Cargo moves between chunks?
  if (_nextLoc->m_chunk != _loc->m_chunk) {
    chunkRemoveCargo(_loc->m_chunk, theCargo);
    chunkAddCargo(_nextLoc->m_chunk, theCargo);
  }

}

bool canBePlacedConveyor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  bool floor = false;
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile < TOT_FLOOR_TILES_INC_PAVED) floor = true;
  //else if (t->m_tile >= SPRITE16_ID(8, 16) && t->m_tile < SPRITE16_ID(8+4, 16)) floor = true; // what was this for?  Paved?

  if (_subType.conveyor == kTunnelIn) {
    t = getTunnelOutTile(_loc, _dir);
    floor &= (t->m_tile < TOT_FLOOR_TILES_INC_PAVED);
    //else if (t->m_tile >= SPRITE16_ID(8, 16) && t->m_tile < SPRITE16_ID(8+4, 16)) floor &= true; // what was this for? Paved?
  }

  if (!floor) return false;

  // We can place a conveyor on top of an existing one
  bool existingGood = false;
  if (_loc->m_building == NULL || _loc->m_building->m_type == kConveyor) existingGood = true;

  if (_subType.conveyor == kTunnelIn) {
    struct Location_t* tunnelOut = getTunnelOutLocation(_loc, _dir);
    existingGood &= (tunnelOut->m_building == NULL || tunnelOut->m_building->m_type == kConveyor);
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


void upgradeConveyor(struct Building_t* _building, bool _forFree) {
  if (!_building) return;
  if (_building->m_type != kConveyor) return;
  if (_building->m_stored[0] == 2) return; // Already upgraded
  if (!_forFree && !getOwned(kUICatUtility, kConveyorGrease)) return;

  if (!_forFree) {
    modOwned(kUICatUtility, kConveyorGrease, /*add*/ false);
    sfx(kSfxPlaceUtil);
  }
  _building->m_stored[0] = 2;
  if (_building->m_subType.conveyor == kBelt) {
    pd->sprite->setImage(_building->m_sprite[2], getConveyorMaster(_building->m_dir, 2), kBitmapUnflipped);
  }
  renderChunkBackgroundImage(_building->m_location->m_chunk);
}

void buildingSetupConveyor(struct Building_t* _building) {

  // Set the starting speed (but don't downgrade)
  if (_building->m_stored[0] == 0) _building->m_stored[0] = 1;
  enum kConvSubType cst = _building->m_subType.conveyor;

  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    _building->m_image[zoom] = getSprite16( CDesc[cst].spriteX, CDesc[cst].spriteY + _building->m_dir, zoom);

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