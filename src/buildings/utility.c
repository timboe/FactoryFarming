#include "utility.h"
#include "extractor.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"

void binUpdateFn(struct Building_t* _building);

void storageUpdateFn(struct Building_t* _building, uint8_t _tick);

/// ///

void storageUpdateFn(struct Building_t* _building, uint8_t _tick) {

  // Pickup
  if (_building->m_location->m_cargo) {
    tryPickupAnyCargo(_building->m_location, _building);
  }

  // Putdown
  _building->m_progress += _tick;
  if (_building->m_progress >= TILE_PIX/2) {
    _building->m_progress = 0;
    tryPutdownAnyCargo(_building, _tick);
  }

}

void binUpdateFn(struct Building_t* _building) {
  if (_building->m_location->m_cargo) {
    clearLocation(_building->m_location, /*clearCargo*/ true, /*clearBuilding*/ false);
  }
}

void utilityUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  if (_building->m_subType.utility == kBin) {
  	return binUpdateFn(_building);
  } else if (_building->m_subType.utility == kStorageBox) {
    return storageUpdateFn(_building, _tick);
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
      case kBin:; _building->m_image[zoom] = getSprite16(14, 14, zoom); break;
      case kWell:; _building->m_image[zoom] = getSprite16(12, 14, zoom); break;
      case kStorageBox:; _building->m_image[zoom] = getSprite16(4 + _building->m_dir, 15, zoom); break;
      case kConveyorGrease:; case kNUtilitySubTypes:; break;
    }
  }
}

void drawUIInspectUtility(struct Building_t* _building) {
  
}