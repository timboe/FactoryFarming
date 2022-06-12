#include "special.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"

/// ///

void specialUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {

}

bool canBePlacedSpecial(struct Location_t* _loc) {
  return true;
}

void assignNeighborsSpecial(struct Building_t* _building) {
}

void buildingSetupSpecial(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.special) {
      case kShop:;    _building->m_image[zoom] = getSprite48(1, 1, zoom); break;
      case kSellBox:; _building->m_image[zoom] = getSprite48(0, 1, zoom); break;
      case kExportBox:; _building->m_image[zoom] = getSprite48(2, 1, zoom); break;
      case kImportBox:; _building->m_image[zoom] = getSprite48(3, 1, zoom); break;
      case kNSpecialSubTypes:; default: break;
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