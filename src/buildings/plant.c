#include "plant.h"
#include "../chunk.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"

void plantSpawnCargo(struct Building_t* _building, struct Location_t* _loc);

/// ///

#define GROW_TIME (TICK_FREQUENCY*2)
#define GROW_RANDOM (TICK_FREQUENCY*2)

void plantSpawnCargo(struct Building_t* _building, struct Location_t* _loc) {
  switch (_building->m_subType.plant) {
    case kCarrotPlant:; newCargo(_loc, kCarrot, false);
    case kAppleTree:;   newCargo(_loc, kApple, false);
    case kNPlantSubTypes:; default: break;
  }
  _building->m_progress = GROW_TIME + rand() % GROW_RANDOM;
  ++_building->m_mode;
}

void plantUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  _building->m_progress -= _tick;

  if (_building->m_progress > 0) return;

  if (_building->m_location->m_cargo == NULL) {
    plantSpawnCargo(_building, _building->m_location);
  }

  // Removed the need for harvesters?  
  /*
  } else {
    for (uint32_t i = 0; i < 4; ++i) {
      if (_building->m_next[i]->m_cargo == NULL && (_building->m_next[i]->m_building == NULL || _building->m_next[i]->m_building->m_type == kConveyor)) {
        plantSpawnCargo(_building, _building->m_next[i]);
        break;
      }
    }
  }
  */
}

bool canBePlacedPlant(struct Location_t* _loc) {
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
  getBuildingNeighbors(_building, &above, &below, &left, &right);
  _building->m_next[0] = above;
  _building->m_next[1] = right;
  _building->m_next[2] = below;
  _building->m_next[3] = left;

  _building->m_progress = GROW_TIME + rand() % GROW_RANDOM; // Somewhere better to put the initial timer?
}

void buildingSpriteSetupPlant(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.plant) {
      case kCarrotPlant:; _building->m_image[zoom] = getSprite16(10, 8, zoom); break;
      case kAppleTree:;   _building->m_image[zoom] = getSprite16(8,  8, zoom); break;
      case kNPlantSubTypes:; default: break;
    }
  }
}