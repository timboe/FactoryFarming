#include "special.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../player.h"

struct Building_t* m_sellBox = NULL;

struct Building_t* m_buyBox = NULL;

struct Building_t* m_warp = NULL;

struct Building_t* m_exportBox = NULL;

struct Building_t* m_importBox = NULL;

void sellBoxUpdateFn(struct Building_t* _building);

bool nearbyConveyor(struct Building_t* _building);

/// ///

int16_t distanceFromBuy() {
  if (!m_buyBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_buyBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_buyBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromSell() {
  if (!m_buyBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_sellBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_sellBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromWarp() {
  if (!m_warp) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_warp->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_warp->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromOut() {
  if (!m_exportBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_exportBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_exportBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromIn() {
  if (!m_importBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_importBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_importBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

bool nearbyConveyor(struct Building_t* _building) {
  for (int32_t x = -2; x < 3; ++x) {
    for (int32_t y = -2; y < 3; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_building && loc->m_building->m_type == kConveyor) {
        return true;
      }
    }
  }
  return false;
}

void sellBoxUpdateFn(struct Building_t* _building) {
  for (int32_t x = _building->m_location->m_x - 1; x < _building->m_location->m_x + 2; ++x) {
    for (int32_t y = _building->m_location->m_y - 1; y < _building->m_location->m_y + 2; ++y) {
      struct Location_t* loc = getLocation_noCheck(x, y); // Will never straddle the world boundary
      if (loc->m_cargo) {
        modMoney( kCargoValue[loc->m_cargo->m_type] );
        // Tutorial
        const enum kUITutorialStage tut = getTutorialStage(); 
        if (tut == kTutBuildConveyor && loc->m_cargo->m_type == kCarrot && nearbyConveyor(_building)) {
          makeTutorialProgress();
        }
        // Tutorial 
        if (tut == kTutBuildVitamin && loc->m_cargo->m_type == kVitamin && nearbyConveyor(_building)) {
          makeTutorialProgress();
        }
        clearLocation(loc, /*cargo*/ true, /*building*/ false);
      }
    }
  }
}

void specialUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  switch (_building->m_subType.special) {
    case kSellBox:; return sellBoxUpdateFn(_building);
    case kShop:; case kExportBox:; case kImportBox:; case kWarp:; return;
    case kNSpecialSubTypes:; return;
  }
}

bool canBePlacedSpecial(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  return true;
}

void assignNeighborsSpecial(struct Building_t* _building) {
}

void buildingSetupSpecial(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.special) {
      case kShop:;      _building->m_image[zoom] = getSprite48(1, 1, zoom); m_buyBox = _building; break;
      case kSellBox:;   _building->m_image[zoom] = getSprite48(0, 1, zoom); m_sellBox = _building; break;
      case kExportBox:; _building->m_image[zoom] = getSprite48(2, 1, zoom); m_exportBox = _building; break;
      case kImportBox:; _building->m_image[zoom] = getSprite48(3, 1, zoom); m_importBox = _building; break;
      case kWarp:;      _building->m_image[zoom] = getSprite48(0, 2, zoom); m_warp = _building; break;
      case kNSpecialSubTypes:;
    }

    PDRect bound = {.x = (COLLISION_OFFSET/2)*zoom, .y = (COLLISION_OFFSET/2)*zoom, .width = (EXTRACTOR_PIX-COLLISION_OFFSET)*zoom, .height = (EXTRACTOR_PIX-COLLISION_OFFSET)*zoom};
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