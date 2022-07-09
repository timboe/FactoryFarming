#include "factory.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../building.h"

/// ///


void factoryUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  
  // Production
  if (_building->m_stored[0] < 255 /*out*/                 && _building->m_stored[1]  && 
    (kFactoryIn2[_building->m_subType.factory] == kNoCargo || _building->m_stored[2]) &&
    (kFactoryIn3[_building->m_subType.factory] == kNoCargo || _building->m_stored[3]) &&
    (kFactoryIn4[_building->m_subType.factory] == kNoCargo || _building->m_stored[4]) &&
    (kFactoryIn5[_building->m_subType.factory] == kNoCargo || _building->m_stored[5])) 
  {
    _building->m_progress += _tick;
    if (_building->m_progress >= kFactoryTime[_building->m_subType.factory] * TICKS_PER_SEC) {
      _building->m_progress -= (kFactoryTime[_building->m_subType.factory] * TICKS_PER_SEC);
      ++_building->m_stored[0];
      for (int32_t i = 1; i < 6; ++i) if (_building->m_stored[i]) --_building->m_stored[i];
    }
  }

  // Picking up
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_cargo) {
        // Is this one of out inputs?
        if (loc->m_cargo->m_type == kFactoryIn1[_building->m_subType.factory] && _building->m_stored[1] < 255) {
          ++_building->m_stored[1]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == kFactoryIn2[_building->m_subType.factory] && _building->m_stored[2] < 255) {
          ++_building->m_stored[2]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == kFactoryIn3[_building->m_subType.factory] && _building->m_stored[3] < 255) {
          ++_building->m_stored[3]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == kFactoryIn4[_building->m_subType.factory] && _building->m_stored[4] < 255) {
          ++_building->m_stored[4]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == kFactoryIn5[_building->m_subType.factory] && _building->m_stored[5] < 255) {
          ++_building->m_stored[5]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        }
      }
    }
  }

  // Placing down
  if (_building->m_stored[0] && _building->m_next[0]->m_cargo == NULL) {
    --_building->m_stored[0];
    newCargo(_building->m_next[0], kFactoryOut[_building->m_subType.factory], _tick == NEAR_TICK_AMOUNT);
  }
}

bool canBePlacedFactory(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Tile_t* t = getTile(_loc->m_x + x, _loc->m_y + y);
      if (t->m_tile > TOT_FLOOR_TILES) return false;
      if (getLocation(_loc->m_x + x, _loc->m_y + y)->m_building != NULL) return false;
    }
  }
  return true;
}

void assignNeighborsFactory(struct Building_t* _building) {
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

void buildingSetupFactory(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    _building->m_image[zoom] = getSprite48(0 + _building->m_dir, 4, zoom); 

    PDRect bound = {.x = (COLLISION_OFFSET/2)*zoom, .y = (COLLISION_OFFSET/2)*zoom, .width = (EXTRACTOR_PIX - COLLISION_OFFSET)*zoom, .height = (EXTRACTOR_PIX - COLLISION_OFFSET)*zoom};
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

}

uint16_t getNIngreindientsText(enum kFactorySubType _type) {
  switch (_type) {
    case kVitiminFac:; return 11;
    default: return 0;
  }
  return 0;
}

const char* toStringIngredients(enum kFactorySubType _type, uint16_t _n, bool* _isFlavour) {
  *_isFlavour = true;
  switch (_type) {
    case kVitiminFac:;
      switch (_n) {
        case 0: return "Gelatin, ";
        case 1: return "Glycerin, ";
        case 2: return "Beta-";
        case 3: return "Carotene ";
        case 4: *_isFlavour = false; return "(Carrots)";
        case 5: return ", ";
        case 6: return "Soy ";
        case 7: return "Lecithin, ";
        case 8: return "Calcium ";
        case 9: return "Carbonate ";
        case 10: *_isFlavour = false; return "(Chalk)";
      }
    default: return "";
  }
}
