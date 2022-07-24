#include "extractor.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"


void cropHarveserUpdateFn(struct Building_t* _building, uint8_t _tick);

void mineUpdateFn(struct Building_t* _building, uint8_t _tick);


/// ///

#define COLLECT_TIME (TICKS_PER_SEC*8)

void tryPickupAnyCargo(struct Location_t* _from, struct Building_t* _building) {
  for (int32_t try = 0; try < (MAX_STORE/2); ++try) {
    // Do we already store of this type? Or do we store nothing in this type?
    if (_building->m_stored[try + (MAX_STORE/2)] == kNoCargo || _building->m_stored[try + (MAX_STORE/2)] == _from->m_cargo->m_type) {
      _building->m_stored[try + (MAX_STORE/2)] = _from->m_cargo->m_type;
      if (_building->m_stored[try] < 255) {
        ++_building->m_stored[try]; 
        clearLocation(_from, /*clearCargo*/ true, /*clearBuilding*/ false);
      }
      break;
    }
  }
}

void tryPutdownAnyCargo(struct Building_t* _building, uint8_t _tick) {
  if (_building->m_next[0]->m_cargo == NULL) {
    for (int32_t try = 0; try < (MAX_STORE/2); ++try) {
      if (_building->m_stored[try]) {
        newCargo(_building->m_next[0], _building->m_stored[try + (MAX_STORE/2)], _tick == NEAR_TICK_AMOUNT);
        if (--_building->m_stored[try] == 0) {
          _building->m_stored[try + 3] = kNoCargo; // Reset the slot if we have run out of items 
        }
        break;
      }
    }
  }
}

void cropHarveserUpdateFn(struct Building_t* _building, uint8_t _tick) {
  // Placing down
  tryPutdownAnyCargo(_building, _tick);

  _building->m_progress -= _tick;
  if (_building->m_progress > 0) return;
  _building->m_progress = COLLECT_TIME;

  int32_t min, max;
  if (_building->m_subType.extractor == kCropHarvesterLarge) {
    min = -4;
    max = 5;
  } else {
    min = -3; 
    max = 4;
  }

  // Picking up
  for (int32_t x = min; x < max; ++x) {
    for (int32_t y = min; y < max; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_cargo && (loc->m_building == NULL || loc->m_building->m_type == kPlant)) {
        // Into  our three storage locations
        tryPickupAnyCargo(loc, _building);
      }
    }
  }
}


void mineUpdateFn(struct Building_t* _building, uint8_t _tick) {
  _building->m_progress += _tick;
  if (_building->m_progress >= MAX_DROP_RATE) {
    _building->m_progress = 0;
    if (_building->m_next[0]->m_cargo == NULL) {
      newCargo(_building->m_next[0], EDesc[_building->m_subType.extractor].out, _tick == NEAR_TICK_AMOUNT);
    }
  }
}

void extractorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  switch (_building->m_subType.extractor) {
    case kCropHarvesterSmall:; case kCropHarvesterLarge:; return cropHarveserUpdateFn(_building, _tick);
    case kChalkQuarry:; case kPump:; case kSaltMine:; return mineUpdateFn(_building, _tick);
    case kNExtractorSubTypes:; break;
  }
}

bool canBePlacedExtractor(struct Location_t* _loc, union kSubType _subType) {
  bool hasWater = false;
  bool hasChalk = false;
  bool hasPeat = false;
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Tile_t* t = getTile(_loc->m_x + x, _loc->m_y + y);
      bool isWater = isWaterTile(_loc->m_x + x, _loc->m_y + y);
      bool isChalk = isGroundTypeTile(_loc->m_x + x, _loc->m_y + y, kChalkyGround);
      bool isPeat = isGroundTypeTile(_loc->m_x + x, _loc->m_y + y, kPeatyGround);
      if (isWater) hasWater = true;
      if (isChalk) hasChalk = true;
      if (isPeat) hasPeat = true;
      if (t->m_tile > TOT_FLOOR_TILES && !isWater) return false;
      if (getLocation(_loc->m_x + x, _loc->m_y + y)->m_building != NULL) return false;
    }
  }
  if (_subType.extractor == kPump) {
    return hasWater;
  } else if (_subType.extractor == kChalkQuarry) {
    return hasChalk && !hasWater;
  } else if (_subType.extractor == kSaltMine) {
    return hasPeat && !hasWater;
  }
 return !hasWater;
}

void assignNeighborsExtractor(struct Building_t* _building) {
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, 2, &above, &below, &left, &right);
  switch (_building->m_dir) {
    case SN:; _building->m_next[0] = above; break;
    case WE:; _building->m_next[0] = right; break;
    case NS:; _building->m_next[0] = below; break;
    case EW:; _building->m_next[0] = left; break;
    case kDirN:; break;
  }
}

void buildingSetupExtractor(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    _building->m_image[zoom] = getSprite48_byidx( EDesc[_building->m_subType.extractor].sprite + _building->m_dir, zoom); 

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

  _building->m_progress = COLLECT_TIME;
}

void drawUIInspectExtractor(struct Building_t* _building) {
  const enum kExtractorSubType est = _building->m_subType.extractor;

  static char text[128];
  uint8_t y = 1;
  snprintf(text, 128, "%s", toStringBuilding(_building->m_type, _building->m_subType, false));
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  if (est == kCropHarvesterSmall || est == kCropHarvesterLarge) {

    for (int32_t hopper = 0; hopper < 3; ++hopper) {
      if (!_building->m_stored[hopper]) continue;

      snprintf(text, 128, "Hopper %i/3:       %s (%i)", 
        (int)hopper+1, 
        toStringCargoByType( _building->m_stored[(MAX_STORE/2) + hopper] ), 
        _building->m_stored[hopper]);
      pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
      pd->graphics->setDrawMode(kDrawModeCopy);
      pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ _building->m_stored[(MAX_STORE/2) + hopper] ].UIIcon, 1), TILE_PIX*7 + TILE_PIX/4, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
      pd->graphics->setDrawMode(kDrawModeFillBlack);
    }

  } else {

    snprintf(text, 128, "Out:       %s", toStringCargoByType( EDesc[est].out ));
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ EDesc[est].out ].UIIcon, 1), TILE_PIX*4, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
    pd->graphics->setDrawMode(kDrawModeFillBlack);

  }
}