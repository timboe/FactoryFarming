#include "plant.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"

/// ///

// Note: with current settings, anything under 72 cannot be kept up with by a single harvester
#define GROW_TIME (TICKS_PER_SEC*52)
#define GROW_RANDOM (TICKS_PER_SEC*40)

void plantTrySpawnCargo(struct Building_t* _building) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo != NULL) {
    return;
  }
  switch (_building->m_subType.plant) {
    case kCarrotPlant:; newCargo(loc, kCarrot, false);
    case kAppleTree:;   newCargo(loc, kApple, false);
    case kWheatPlant:;  newCargo(loc, kWheat, false);
    case kP4:; case kP5:; case kP6:; case kP7:; case kP8:; case kP9:; case kP10:; case kP11:; case kP12:; break; 
    case kNPlantSubTypes:; break;
  }
  // TODO growing bonuses?
  _building->m_progress = GROW_TIME + rand() % GROW_RANDOM;
  if (++_building->m_mode == N_CROPS_BEFORE_FARMLAND || _building->m_mode == 2*N_CROPS_BEFORE_FARMLAND ) {
    renderChunkBackgroundImage(loc->m_chunk);
  }
}

#define CARGO_BOUNCE_OFFSET 4
void plantUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  _building->m_progress -= _tick;

  if (_tick == NEAR_TICK_AMOUNT && _building->m_location->m_cargo) {
    _building->m_stored[0] = _building->m_stored[0] + (_building->m_stored[1] == 0 ? 1 : -1);
    if      (_building->m_stored[0] == CARGO_BOUNCE_OFFSET) _building->m_stored[1] = 1;
    else if (_building->m_stored[0] == 0)                   _building->m_stored[1] = 0;
    pd->sprite->moveTo(
      _building->m_location->m_cargo->m_sprite[_zoom], 
      (_building->m_pix_x + _building->m_location->m_pix_off_x)*_zoom, 
      (_building->m_pix_y + _building->m_location->m_pix_off_y - _building->m_stored[0])*_zoom);
  }

  if (_building->m_progress > 0) return;

  // TODO: Growning penalties?

  plantTrySpawnCargo(_building);

}

bool canBePlacedPlant(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile > FLOOR_TILES) return false;
  if (_loc->m_building != NULL) return false;
  return true;
}

void assignNeighborsPlant(struct Building_t* _building) {
  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  getBuildingNeighbors(_building, 1, &above, &below, &left, &right); // Not currently used
  _building->m_next[0] = above;
  _building->m_next[1] = right;
  _building->m_next[2] = below;
  _building->m_next[3] = left; 

}

void buildingSetupPlant(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.plant) {
      case kCarrotPlant:; _building->m_image[zoom] = getSprite16(10, 8, zoom); break;
      case kAppleTree:;   _building->m_image[zoom] = getSprite16(8,  8, zoom); break;
      case kWheatPlant:;  _building->m_image[zoom] = getSprite16(11,  8, zoom); break;
      case kNPlantSubTypes:; default: break;
    }
  }

  clearLocation(_building->m_location, /*cargo*/ true, /*building*/ false);

  _building->m_progress = GROW_TIME + rand() % GROW_RANDOM; // Somewhere better to put the initial timer?

}