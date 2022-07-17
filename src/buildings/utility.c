#include "utility.h"
#include "extractor.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../ui.h"

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
  const enum kUtilitySubType ust = _building->m_subType.utility;

  static char text[128];
  uint8_t y = 1;

  if (ust == kStorageBox) {
    snprintf(text, 128, "%s (%s)", 
      toStringBuilding(_building->m_type, _building->m_subType, false), 
      getRotationAsString(kUICatConv, kBelt, _building->m_dir) );
  } else {
    snprintf(text, 128, "%s", toStringBuilding(_building->m_type, _building->m_subType, false));
  }
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  if (ust == kBin) {

    snprintf(text, 128, "Permanently erases unwanted Cargo.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Drop the unwanted Cargo, or use Conveyors");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kWell) {

    snprintf(text, 128, "Creates a surrounding area of Wet and Moist soil.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (ust == kStorageBox) {

    for (int32_t compartment = 0; compartment < 3; ++compartment) {
      if (!_building->m_stored[compartment]) continue;

      snprintf(text, 128, "Compartment %i/3:       %s (%i)", 
        (int)compartment+1, 
        toStringCargoByType( _building->m_stored[(MAX_STORE/2) + compartment] ), 
        _building->m_stored[compartment]);
      pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
      pd->graphics->setDrawMode(kDrawModeCopy);
      pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ _building->m_stored[(MAX_STORE/2) + compartment] ].UIIcon, 1), TILE_PIX*10, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
      pd->graphics->setDrawMode(kDrawModeFillBlack);
    }

  }
}