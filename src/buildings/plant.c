#include "plant.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../building.h"
#include "../chunk.h"

int8_t getWaterBonus(enum kGroundWetness _likes, enum kGroundWetness _has);

int8_t getGroundBonus(enum kGroundType _likes, enum kGroundType _has);

uint16_t getGrowdownTimer(struct Building_t* _building, bool _smear);

/// ///

void plantTrySpawnCargo(struct Building_t* _building, uint8_t _tickLength) {
  struct Location_t* loc = _building->m_location;
  if (loc->m_cargo != NULL) {
    return;
  }

  newCargo(loc, PDesc[_building->m_subType.plant].out, _tickLength == NEAR_TICK_AMOUNT);

  _building->m_progress = getGrowdownTimer(_building, true);
  
  ++_building->m_mode.mode16;
  // If _building->m_mode.mode16 >= N_CROPS_BEFORE_FARMLAND or 2*N_CROPS_BEFORE_FARMLAND then we
  // render the soil differently. But it is too expensive to do this here.
  // Wait until the tile gets re-rendered for some other reason. Should still do it for the 1st grow, though
  
  if (_building->m_mode.mode16 == 1) {
    renderChunkBackgroundImage(loc->m_chunk);
  }
}

uint16_t getGrowdownTimer(struct Building_t* _building, bool _smear) {
  const enum kPlantSubType pst = _building->m_subType.plant;
  const struct Tile_t* t = getTile_fromLocation( _building->m_location );
  const int8_t gb = getGroundBonus( PDesc[pst].soil, (enum kGroundType) t->m_groundType );
  const int8_t wb = getWaterBonus( PDesc[pst].wetness, getWetness( t->m_wetness ) );
  const int8_t bonus = gb + wb;
  float growTime = PDesc[_building->m_subType.plant].time * TICKS_PER_SEC;
  if (bonus >= 2) {
    growTime *= 0.7f;
  } else if (bonus == 1) {
    growTime *= 0.9f;
  } else if (bonus == -1) {
    growTime *= 2.0f;
  } else if (bonus <= -2) {
    return INT16_MAX;
  }
  uint8_t smear = (_smear ? rand() % (TICKS_PER_SEC * 3) : 0);
  return (uint16_t)growTime - smear;
}

#define CARGO_BOUNCE_OFFSET 4
void plantUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {
  if (_building->m_tickProcessed == _tickID) return;
  _building->m_tickProcessed = _tickID;
  ++m_recursionCount;

  if (_building->m_location->m_cargo == NULL) {
    _building->m_progress -= _tickLength;
  }

  if (_tickLength == NEAR_TICK_AMOUNT && _building->m_location->m_cargo) {
    _building->m_stored[0] = _building->m_stored[0] + (_building->m_stored[1] == 0 ? 1 : -1);
    if      (_building->m_stored[0] == CARGO_BOUNCE_OFFSET) _building->m_stored[1] = 1;
    else if (_building->m_stored[0] == 0)                   _building->m_stored[1] = 0;
    pd->sprite->moveTo(
      _building->m_location->m_cargo->m_sprite[_zoom], 
      (_building->m_pix_x + _building->m_location->m_pix_off_x)*_zoom, 
      (_building->m_pix_y + _building->m_location->m_pix_off_y - _building->m_stored[0])*_zoom);
  }

  if (_building->m_progress > 0) return;

  plantTrySpawnCargo(_building, _tickLength);

}

bool canBePlacedPlant(struct Location_t* _loc, union kSubType _subType) {
  if (_loc->m_building != NULL) return false;
  if (_subType.plant == kSeaweedPlant || _subType.plant == kSeaCucumberPlant) {
    return isWaterTile(_loc->m_x, _loc->m_y);
  }
  struct Tile_t* t = getTile(_loc->m_x, _loc->m_y);
  if (t->m_tile >= TOT_FLOOR_TILES) return false;
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

  if (_building->m_progress <= 0) {
    _building->m_progress = getGrowdownTimer(_building, true);
  }
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
        case kPeatyGround: return -1;
        case kSandyGround: return 0;
        case kClayGround: return -1;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return -3;
        case kNGroundTypes: return 0;
      }
    case kChalkyGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return +1;
        case kPeatyGround: return -1;
        case kSandyGround: return 0;
        case kClayGround: return -1;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return -3;
        case kNGroundTypes: return 0;
      }
    case kPeatyGround:
      switch (_has) {
        case kSiltyGround: return 0;
        case kChalkyGround: return -1;
        case kPeatyGround: return +1;
        case kSandyGround: return -1;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return 0;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return -3;
        case kNGroundTypes: return 0;
      }
    case kSandyGround:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return +1;
        case kClayGround: return -1;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return -3;
        case kNGroundTypes: return 0;
      }
    case kClayGround:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return -3;
        case kClayGround: return +1;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return -3;
        case kNGroundTypes: return 0;
      }
    case kLoamyGround:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return -3;
        case kClayGround: return -3;
        case kLoamyGround: return +1;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return -3;
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
        case kOcean: return -3;
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
        case kRiver: return 0;
        case kOcean: return -1;
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
        case kLake: return 0;
        case kRiver: return +1;
        case kOcean: return -1;
        case kNGroundTypes: return 0;
      }
    case kOcean:
      switch (_has) {
        case kSiltyGround: return -3;
        case kChalkyGround: return -3;
        case kPeatyGround: return -3;
        case kSandyGround: return -3;
        case kClayGround: return 0;
        case kLoamyGround: return 0;
        case kPavedGround: return -3;
        case kObstructedGround: return -3;
        case kLake: return -3;
        case kRiver: return -3;
        case kOcean: return +1;
        case kNGroundTypes: return 0;
      }
    case kNGroundTypes: return 0;
  }
  return 0;
}

void drawUIInspectPlant(struct Building_t* _building) {
  pd->graphics->drawLine(SCREEN_PIX_X/2, TUT_Y_SPACING*3 - 4, SCREEN_PIX_X/2, TUT_Y_SPACING*7 - 4, 1, kColorBlack);
  pd->graphics->drawLine(TILE_PIX*21,    TUT_Y_SPACING*3 - 4, TILE_PIX*21,    TUT_Y_SPACING*7 - 4, 1, kColorBlack);

  const enum kPlantSubType pst = _building->m_subType.plant;

  static char text[128];
  uint8_t y = 1;
  snprintf(text, 128, "%s", toStringBuilding(_building->m_type, _building->m_subType, true));
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  const struct Tile_t* t = getTile_fromLocation( _building->m_location );
  const int8_t gb = getGroundBonus( PDesc[pst].soil, (enum kGroundType) t->m_groundType );
  const int8_t wb = getWaterBonus( PDesc[pst].wetness, getWetness( t->m_wetness ) );
  const uint16_t growTime = getGrowdownTimer(_building, false);

  const bool isWater = isWaterTile(_building->m_location->m_x, _building->m_location->m_y);

  snprintf(text, 128, "Likes: %s", toStringSoil( PDesc[pst].soil ) );
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  snprintf(text, 128, "Has: %s", toStringSoil( (enum kGroundType) t->m_groundType) ); 
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*13, TUT_Y_SPACING*y - TUT_Y_SHFT);

  snprintf(text, 128, "Likes: %s %s", toStringWetness( PDesc[pst].wetness ), (isWater ? " " : "Soil") ); 
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  snprintf(text, 128, "Has: %s %s", toStringWetness( getWetness(t->m_wetness) ), (isWater ? " " : "Soil") ); 
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*13, TUT_Y_SPACING*y - TUT_Y_SHFT);

  if (_building->m_progress > INT16_MAX/2) {
    snprintf(text, 128, "Cannot Grow Here"); 
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, " "); 
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  } else {
    snprintf(text, 128, "Grow Time: %is", growTime / TICKS_PER_SEC); 
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    if (_building->m_location->m_cargo != NULL) {
      snprintf(text, 128, "Harvest to grow"); 
    } else {
      snprintf(text, 128, "Time Remaining: %is", _building->m_progress / TICKS_PER_SEC); 
    }
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  }

  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(getPlantSmilieSprite(gb), 1), TILE_PIX*21 + TILE_PIX/2, TUT_Y_SPACING*3 - TUT_Y_SHFT - 1, kBitmapUnflipped);
  pd->graphics->drawBitmap(getSprite16_byidx(getPlantSmilieSprite(wb), 1), TILE_PIX*21 + TILE_PIX/2, TUT_Y_SPACING*4 - TUT_Y_SHFT - 1, kBitmapUnflipped);
  pd->graphics->drawBitmap(getSprite16_byidx(getPlantSmilieSprite(gb+wb), 1), TILE_PIX*21 + TILE_PIX/2, TUT_Y_SPACING*5 - TUT_Y_SHFT - 1, kBitmapUnflipped);

}

uint16_t getPlantSmilieSprite(int8_t _value) {
  if (_value < -1)       return SPRITE16_ID(4, 19);
  else if (_value == -1) return SPRITE16_ID(1, 19);
  else if (!_value)      return SPRITE16_ID(2, 19);
  else if (_value == 1)  return SPRITE16_ID(0, 19);
  else                   return SPRITE16_ID(3, 19);
}