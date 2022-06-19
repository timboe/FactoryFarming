#include "extractor.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"


void cropHarveserUpdateFn(struct Building_t* _building, uint8_t _tick);

/// ///

#define COLLECT_TIME (TICKS_PER_SEC*8)

void cropHarveserUpdateFn(struct Building_t* _building, uint8_t _tick) {
  // Placing down
  if (_building->m_next[0]->m_cargo == NULL) {
    for (int32_t try = 0; try < (MAX_STORE/2); ++try) {
      if (_building->m_stored[try] && (_building->m_next[0]->m_building == NULL || _building->m_next[0]->m_building->m_type == kConveyor)) {
        newCargo(_building->m_next[0], _building->m_stored[try + (MAX_STORE/2)], false);
        if (--_building->m_stored[try] == 0) {
          _building->m_stored[try + 3] = kNoCargo; // Reset the slot if we have run out of items 
        }
        break;
      }
    }
  }

  _building->m_progress -= _tick;
  if (_building->m_progress > 0) return;
  _building->m_progress = COLLECT_TIME;

  // Picking up
  for (int32_t x = -4; x < 5; ++x) {
    for (int32_t y = -4; y < 5; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_cargo && (loc->m_building == NULL || loc->m_building->m_type == kPlant)) {
        // For our three storage locations
        for (int32_t try = 0; try < (MAX_STORE/2); ++try) {
          // Do we already store of this type? Or do we store nothing in this type?
          if (_building->m_stored[try + (MAX_STORE/2)] == kNoCargo || _building->m_stored[try + (MAX_STORE/2)] == loc->m_cargo->m_type) {
            _building->m_stored[try + (MAX_STORE/2)] = loc->m_cargo->m_type;
            if (_building->m_stored[try] < 255) ++_building->m_stored[try]; // TODO, unsigned should not overflow. Check
            clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
            break;
          }
        }
      }
    }
  }

}

void extractorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  switch (_building->m_subType.extractor) {
    case kCropHarvester:; return cropHarveserUpdateFn(_building, _tick);
    case kPump:;          return;
    case kNExtractorSubTypes:; break;
  }
}

bool canBePlacedExtractor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Tile_t* t = getTile(_loc->m_x + x, _loc->m_y + y);
      if (t->m_tile > FLOOR_TILES) { pd->system->logToConsole("check FAILED 1"); return false; }
      if (getLocation(_loc->m_x + x, _loc->m_y + y)->m_building != NULL) { pd->system->logToConsole("check FAILED 1"); return false; }
      pd->system->logToConsole("check passed");
    }
  }
  return true;
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
    switch (_building->m_subType.extractor) {
      case kCropHarvester:; _building->m_image[zoom] = getSprite48(0 + _building->m_dir, 0, zoom); break;
      case kPump:;          _building->m_image[zoom] = getSprite48(0 + _building->m_dir, 1, zoom); break;
      case kNExtractorSubTypes:; break;
    }

    PDRect bound = {.x = 0, .y = 0, .width = EXTRACTOR_PIX*zoom, .height = EXTRACTOR_PIX*zoom};
    if (_building->m_sprite[zoom] == NULL) _building->m_sprite[zoom] = pd->sprite->newSprite();
    pd->sprite->setCollideRect(_building->m_sprite[zoom], bound);
    pd->sprite->moveTo(_building->m_sprite[zoom], (_building->m_pix_x - EXTRACTOR_PIX/2)*zoom, (_building->m_pix_y - EXTRACTOR_PIX/2)*zoom);
  }

  for (int32_t x = _building->m_location->m_x - 1; x < _building->m_location->m_x + 2; ++x) {
    for (int32_t y = _building->m_location->m_y - 1; y < _building->m_location->m_y + 2; ++y) {
      clearLocation(getLocation(x, y), /*cargo*/ true, /*building*/ false);
    }
  }

  _building->m_progress = COLLECT_TIME;
}