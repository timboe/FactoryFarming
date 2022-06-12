#include "extractor.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"

/// ///

void extractorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {

}

bool canBePlacedExtractor(struct Location_t* _loc) {
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile > FLOOR_TILES) return false;
  if (_loc->m_building != NULL) return false;
  return true;
}

void assignNeighborsExtractor(struct Building_t* _building) {
  return;
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, &above, &below, &left, &right);
  _building->m_next[0] = above;
  _building->m_next[1] = right;
  _building->m_next[2] = below;
  _building->m_next[3] = left;

}

void buildingSetupExtractor(struct Building_t* _building) {
  return;

  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.plant) {
      case kCarrotPlant:; _building->m_image[zoom] = getSprite16(10, 8, zoom); break;
      case kAppleTree:;   _building->m_image[zoom] = getSprite16(8,  8, zoom); break;
      case kNPlantSubTypes:; default: break;
    }
  }

  clearLocation(_building->m_location, /*cargo*/ true, /*building*/ false);


}