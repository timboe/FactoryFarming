#include "factory.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"


/// ///


void factoryUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  switch (_building->m_subType.factory) {
    default:; break;
  }
}

bool canBePlacedFactory(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Tile_t* t = getTile(_loc->m_x + x, _loc->m_y + y);
      if (t->m_tile > FLOOR_TILES) return false;
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
    switch (_building->m_subType.extractor) {
      //case kCropHarvester:; _building->m_image[zoom] = getSprite48(0 + _building->m_dir, 0, zoom); break;
      default: break;
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

}