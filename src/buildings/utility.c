#include "utility.h"
#include "extractor.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../ui.h"
#include "../io.h"
#include "../chunk.h"
#include "../sound.h"

struct Building_t* m_retirement = NULL;

void binUpdateFn(struct Building_t* _building);

void storageUpdateFn(struct Building_t* _building);

void bufferUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void buildingSetupRetirement(struct Building_t* _building);

void rotavatorCheckSet(struct Tile_t* _t, uint8_t* _stored);

void rotavatorCheckRevert(struct Tile_t* _t, uint8_t _stored);

/// ///

void storageUpdateFn(struct Building_t* _building) {

  // Pickup
  if (_building->m_location->m_cargo) {
    tryPickupAnyCargo(_building->m_location, _building);
  }

}

void bufferUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {

  // Pickup
  if (_building->m_location->m_cargo) {
    tryPickupAnyCargo(_building->m_location, _building);
  }

  // Putdown
  _building->m_progress += _tickLength;
  if (_building->m_progress >= MAX_DROP_RATE) {
    _building->m_progress = 0;
    tryPutdownAnyCargo(_building, _tickLength, _tickID, _zoom);
  }

}

void binUpdateFn(struct Building_t* _building) {
  if (_building->m_location->m_cargo) {
    clearLocation(_building->m_location, /*clearCargo*/ true, /*clearBuilding*/ false);
  }
}

void utilityUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {
  if (_building->m_tickProcessed == _tickID) return;
  _building->m_tickProcessed = _tickID;
  ++m_recursionCount;

  if (_building->m_subType.utility == kBin) {
  	return binUpdateFn(_building);
  } else if (_building->m_subType.utility == kStorageBox) {
    return storageUpdateFn(_building);
  } else if (_building->m_subType.utility == kBuffferBox) {
    return bufferUpdateFn(_building, _tickLength, _tickID, _zoom);
  }
}

void rotavatorCheckSet(struct Tile_t* _t, uint8_t* _stored) {
  if (!isSoilTile(_t) || isGroundTypeTile_ptr(_t, kLoamyGround)) {
    *_stored = 255;
  } else {
    *_stored = _t->m_tile;
    setTile_ptr(_t, (FLOOR_VARIETIES * kLoamyGround) + rand() % FLOOR_VARIETIES);
  }
}

void buildRotavator(struct Location_t* _loc) {
    rotavatorCheckSet(getTile(_loc->m_x - 1, _loc->m_y + 1), &_loc->m_building->m_stored[0]);
    rotavatorCheckSet(getTile(_loc->m_x    , _loc->m_y + 1), &_loc->m_building->m_stored[1]);
    rotavatorCheckSet(getTile(_loc->m_x + 1, _loc->m_y + 1), &_loc->m_building->m_stored[2]);
    rotavatorCheckSet(getTile(_loc->m_x - 1, _loc->m_y    ), &_loc->m_building->m_stored[3]);
    rotavatorCheckSet(getTile(_loc->m_x + 1, _loc->m_y    ), &_loc->m_building->m_stored[4]);
    rotavatorCheckSet(getTile(_loc->m_x - 1, _loc->m_y - 1), &_loc->m_building->m_stored[5]);
    rotavatorCheckSet(getTile(_loc->m_x    , _loc->m_y - 1), &_loc->m_building->m_mode.mode8[0]);
    rotavatorCheckSet(getTile(_loc->m_x + 1, _loc->m_y - 1), &_loc->m_building->m_mode.mode8[1]);
}

void rotavatorCheckRevert(struct Tile_t* _t, uint8_t _stored) {
  if (_stored != 255) {
    setTile_ptr(_t, _stored);
  }
}

void destroyRotavator(struct Location_t* _loc) {
    rotavatorCheckRevert(getTile(_loc->m_x - 1, _loc->m_y + 1), _loc->m_building->m_stored[0]);
    rotavatorCheckRevert(getTile(_loc->m_x    , _loc->m_y + 1), _loc->m_building->m_stored[1]);
    rotavatorCheckRevert(getTile(_loc->m_x + 1, _loc->m_y + 1), _loc->m_building->m_stored[2]);
    rotavatorCheckRevert(getTile(_loc->m_x - 1, _loc->m_y    ), _loc->m_building->m_stored[3]);
    rotavatorCheckRevert(getTile(_loc->m_x + 1, _loc->m_y    ), _loc->m_building->m_stored[4]);
    rotavatorCheckRevert(getTile(_loc->m_x - 1, _loc->m_y - 1), _loc->m_building->m_stored[5]);
    rotavatorCheckRevert(getTile(_loc->m_x    , _loc->m_y - 1), _loc->m_building->m_mode.mode8[0]);
    rotavatorCheckRevert(getTile(_loc->m_x + 1, _loc->m_y - 1), _loc->m_building->m_mode.mode8[1]);
}

bool doPlaceRetirement(struct Location_t* _loc) {
  if (!canBePlacedUtility(_loc, (union kSubType) {.utility = kRetirement})) {
    return false;
  }

  pauseMusic();

  // Cottage
  newBuilding(getLocation(_loc->m_x, _loc->m_y - 1), SN, kUtility, (union kSubType) {.utility = kRetirement} );

  // Path
  for (int32_t y = _loc->m_y; y < _loc->m_y + 5; ++y) {
    newBuilding(getLocation(_loc->m_x, y), SN, kUtility, (union kSubType) {.utility = kPath} );
  }

  // Fence
  for (int32_t y = _loc->m_y - 4; y < _loc->m_y + 5; ++y) {
    newBuilding(getLocation(_loc->m_x - 4, y), SN, kUtility, (union kSubType) {.utility = kFence} );
    newBuilding(getLocation(_loc->m_x + 4, y), SN, kUtility, (union kSubType) {.utility = kFence} );
  }

  for (int32_t x = _loc->m_x - 4; x < _loc->m_x + 5; ++x) {
    newBuilding(getLocation(x, _loc->m_y + 4), SN, kUtility, (union kSubType) {.utility = kFence} );
    newBuilding(getLocation(x, _loc->m_y - 4), SN, kUtility, (union kSubType) {.utility = kFence} );
  }

  // Garden
  for (int32_t x = _loc->m_x - 3; x < _loc->m_x - 1; ++x) {
    for (int32_t y = _loc->m_y + 2; y < _loc->m_y + 5; ++y) {
      newBuilding(getLocation(x, y), SN, kPlant, (union kSubType) {.plant = kRosePlant} );
    }
  }

  // Garden
  for (int32_t x = _loc->m_x + 2; x < _loc->m_x + 5; ++x) {
    for (int32_t y = _loc->m_y + 2; y < _loc->m_y + 5; ++y) {
      newBuilding(getLocation(x, y), SN, kPlant, (union kSubType) {.plant = kRosePlant} );
    }
  }

  // Garden
  for (int32_t y = _loc->m_y - 3; y < _loc->m_y + 1; ++y) {
    newBuilding(getLocation(_loc->m_x - 3, y), SN, kPlant, (union kSubType) {.plant = kStrawberryPlant} );
    newBuilding(getLocation(_loc->m_x + 3, y), SN, kPlant, (union kSubType) {.plant = kSunflowerPlant} );
  }

  resumeMusic();

  return true;
}

bool doPlaceLandfill(struct Location_t* _loc) {
  if (!canBePlacedUtility(_loc, (union kSubType) {.utility = kLandfill})) {
    return false;
  }
  // Update the tile, re-do wetness (like for a well, but this is irreversible)
  pauseMusic();
  setTile( getTile_idx(_loc->m_x, _loc->m_y), UDesc[kLandfill].sprite );
  doWetnessAroundLoc(_loc);
  renderChunkBackgroundImageAround(_loc->m_chunk);
  resumeMusic();
  return true;
}

bool canBePlacedUtility(struct Location_t* _loc, union kSubType _subType) {

  if (_subType.utility == kRetirement) {
    //TODO fix this bug
    if (_loc->m_x == 0 || _loc->m_y == 0) return false;

    if (getSlot() != kTranquilWorld) return false;
    for (int32_t x = -4; x < 5; ++x) {
      for (int32_t y = -4; y < 5; ++y) {
        struct Tile_t* t = getTile(_loc->m_x + x, _loc->m_y + y);
        if (t->m_tile >= TOT_FLOOR_TILES_INC_LANDFILL) return false;
        if (getLocation(_loc->m_x + x, _loc->m_y + y)->m_building != NULL) return false;
      }
    }
    return true;
  }

  if (_subType.utility == kConveyorGrease) return true; // A different system is used to apply this

  if (_subType.utility == kObstructionRemover) return true; // A different system is used to apply this

  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);

  if (_subType.utility == kLandfill) {
    if (isWaterTile(_loc->m_x, _loc->m_y)) {
      //noop
    } else if (t->m_tile >= TOT_FLOOR_TILES) { // Excluding landfill here
      return false;
    }
  } else {
    if (t->m_tile >= TOT_FLOOR_TILES_INC_LANDFILL) return false;
  }
  
  if (_loc->m_building != NULL) return false;
  return true;
}

void assignNeighborsUtility(struct Building_t* _building) {
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, 1, &above, &below, &left, &right);
  // This is only used by kBuffferBox
  switch (_building->m_dir) {
    case SN:; _building->m_next[0] = above; break;
    case WE:; _building->m_next[0] = right; break;
    case NS:; _building->m_next[0] = below; break;
    case EW:; _building->m_next[0] = left; break;
    case kDirN:; break;
  }
}

void buildingSetupUtility(struct Building_t* _building) {
  if (_building->m_subType.utility == kRetirement) {
    return buildingSetupRetirement(_building);
  }
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.utility) {
      case kBuffferBox:; _building->m_image[zoom] = getSprite16_byidx( UDesc[kBuffferBox].sprite + _building->m_dir, zoom); break;
      default: _building->m_image[zoom] = getSprite16_byidx( UDesc[_building->m_subType.utility].sprite, zoom); break;
    }

    if (_building->m_subType.utility == kFence) {
      PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
      //PDRect bound = {.x = ((TILE_PIX/4)-2)*zoom, .y = ((TILE_PIX/4)-2)*zoom, .width = ((TILE_PIX/2)+4)*zoom, .height = ((TILE_PIX/2)+4)*zoom};
      if (_building->m_sprite[zoom] == NULL) _building->m_sprite[zoom] = pd->sprite->newSprite();
      pd->sprite->setCollideRect(_building->m_sprite[zoom], bound);
      pd->sprite->moveTo(_building->m_sprite[zoom], 
        (_building->m_pix_x - TILE_PIX/2)*zoom, 
        (_building->m_pix_y - TILE_PIX/2)*zoom);
    }
  }
}

void buildingSetupRetirement(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    _building->m_image[zoom] = getSprite48_byidx( UDesc[_building->m_subType.extractor].sprite, zoom); 

    PDRect bound = {.x = (COLLISION_OFFSET_BIG/2)*zoom, .y = (COLLISION_OFFSET_BIG/2)*zoom, .width = (EXTRACTOR_PIX - COLLISION_OFFSET_BIG)*zoom, .height = (EXTRACTOR_PIX - COLLISION_OFFSET_BIG)*zoom};
    if (_building->m_sprite[zoom] == NULL) _building->m_sprite[zoom] = pd->sprite->newSprite();
    pd->sprite->setCollideRect(_building->m_sprite[zoom], bound);
    pd->sprite->moveTo(_building->m_sprite[zoom], 
      (_building->m_pix_x + _building->m_location->m_pix_off_x - EXTRACTOR_PIX/2)*zoom, 
      (_building->m_pix_y + _building->m_location->m_pix_off_y - EXTRACTOR_PIX/2)*zoom);
  }

  for (int32_t x = _building->m_location->m_x - 1; x < _building->m_location->m_x + 2; ++x) {
    for (int32_t y = _building->m_location->m_y - 1; y < _building->m_location->m_y + 2; ++y) {
      clearLocation(getLocation(x, y), /*cargo*/ true, /*building*/ false);
    }
  }

  m_retirement = _building;
}

int16_t distanceFromRetirement() {
  if (!m_retirement) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_retirement->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_retirement->m_pix_y);
  return (dx < dy ? dy : dx);
}

void drawUIInspectUtility(struct Building_t* _building) {
  const enum kUtilitySubType ust = _building->m_subType.utility;

  static char text[128];
  uint8_t y = 1;

  if (ust == kBuffferBox) {
    snprintf(text, 128, "%s (%s)", 
      toStringBuilding(_building->m_type, _building->m_subType, false), 
      getRotationAsString(kUICatConv, kBelt, _building->m_dir) );
  } else {
    snprintf(text, 128, "%s", toStringBuilding(_building->m_type, _building->m_subType, false));
  }
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  if (ust == kPath) {

    snprintf(text, 128, "Movement speed is enhanced on the path.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kFence) {

    snprintf(text, 128, "A charming fence.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kRetirement) {

    snprintf(text, 128, "Your cozy little retirement cottage.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  
  } else if (ust == kSign) {

    snprintf(text, 128, "Place a cargo here to display it on the sign.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

    if (_building->m_mode.mode16) {
      snprintf(text, 128, "Sign for: %s", toStringCargoByType( _building->m_mode.mode16, /*plural=*/true ) );
      pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    }

  } else if (ust == kBin) {

    snprintf(text, 128, "Permanently erases unwanted cargo.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Drop the unwanted cargo, or use conveyors");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kRotavator) {

    snprintf(text, 128, "Converts the 8 surrounding tiles to loamy soil.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Removing the rotavator will revert these tiles");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "to their previous soil type.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kFactoryUpgrade) {

    snprintf(text, 128, "Reduces production time by 0.5 seconds in any");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "factory immediately N, S, E or W of the module.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Minimum production time: 0.5 seconds.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kWell) {

    snprintf(text, 128, "Creates a surrounding area of wet and moist soil.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kBuffferBox || ust == kStorageBox) {

    for (int32_t compartment = 0; compartment < 3; ++compartment) {
      if (!_building->m_stored[compartment]) continue;

      snprintf(text, 128, "Compartment %i/3:       %s (%i)", 
        (int)compartment+1, 
        toStringCargoByType( _building->m_stored[(MAX_STORE/2) + compartment], /*plural=*/(_building->m_stored[compartment] != 1) ), 
        _building->m_stored[compartment]);
      pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
      pd->graphics->setDrawMode(kDrawModeCopy);
      pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ _building->m_stored[(MAX_STORE/2) + compartment] ].UIIcon, 1), TILE_PIX*10, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
      pd->graphics->setDrawMode(kDrawModeFillBlack);
    }

  }
}