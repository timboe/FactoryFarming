#include "utility.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"

/// ///

void utilityUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  if (_building->m_subType.utility != kStorageBox) {
  	return;
  }
}

bool canBePlacedUtility(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile > TOT_FLOOR_TILES) return false;
  if (_loc->m_building != NULL) return false;
  return true;
}

void assignNeighborsUtility(struct Building_t* _building) {
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, 1, &above, &below, &left, &right);
  // This is only used by kStorageBox
  switch (_building->m_dir) {
    case SN:; _building->m_next[0] = above; break;
    case WE:; _building->m_next[0] = right; break;
    case NS:; _building->m_next[0] = below; break;
    case EW:; _building->m_next[0] = left; break;
    case kDirN:; break;
  }
}

void buildingSetupUtility(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.utility) {
      case kWell:; _building->m_image[zoom] = getSprite16(12, 14, zoom); break;
      case kStorageBox:; _building->m_image[zoom] = getSprite16(4 + _building->m_dir, 15, zoom); break;
      case kConveyorGrease:; case kNUtilitySubTypes:; break;
    }
  }
}