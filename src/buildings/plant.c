#include "plant.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../building.h"

int8_t getWaterBonus(enum kGroundWetness _likes, enum kGroundWetness _has);

int8_t getGroundBonus(enum kGroundType _likes, enum kGroundType _has);

void setGrowdownTimer(struct Building_t* _building);

/// ///

// Note: with current settings, anything under 72 cannot be kept up with by a single harvester
//#define GROW_TIME (TICKS_PER_SEC*52)
//#define GROW_RANDOM (TICKS_PER_SEC*40)

void plantTrySpawnCargo(struct Building_t* _building, uint8_t _tick) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo != NULL) {
    return;
  }

  newCargo(loc, PDesc[_building->m_subType.plant].out, _tick == NEAR_TICK_AMOUNT);

  setGrowdownTimer(_building);
  
  if (++_building->m_mode.mode16 == N_CROPS_BEFORE_FARMLAND || _building->m_mode.mode16 == 2*N_CROPS_BEFORE_FARMLAND ) {
    renderChunkBackgroundImage(loc->m_chunk);
  }
}

void setGrowdownTimer(struct Building_t* _building) {
  // TODO growing bonuses?
  _building->m_progress = PDesc[_building->m_subType.plant].time * TICKS_PER_SEC;
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

  plantTrySpawnCargo(_building, _tick);

}

bool canBePlacedPlant(struct Location_t* _loc, enum kDir _dir, union kSubType _subType) {
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile > TOT_FLOOR_TILES) return false;
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
    _building->m_image[zoom] = getSprite16_byidx( PDesc[_building->m_subType.plant].sprite, zoom); break;
  }

  clearLocation(_building->m_location, /*cargo*/ true, /*building*/ false);

  setGrowdownTimer(_building);
}

int8_t getWaterBonus(enum kGroundWetness _likes, enum kGroundWetness _has) {
  switch (_likes) {
    case kWater:
      switch (_has) {
        case kWater: return +1;
        case kWet: return -1;
        case kMoist: return -2;
        case kDry: return -3;
        case kNGroundWetnesses: return 0;
      }
    case kWet:
      switch (_has) {
        case kWater: return -1;
        case kWet: return +1;
        case kMoist: return 0;
        case kDry: return -1;
        case kNGroundWetnesses: return 0;
      }
    case kMoist:
      switch (_has) {
        case kWater: return -2;
        case kWet: return 0;
        case kMoist: return +1;
        case kDry: return 0;
        case kNGroundWetnesses: return 0;
      }
    case kDry:
      switch (_has) {
        case kWater: return -3;
        case kWet: return -1;
        case kMoist: return 0;
        case kDry: return +1;
        case kNGroundWetnesses: return 0;
      }
    case kNGroundWetnesses: return 0;
  }
  return 0;
}

int8_t getGroundBonus(enum kGroundType _likes, enum kGroundType _has) {
  switch (_likes) {
    case kSiltyGround:
      switch (_has) {
        case kSiltyGround: return +1;
        case kChalkyGround: return 0;
        case kPeatyGround: return 0;
        case kSandyGround: return 0;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kChalkyGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return +1;
        case kPeatyGround: return 0;
        case kSandyGround: return 0;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kPeatyGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return 0;
        case kPeatyGround: return +1;
        case kSandyGround: return 0;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return 0;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kSandyGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return 0;
        case kPeatyGround: return 0;
        case kSandyGround: return +1;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kClayGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return 0;
        case kPeatyGround: return 0;
        case kSandyGround: return 0;
        case kClayGround: return +1;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kLoamyGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return 0;
        case kPeatyGround: return 0;
        case kSandyGround: return 0;
        case kClayGround: return 0;
        case kLoamyGround: return +1;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kPavedGround:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return -3;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return +1;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kNGroundTypes: return 0;
      }
    case kObstructedGround: return -3;
    case kLake:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return -3;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return +1;
        case kRiver: return +1;
        case kNGroundTypes: return 0;
      }
    case kRiver:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return -3;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return +1;
        case kRiver: return +1;
        case kNGroundTypes: return 0;
      }
    case kNGroundTypes: return 0;
  }
  return 0;
}

void drawUIInspectPlant(struct Building_t* _building) {
  
}