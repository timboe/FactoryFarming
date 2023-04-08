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

bool recursiveGetIsAbleToMove(struct Location_t* _nextLoc, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

bool anyOutputInDir(struct Location_t* _loc, enum kDir _dir);

#define CONV_SPEED 0
#define CONV_X 1
#define CONV_Y 2
#define CONV_HIDE 3

/// ///

bool anyOutputInDir(struct Location_t* _loc, enum kDir _dir) {
  struct Building_t* _building = _loc->m_building;
  if (!_building) return false;
  if (_building->m_type == kConveyor) {
    if (_building->m_subType.conveyor == kTunnelIn) return false;
    for (uint8_t i = 0; i < 4; ++i) {
      if (_building->m_nextDir[i] == _dir) {
        return true;
      }
    }
  } else if (producesOutputCargoOnAdjacentTile(_building->m_type, _building->m_subType)) {
    if (isLargeBuilding(_building->m_type, _building->m_subType)) {
      // We have been given one of 8 non-owned tiles surrounding the building.
      // We need to check that going from the building centere in the given direction _dir gives us _loc
      // if we are to consider this tile any further
      struct Location_t* buildingCentreLoc = _building->m_location;
      struct Location_t* walkOne = NULL;
      switch (_dir) {
        case SN: walkOne = getLocation(buildingCentreLoc->m_x, buildingCentreLoc->m_y - 1); break;
        case NS: walkOne = getLocation(buildingCentreLoc->m_x, buildingCentreLoc->m_y + 1); break;
        case WE: walkOne = getLocation(buildingCentreLoc->m_x + 1, buildingCentreLoc->m_y); break;
        case EW: walkOne = getLocation(buildingCentreLoc->m_x - 1, buildingCentreLoc->m_y); break;
        default: return false;
      }
      if (walkOne != _loc) {
        return false; // Not the tile which acts like an incoming conveyor from this large building
      }
    }
    if (_building->m_dir == _dir) {
      return true;
    }
  }
  return false;
}

void conveyorUpdateSprite(struct Building_t* _building) {
  // Update sprites of "corner" pieces
  if (_building->m_type != kConveyor) return;
  if (_building->m_subType.conveyor != kBelt) return;

  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, 1, &above, &below, &left, &right);

  // we require exactly one input to this tile
  bool specialCornerArrangement = true;
  uint8_t nIn = 0;
  struct Location_t* pointsToMe = NULL;
  if (anyOutputInDir(below, SN)) {
    ++nIn;
    // pd->system->logToConsole("BelowIn");
    pointsToMe = below;
    // No corner tile if I output in a stright line (or send the cargo straight back, which would be very stange)
    if (_building->m_nextDir[0] == SN || _building->m_nextDir[0] == NS) {
      // pd->system->logToConsole("\\-> veto");
      specialCornerArrangement = false;
    }
  } 
  if (anyOutputInDir(left, WE)) {
    ++nIn;
    // pd->system->logToConsole("LeftIn");
    pointsToMe = left;
    if (_building->m_nextDir[0] == WE || _building->m_nextDir[0] == EW) {
      // pd->system->logToConsole("\\-> veto");
      specialCornerArrangement = false;
    }
  }
  if (anyOutputInDir(above, NS)) {
    // pd->system->logToConsole("AboveIn");
    ++nIn;
    pointsToMe = above;
    if (_building->m_nextDir[0] == NS || _building->m_nextDir[0] == SN) {
      // pd->system->logToConsole("\\-> veto");
      specialCornerArrangement = false;
    }
  } 
  if (anyOutputInDir(right, EW)) {
    // pd->system->logToConsole("RightIn");
    ++nIn;
    pointsToMe = right;
    if (_building->m_nextDir[0] == EW || _building->m_nextDir[0] == WE) {
      // pd->system->logToConsole("\\-> veto");
      specialCornerArrangement = false;
    }
  } 

  // No corner tile if more than one tile output onto this tile (or if none do)
  if (nIn != 1) {
    // pd->system->logToConsole("nIn != 1, veto");
    specialCornerArrangement = false;
  }

  if (specialCornerArrangement) { 
    bool evenIn = false;
    // Check which way I point
    switch (_building->m_nextDir[0]) {
      case SN: evenIn = (left  == pointsToMe); break;
      case NS: evenIn = (right == pointsToMe); break;
      case WE: evenIn = (above == pointsToMe); break;
      case EW: evenIn = (below == pointsToMe); break;
      case kDirN: break;
    }
    uint16_t sID = 0;
    switch (_building->m_nextDir[0]) {
      case EW: sID = evenIn ? SID(8,3)  : SID(13,3); break;
      case WE: sID = evenIn ? SID(12,3) : SID(9,3);  break;
      case NS: sID = evenIn ? SID(14,3) : SID(11,3); break;
      case SN: sID = evenIn ? SID(10,3) : SID(15,3); break;
      case kDirN: break;
    }
    // pd->system->logToConsole("even:%i sID:%i", evenIn, sID);
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      _building->m_image[zoom] = getSprite16_byidx(sID, zoom);
    }
  } else {
    for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
      _building->m_image[zoom] = getSprite16(CDesc[kBelt].spriteX, CDesc[kBelt].spriteY + _building->m_dir, zoom);
    }
  }

}

void checkConveyorSpritesAroundLoc(struct Location_t* _loc) {
  if (_loc->m_building && _loc->m_building->m_type == kConveyor) conveyorUpdateSprite(_loc->m_building);
  //
  int8_t distance = 1;
  if (_loc->m_building && (_loc->m_building->m_type == kFactory || _loc->m_building->m_type == kExtractor)) {
    distance = 2;
  }
  // pd->system->logToConsole("d is %i", distance);
  //
  struct Location_t* l = NULL;
  l = getLocation(_loc->m_x + distance, _loc->m_y);
  if (l->m_building && l->m_building->m_type == kConveyor) {  
    // pd->system->logToConsole("A");
    conveyorUpdateSprite(l->m_building);
  }
  //
  l = getLocation(_loc->m_x - distance, _loc->m_y);
  if (l->m_building && l->m_building->m_type == kConveyor) {
    // pd->system->logToConsole("B");
    conveyorUpdateSprite(l->m_building);
  }
  //
  l = getLocation(_loc->m_x, _loc->m_y + distance);
  if (l->m_building && l->m_building->m_type == kConveyor) { 
    // pd->system->logToConsole("C"); 
    conveyorUpdateSprite(l->m_building);
  }
  //
  l = getLocation(_loc->m_x, _loc->m_y - distance);
  if (l->m_building && l->m_building->m_type == kConveyor) {
    // pd->system->logToConsole("D");
    conveyorUpdateSprite(l->m_building);
  }
}



void conveyorLocationUpdate(struct Building_t* _building, uint8_t _zoom) {
  const int8_t x = (int8_t) _building->m_stored[CONV_X];
  const int8_t y = (int8_t) _building->m_stored[CONV_Y];
  struct Location_t* loc = _building->m_location;
  if (_building->m_stored[CONV_HIDE]) {
    pd->sprite->moveTo(loc->m_cargo->m_sprite[_zoom], 32768, 32768);
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

bool recursiveGetIsAbleToMove(struct Location_t* _nextLoc, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {
  if (_nextLoc->m_cargo != NULL) {
    // Cascade the call
    struct Building_t* nextBuilding = _nextLoc->m_building;
    if (nextBuilding == NULL || m_recursionCount >= MAX_RECURSION || _nextLoc->m_chunk->m_isNearTick != (_tickLength == NEAR_TICK_AMOUNT)) {
      return false;
    } else {
      // Cuation: When calling into other conveyors this is a potentially recursive fn!
      (*nextBuilding->m_updateFn)(nextBuilding, _tickLength, _tickID, _zoom);
      // We might have been able to move the cargo on/into a building, check again
      return (_nextLoc->m_cargo == NULL);
    }
  }
  return true;
}


void conveyorUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo == NULL || _building->m_tickProcessed == _tickID) {
    return;
  }
  _building->m_tickProcessed = _tickID;
  // Most important place to track this
  ++m_recursionCount;

  #ifdef ONLY_SLOW_TICKS
  const bool nearTick = true;
  #else
  const bool nearTick = (_tickLength == NEAR_TICK_AMOUNT);
  #endif

  const enum kConvSubType bst = _building->m_subType.conveyor;

  
  // if (_building->m_subType.conveyor == kTunnelIn) _tick /= 2; // As travelling two tiles. But tricky, near tick amount is 1...

  if (_building->m_progress < TILE_PIX) {
    uint8_t newProgress = _building->m_progress + (_tickLength * _building->m_stored[0]); // Stored[0] used to hold conveyor speed  
    _building->m_progress = newProgress < TILE_PIX ? newProgress : TILE_PIX;

    // Handle filters vs. splitters
    enum kDir direction;
    if (bst >= kFilterL) {
      // First encounter with an object?
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
    if (bst >= kFilterL) {
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

  if (_building->m_progress < TILE_PIX) return;

  // Handle filters vs. splitters
  struct Location_t* nextLoc = NULL;
  if (bst >= kFilterL) {
    nextLoc = (_building->m_mode.mode16 == loc->m_cargo->m_type ? _building->m_next[1] :  _building->m_next[0]);  
  } else {
    nextLoc = _building->m_next[_building->m_mode.mode16];
  }

  bool ableToMove = recursiveGetIsAbleToMove(nextLoc, _tickLength, _tickID, _zoom);

  // Handle overflow
  if (!ableToMove && (bst == kOverflowI || bst == kOverflowL)) {
    // Try again 
    nextLoc = _building->m_next[1];
    ableToMove = recursiveGetIsAbleToMove(nextLoc, _tickLength, _tickID, _zoom);
  }
 
  if (!ableToMove) return;

  // MOVE CARGO
  struct Building_t* nextBuilding = nextLoc->m_building;
  struct Cargo_t* theCargo = loc->m_cargo;

  // Move cargo
  nextLoc->m_cargo = theCargo;
  loc->m_cargo = NULL;

  // Carry over any excess ticks (?)
  if (nextBuilding && nextBuilding->m_type == kConveyor) {
    // Don't process this tile again this tick
    nextBuilding->m_tickProcessed = _tickID;

    //nextBuilding->m_progress = _building->m_progress - TILE_PIX;
    nextBuilding->m_progress = 0;

    // Note: The direction does not matter here as we are the 1st tick into the new tile 
    // if we are actually looking at the sprite and hence calling conveyorLocationUpdate
    // hence we'll be drawn in the centre of the tile for any of the 4 possible directions
    conveyorSetLocation(nextBuilding, NS, nearTick, _zoom);

    // No longer doing conveyor update on IN, doing it on OUT instead (see below)
    //updateConveyorDirection(nextBuilding);

  } else {
    // Dump the cargo in the centre of the tile
    pd->sprite->moveTo(theCargo->m_sprite[_zoom], // TODO pass down zoom
                      (nextLoc->m_x*TILE_PIX + nextLoc->m_pix_off_x + TILE_PIX/2)* _zoom, 
                      (nextLoc->m_y*TILE_PIX + nextLoc->m_pix_off_y + TILE_PIX/2)* _zoom);
    pd->sprite->setZIndex(theCargo->m_sprite[1], Z_INDEX_CARGO_FLOOR);
    pd->sprite->setZIndex(theCargo->m_sprite[2], Z_INDEX_CARGO_FLOOR);
  }

  // Cargo moves between chunks?
  if (nextLoc->m_chunk != loc->m_chunk) {
    chunkRemoveCargo(loc->m_chunk, theCargo);
    chunkAddCargo(nextLoc->m_chunk, theCargo);
  }

  updateConveyorDirection(_building, _tickLength, _tickID, _zoom);
}

void updateConveyorDirection(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {
  // Update conveyor direction
  // Cycle outputs variant 2 (keep this one, it is nicer on the player)
  int16_t proposedNext = -1;
  switch (_building->m_subType.conveyor) {
    case kSplitI:; case kSplitL:; proposedNext = (_building->m_mode.mode16 + 1) % 2; break;
    case kSplitT:; proposedNext = (_building->m_mode.mode16 + 1) % 3; break;
    default:; // NOTE: We catch kFilterX etc. here and do NOT update m_mode for filters, regular belts, or overflows
  }

  if (proposedNext == -1) return;

  // Update nextLoc to the _proposed_ next location for this splitter
  struct Location_t* nextLoc = _building->m_next[proposedNext];

  const bool ableToSwitch = recursiveGetIsAbleToMove(nextLoc, _tickLength, _tickID, _zoom);;

  if (ableToSwitch) _building->m_mode.mode16 = proposedNext;
}


bool canBePlacedConveyor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  bool floor = false;
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile < TOT_FLOOR_TILES_INC_PAVED) floor = true;

  if (_subType.conveyor == kTunnelIn) {
    t = getTunnelOutTile(_loc, _dir);
    floor &= (t->m_tile < TOT_FLOOR_TILES_INC_PAVED);
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

  for (uint8_t i = 0; i < 4; ++i) {
    _building->m_nextDir[i] = kDirN;
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

  if (bst == kSplitI || bst == kFilterI || bst == kSplitL || bst == kFilterL || bst == kOverflowI || bst == kOverflowL) return;

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
  } else if (_subType == kSplitI || _subType == kFilterI || _subType == kOverflowI) {
    switch (_dir) {
      case WE:; if (_progress == 0) { return N; } else { return S; }
      case SN:; if (_progress == 0) { return W; } else { return E; }
      case EW:; if (_progress == 0) { return S; } else { return N; }
      case NS:; if (_progress == 0) { return E; } else { return W; }
      case kDirN:; return 4;
    }
  } else if (_subType == kSplitL || _subType == kFilterL || _subType == kOverflowL) {
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
    case N: return tr(kTRConvTransitN);
    case E: return tr(kTRConvTransitE);
    case S: return tr(kTRConvTransitS);
    case W: return tr(kTRConvTransitW);
  }
  return "????";
}

void drawUIInspectConveyor(struct Building_t* _building) {
  static char text[256];
  uint8_t y = 1;
  strcpy(text, toStringBuilding(_building->m_type, _building->m_subType, false));
  strcat(text, space());
  strcat(text, lb());
  strcat(text, getRotationAsString(kUICatConv, _building->m_subType.conveyor, _building->m_dir) );
  strcat(text, rb());

  //snprintf(text, 256, "%s (%s)", 
  //  toStringBuilding(_building->m_type, _building->m_subType, false), 
  //  getRotationAsString(kUICatConv, _building->m_subType.conveyor, _building->m_dir) );
  pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());

  strcpy(text, tr(kTRConvSpeed));
  strcat(text, cspace());
  strcat(text, _building->m_stored[0] == 1 ? "x1" : "x2");
  //snprintf(text, 128, tr(kTRConvSpeed), _building->m_stored[0] == 1 ? "x1" : "x2");

  pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  if (_building->m_subType.conveyor >= kFilterL) {
    if (_building->m_mode.mode16 == kNoCargo) {
      snprintf(text, 256, "%s", tr(kTRConvNoFilter0));
      pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
      snprintf(text, 256, "%s", tr(kTRConvNoFilter1));
      pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
    } else {

      strcpy(text, tr(kTRConvFilter));
      strcat(text, c5space());
      strcat(text, toStringCargoByType(_building->m_mode.mode16, /*plural=*/true));
      //snprintf(text, 256, tr(kTRConvFilter), toStringCargoByType(_building->m_mode.mode16, /*plural=*/true));
      pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
      pd->graphics->setDrawMode(kDrawModeCopy);
      pd->graphics->drawBitmap(getSprite16_byidx( CargoDesc[ _building->m_mode.mode16 ].UIIcon, 1), TILE_PIX*2 + trLen(kTRConvFilter), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
    }
  } else {
    strcpy(text, tr(kTRConvNextTransit));
    strcat(text, cspace());
    strcat(text, getTransitText( getConveyorDirection(_building->m_subType.conveyor, _building->m_dir, _building->m_mode.mode16) ));
    //snprintf(text, 256, 
    //  tr(kTRConvNextTransit),
    //  getTransitText( getConveyorDirection(_building->m_subType.conveyor, _building->m_dir, _building->m_mode.mode16) ));
    pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
    pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  }
}