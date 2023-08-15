#include "factory.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../building.h"
#include "../ui.h"
#include "../chunk.h"

int8_t locationHasUpgradeModule(int32_t _x, int32_t _y);

/// ///

void factoryUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom) {

  // Picking up - note we do this BEFORE checking for recursive exit. I.e. we can pick up (if called recursively) multiple times on a single tick
  const enum kFactorySubType fst = _building->m_subType.factory;
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_cargo) {
        // Is this one of out inputs?
        if        (loc->m_cargo->m_type == FDesc[fst].in1 && _building->m_stored[1] < 255) {
          ++_building->m_stored[1]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == FDesc[fst].in2 && _building->m_stored[2] < 255) {
          ++_building->m_stored[2]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == FDesc[fst].in3 && _building->m_stored[3] < 255) {
          ++_building->m_stored[3]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == FDesc[fst].in4 && _building->m_stored[4] < 255) {
          ++_building->m_stored[4]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        } else if (loc->m_cargo->m_type == FDesc[fst].in5 && _building->m_stored[5] < 255) {
          ++_building->m_stored[5]; 
          clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        }
      }
    }
  }

  // Check for recursive call - the following is once per tick only
  if (_building->m_tickProcessed == _tickID) return;
  _building->m_tickProcessed = _tickID;
  ++m_recursionCount;

  // Production
  if (_building->m_stored[0] < 255 /*out*/ &&
    _building->m_stored[1] && 
    (FDesc[fst].in2 == kNoCargo || _building->m_stored[2]) &&
    (FDesc[fst].in3 == kNoCargo || _building->m_stored[3]) &&
    (FDesc[fst].in4 == kNoCargo || _building->m_stored[4]) &&
    (FDesc[fst].in5 == kNoCargo || _building->m_stored[5])) 
  {
    _building->m_progress += _tickLength;
    if (_building->m_progress >= _building->m_mode.mode16) {
      _building->m_progress -= _building->m_mode.mode16;
      ++_building->m_stored[0];
      for (int32_t i = 1; i < 6; ++i) if (_building->m_stored[i]) --_building->m_stored[i];
    }
  }

  // Placing down
  if (_building->m_stored[0] == 0) return; 

  bool ableToMove = true;
  if (_building->m_next[0]->m_cargo != NULL) {
    if (_building->m_next[0]->m_building == NULL) {
      ableToMove = false;
    } else {
      // Try recursion
      (*_building->m_next[0]->m_building->m_updateFn)(_building->m_next[0]->m_building, _tickLength, _tickID, _zoom);
      ableToMove = (_building->m_next[0]->m_cargo == NULL);
    }
  }

  if (ableToMove) {
    --_building->m_stored[0];
    newCargo(_building->m_next[0], FDesc[fst].out, _tickLength == NEAR_TICK_AMOUNT);
  }
}

bool canBePlacedFactory(const struct Location_t* _loc) {
  //TODO fix this bug
  if (_loc->m_x == 0 || _loc->m_y == 0) return false;
  
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Tile_t* t = getTile(_loc->m_x + x, _loc->m_y + y);
      if (t->m_tile >= TOT_FLOOR_TILES_INC_LANDFILL) return false;
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
    _building->m_image[zoom] = getSprite48_byidx(FDesc[_building->m_subType.factory].sprite + _building->m_dir, zoom); 

    PDRect bound = {.x = (COLLISION_OFFSET_BIG/2)*zoom, .y = (COLLISION_OFFSET_BIG/2)*zoom, .width = (EXTRACTOR_PIX - COLLISION_OFFSET_BIG)*zoom, .height = (EXTRACTOR_PIX - COLLISION_OFFSET_BIG)*zoom};
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

int8_t locationHasUpgradeModule(int32_t _x, int32_t _y) {
  struct Location_t* loc = getLocation(_x, _y);
  return (int8_t) (loc->m_building && loc->m_building->m_type == kUtility && loc->m_building->m_subType.utility == kFactoryUpgrade);
}


void updateFactoryUpgrade(struct Building_t* _building) {
  if (!_building || _building->m_type != kFactory) {
    return;
  }

  const enum kFactorySubType fst = _building->m_subType.factory;
  int16_t prodTime = FDesc[fst].time; // Base time

  // Check for upgrade modules, there can in theory be up to 12. But in practice I/O will mean this is much smaller
  int8_t nModules = 0;

  int32_t b_x = _building->m_location->m_x;
  int32_t b_y = _building->m_location->m_y;
  for (int32_t x = b_x - 1; x < b_x + 2; ++x) {
    nModules += locationHasUpgradeModule(x, b_y + 2);
    nModules += locationHasUpgradeModule(x, b_y - 2);
  }
  for (int32_t y = b_y - 1; y < b_y + 2; ++y) {
    nModules += locationHasUpgradeModule(b_x + 2, y);
    nModules += locationHasUpgradeModule(b_x - 2, y);
  }

  int8_t saving = 0;
  if (nModules == 1) {
    saving = (TICKS_PER_SEC/2);
  } else if (nModules == 2) {
    saving = (3*(TICKS_PER_SEC)/4);
  } else if (nModules >= 3) {
    saving = TICKS_PER_SEC;
  }
  prodTime -= saving;

  if (prodTime < TICKS_PER_SEC/2) {
    prodTime = TICKS_PER_SEC/2;
  }

  _building->m_mode.mode16 = (uint16_t) prodTime;

}

void checkUpdateFactoryUpgradeAroundLoc(struct Location_t* _loc) {
  struct Location_t* l = getLocation(_loc->m_x + 1, _loc->m_y);
  if (l->m_building && l->m_building->m_type == kFactory) updateFactoryUpgrade(l->m_building);
  l = getLocation(_loc->m_x - 1, _loc->m_y);
  if (l->m_building && l->m_building->m_type == kFactory) updateFactoryUpgrade(l->m_building);
  l = getLocation(_loc->m_x, _loc->m_y + 1);
  if (l->m_building && l->m_building->m_type == kFactory) updateFactoryUpgrade(l->m_building);
  l = getLocation(_loc->m_x, _loc->m_y - 1);
  if (l->m_building && l->m_building->m_type == kFactory) updateFactoryUpgrade(l->m_building);
}


const char* toStringIngredients(enum kFactorySubType _type, uint16_t _n, bool* _isFlavour) {
  *_isFlavour = true;
  static char compound[64] = " ";
  switch (_type) {
    case kVitiminFac:;
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavGlycerin);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavBeta);
        case 6: return " ";
        case 7: return tr(kTRFacFlavCarotene);
        case 8: return " ";
        case 9: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kCarrot, /*plural=*/ true)); return compound;
        case 10: return ",";
        case 11: return " ";        
        case 12: return tr(kTRFacFlavSoy);
        case 13: return " ";
        case 14: return tr(kTRFacFlavLecithin);
        case 15: return ",";
        case 16: return " ";
        case 17: return tr(kTRFacFlavCalCarbonate0);
        case 18: return " ";
        case 19: return tr(kTRFacFlavCalCarbonate1);
        case 20: return " ";
        case 21: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kChalk, /*plural=*/ false)); return compound;
      }
      break;
    case kVegOilFac:;
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kSunflower, /*plural=*/ true);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavStabilizers);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavPreservatives);
      }
      break;
    case kCrispsFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kPotato, /*plural=*/ true);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kOil, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kSalt, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavArtificialFlav0);
        case 12: return " ";
        case 13: return tr(kTRFacFlavArtificialFlav1);
      }
      break;
    case kEthanolFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavDistilled);
        case 6: return " ";
        case 7: return tr(kTRFacFlavVegetable);
        case 8: return " ";
        case 9: return tr(kTRFacFlavAlcohol);
        case 10: return " ";
        case 11: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kPotato, /*plural=*/ true)); return compound;
      }
      break;
    case kHardCiderFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavAppleJuice0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavAppleJuice1);
        case 5: return " ";
        case 6: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kApple, /*plural=*/ true)); return compound;
        case 7: return ",";
        case 8: return " ";
        case 9: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 10: return ",";
        case 11: return " ";
        case 12: return tr(kTRFacFlavArtificialColoring0);
        case 13: return " ";
        case 14: return tr(kTRFacFlavArtificialColoring1);
        case 15: return ",";
        case 16: return " ";
        case 17: return tr(kTRFacFlavStabilizers);
        case 18: return ",";
        case 19: return " ";
        case 20: return tr(kTRFacFlavPreservatives);      
      }
    case kAbattoir:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavAnimalFeed);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kSunflower, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kSeaweed, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavHormones);
        case 12: return ",";
        case 13: return " ";
        case 14: return tr(kTRFacFlavSteroids);
      }
      break;
    case kHydrogenFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavProcess);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavElectrolysis0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavElectrolysis1);
        case 5: return " ";
        case 6: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
      }
      break;
    case kHOILFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kOil, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kHydrogen, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavStabilizers);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavArtificialColoring0);
        case 12: return " ";
        case 13: return tr(kTRFacFlavArtificialColoring1);
        case 14: return " ";
        case 15: snprintf(compound, 64, "(%s", tr(kTRFacFlavSunset)); return compound;
        case 16: return " ";
        case 17: snprintf(compound, 64, "%s)", tr(kTRFacFlavOrange)); return compound;
      }
      break;
    case kCornDogFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCorn, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavHydrogenatedOil0);
        case 6: return " ";
        case 7: return tr(kTRFacFlavVegetable);
        case 8: return " ";
        case 9: return tr(kTRFacFlavHydrogenatedOil1);
        case 10: return " ";
        case 11: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHOil, /*plural=*/ false)); return compound;
        case 12: return ",";
        case 13: return " ";
        case 14: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 15: return ",";
        case 16: return " ";
        case 17: return tr(kTRFacFlavPreservatives);
      }
    case kBeerFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavHopOil0);
        case 12: return " ";
        case 13: return tr(kTRFacFlavHopOil1);
      }
      break;
    case kTequilaFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavAgave);
        case 3: return " ";
        case 4: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kCactus, /*plural=*/ false)); return compound;
        case 5: return ",";
        case 6: return " ";
        case 7: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 8: return ",";
        case 9: return " ";
        case 10: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 11: return ",";
        case 12: return " ";
        case 13: return tr(kTRFacFlavArtificialFlav0);
        case 14: return " ";
        case 15: return tr(kTRFacFlavArtificialFlav1);
      }
      break;
    case kHFCSFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCorn, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavPreservatives);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavStabilizers);
      }
      break;
    case kGelatinFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavPreservatives);
      }
      break;
    case kJellyFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavHFCS0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavHFCS1);
        case 5: return " ";
        case 6: return tr(kTRFacFlavHFCS2);
        case 7: return " ";
        case 8: return tr(kTRFacFlavHFCS3);
        case 9: return " ";
        case 10: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHFCS, /*plural=*/ false)); return compound;
        case 11: return ",";
        case 12: return " ";
        case 13: *_isFlavour = false; return toStringCargoByType(kGelatin, /*plural=*/ false);
        case 14: return ",";
        case 15: return " ";
        case 16: *_isFlavour = false; return toStringCargoByType(kLime, /*plural=*/ false);
        case 17: return ",";
        case 18: return " ";
        case 19: return tr(kTRFacFlavArtificialColoring0);
        case 20: return " ";
        case 21: return tr(kTRFacFlavArtificialColoring1);
      }
      break;
    case kJelloShotFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kJelly, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kTequila, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavStabilizers);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavArtificialFlav0);
        case 12: return " ";
        case 13: return tr(kTRFacFlavArtificialFlav1);
      }
      break;
    case kEmulsifierFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kSeaweed, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavPreservatives);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavStabilizers);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavAnticakingAgents0);
        case 12: return " ";
        case 13: return tr(kTRFacFlavAnticakingAgents1);
      }
      break;
    case kIceCreamFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavHFCS0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavHFCS1);
        case 5: return " ";
        case 6: return tr(kTRFacFlavHFCS2);
        case 7: return " ";
        case 8: return tr(kTRFacFlavHFCS3);
        case 9: return " ";
        case 10: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHFCS, /*plural=*/ false)); return compound;
        case 11: return ",";
        case 12: return " ";
        case 13: return tr(kTRFacFlavHydrogenatedOil0);
        case 14: return " ";
        case 15: return tr(kTRFacFlavVegetable);
        case 16: return " ";
        case 17: return tr(kTRFacFlavHydrogenatedOil1);
        case 18: return " ";
        case 19: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHOil, /*plural=*/ false)); return compound;
        case 20: return ",";
        case 21: return " ";
        case 22: *_isFlavour = false; return toStringCargoByType(kEmulsifiers, /*plural=*/ true);
        case 23: return ",";
        case 24: return " ";
        case 25: *_isFlavour = false; return toStringCargoByType(kStrawberry, /*plural=*/ true);
      }
      break;
    case kBatteryFarm:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavAnimalFeed);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kCorn, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavAntibiotics);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavDewormer);
        case 12: return ",";
        case 13: return " ";
        case 14: return tr(kTRFacFlavInsecticides);
      }
      break;
    case kProteinFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kEggs, /*plural=*/ true);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kSeaweed, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kChalk, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: *_isFlavour = false; return toStringCargoByType(kVitamin, /*plural=*/ true);
        case 12: return ",";
        case 13: return " ";
        case 14: return tr(kTRFacFlavStabilizers);
        case 15: return ",";
        case 16: return " ";
        case 17: return tr(kTRFacFlavAnticakingAgents0);
        case 18: return " ";
        case 19: return tr(kTRFacFlavAnticakingAgents1);
      }
      break;
    case kChocolateFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavHFCS0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavHFCS1);
        case 5: return " ";
        case 6: return tr(kTRFacFlavHFCS2);
        case 7: return " ";
        case 8: return tr(kTRFacFlavHFCS3);
        case 9: return " ";
        case 10: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHFCS, /*plural=*/ false)); return compound;
        case 11: return ",";
        case 12: return " ";
        case 13: return tr(kTRFacFlavHydrogenatedOil0);
        case 14: return " ";
        case 15: return tr(kTRFacFlavVegetable);
        case 16: return " ";
        case 17: return tr(kTRFacFlavHydrogenatedOil1);
        case 18: return " ";
        case 19: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHOil, /*plural=*/ false)); return compound;
        case 20: return ",";
        case 21: return " ";
        case 22: *_isFlavour = false; return toStringCargoByType(kCocoBean, /*plural=*/ true);
      }
      break;
    case kPieFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kPotato, /*plural=*/ true);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kGelatin, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 12: return ",";
        case 13: return " ";
        case 14: return tr(kTRFacFlavPreservatives);
        case 15: return ",";
        case 16: return " ";
        case 17: return tr(kTRFacFlavArtificialFlav0);
        case 18: return " ";
        case 19: return tr(kTRFacFlavArtificialFlav1);
      }
      break;
    case kMSGFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavGlutamicAcid);
        case 3: return " ";
        case 4: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kWheat, /*plural=*/ false)); return compound;
        case 5: return ",";
        case 6: return " ";
        case 7: return tr(kTRFacFlavSodium);
        case 8: return " ";
        case 9: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kSalt, /*plural=*/ false)); return compound;
      }
      break;
    case kTVDinnerFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCarrot, /*plural=*/ true);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kMeatPie, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavMSG0);
        case 9: return " ";
        case 10: return tr(kTRFacFlavMSG1);
        case 11: return " ";
        case 12: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kMSG, /*plural=*/ false)); return compound;
        case 13: return ",";
        case 14: return " ";
        case 15: *_isFlavour = false; return toStringCargoByType(kEmulsifiers, /*plural=*/ true);
        case 16: return ",";
        case 17: return " ";
        case 18: return tr(kTRFacFlavStabilizers);
      }
      break;
    case kCakeFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavHFCS0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavHFCS1);
        case 5: return " ";
        case 6: return tr(kTRFacFlavHFCS2);
        case 7: return " ";
        case 8: return tr(kTRFacFlavHFCS3);
        case 9: return " ";
        case 10: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHFCS, /*plural=*/ false)); return compound;
        case 11: return ",";
        case 12: return " ";
        case 13: *_isFlavour = false; return toStringCargoByType(kEggs, /*plural=*/ true);
        case 14: return ",";
        case 15: return " ";
        case 16: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 17: return ",";
        case 18: return " ";
        case 19: *_isFlavour = false; return toStringCargoByType(kChocolate, /*plural=*/ false);
      }
      break;
    case kCaffeineFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavRequirements);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCoffeeBean, /*plural=*/ true);
        case 3: return ",";
        case 4: return " ";
        case 5: return tr(kTRFacFlavSolvent);
        case 6: return " ";
        case 7: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kCO2, /*plural=*/ false)); return compound;
      }
      break;
    case kEnergyDrinkFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavHFCS0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavHFCS1);
        case 5: return " ";
        case 6: return tr(kTRFacFlavHFCS2);
        case 7: return " ";
        case 8: return tr(kTRFacFlavHFCS3);
        case 9: return " ";
        case 10: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHFCS, /*plural=*/ false)); return compound;
        case 11: return ",";
        case 12: return " ";
        case 13: *_isFlavour = false; return toStringCargoByType(kCO2, /*plural=*/ false);
        case 14: return ",";
        case 15: return " ";
        case 16: *_isFlavour = false; return toStringCargoByType(kCaffeine, /*plural=*/ false);
        case 17: return ",";
        case 18: return " ";
        case 19: *_isFlavour = false; return toStringCargoByType(kLime, /*plural=*/ false);
        case 20: return ",";
        case 21: return " ";
        case 22: return tr(kTRFacFlavArtificialFlav0);
        case 23: return " ";
        case 24: return tr(kTRFacFlavArtificialFlav1);
      }
      break;
    case kRaveJuiceFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kHardCider, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kEnergyDrink, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavPreservatives);
      }
      break;
    case kPerkPillFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCaffeine, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kChalk, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavTitaniumDioxide0);
        case 9: return " ";
        case 10: return tr(kTRFacFlavTitaniumDioxide1);
      }
      break;
    case kPackagingFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavRequirements);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kBamboo, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kCactus, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavBleach);
      }
      break;
    case kDessertFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kJelly, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kIceCream, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavFruitSyrup0);
        case 9: return " ";
        case 10: return tr(kTRFacFlavFruitSyrup1);
        case 11: return " ";
        case 12: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kApple, /*plural=*/ true)); return compound;
        case 13: return ",";
        case 14: return " ";
        case 15: return tr(kTRFacFlavPreservatives);
      }
      break;
    case kCateringKitFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavPack);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kTVDinner, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kDessert, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kEnergyDrink, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavBox);
        case 12: return " ";
        case 13: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kPackaging, /*plural=*/ false)); return compound;
      }
      break;
    case kChineseReayMealFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kEggs, /*plural=*/ true);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kBamboo, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: return tr(kTRFacFlavMSG0);
        case 12: return " ";
        case 13: return tr(kTRFacFlavMSG1);
        case 14: return " ";
        case 15: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kMSG, /*plural=*/ false)); return compound;
        case 16: return ",";
        case 17: return " ";
        case 18: return tr(kTRFacFlavPreservatives);
        case 19: return ",";
        case 20: return " ";
        case 21: return tr(kTRFacFlavArtificialFlav0);
        case 22: return " ";
        case 23: return tr(kTRFacFlavArtificialFlav1);
      }
      break;
    case kPartyPackFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavPack);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kFooYoung, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kCake, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kRaveJuice, /*plural=*/ false);
        case 9: return ",";
        case 10: return " ";
        case 11: *_isFlavour = false; return toStringCargoByType(kJelloShot, /*plural=*/ true);
        case 12: return ",";
        case 13: return " ";
        case 14: return tr(kTRFacFlavBox);
        case 15: return " ";
        case 16: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kPackaging, /*plural=*/ false)); return compound;
      }
      break;
    case kParfumeFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kSeaCucumber, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: return tr(kTRFacFlavFormaldehyde);        
        case 9: return ",";
        case 10: return " ";
        case 11: *_isFlavour = false; return toStringCargoByType(kStrawberry, /*plural=*/ true);
      }
      break;
    case kMiraclePowderFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kProtein, /*plural=*/ false);
        case 3: return ",";
        case 4: return " ";
        case 5: *_isFlavour = false; return toStringCargoByType(kCocoBean, /*plural=*/ false);
        case 6: return ",";
        case 7: return " ";
        case 8: *_isFlavour = false; return toStringCargoByType(kPerkPills, /*plural=*/ true);
        case 9: return ",";
        case 10: return " ";
        case 11: *_isFlavour = false; return toStringCargoByType(kSeaCucumber, /*plural=*/ false);
      } break;
    case kTurkishDelightFac:
      switch (_n) {
        case 0: *_isFlavour = false; return tr(kTRFacFlavIngredients);
        case 1: return "LB";
        case 2: return tr(kTRFacFlavHFCS0);
        case 3: return " ";
        case 4: return tr(kTRFacFlavHFCS1);
        case 5: return " ";
        case 6: return tr(kTRFacFlavHFCS2);
        case 7: return " ";
        case 8: return tr(kTRFacFlavHFCS3);
        case 9: return " ";
        case 10: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kHFCS, /*plural=*/ false)); return compound;
        case 11: return ",";
        case 12: return " ";
        case 13: *_isFlavour = false; return toStringCargoByType(kRose, /*plural=*/ false);
        case 14: return ",";
        case 15: return " ";
        case 16: return tr(kTRFacFlavStarch);
        case 17: return " ";
        case 18: *_isFlavour = false; snprintf(compound, 64, "(%s)", toStringCargoByType(kPotato, /*plural=*/ false)); return compound;

      } break;
    case kSpareFac0: case kSpareFac1: case kSpareFac2: case kSpareFac3:
    case kSpareFac4: case kSpareFac5: case kSpareFac6: case kSpareFac7: 
    case kSpareFac8: case kSpareFac9: case kSpareFac10: case kSpareFac11:
    case kSpareFac12: case kSpareFac13: case kSpareFac14: case kSpareFac15: case kNFactorySubTypes:;
  }
  return "FIN";
}

#define STR_PRINT_LEN 8
void drawUIInspectFactory(struct Building_t* _building) {
  pd->graphics->drawLine(SCREEN_PIX_X/2, TUT_Y_SPACING*3 - 4, SCREEN_PIX_X/2, TUT_Y_SPACING*7 - 4, 1, kColorBlack);
  enum kFactorySubType fst = _building->m_subType.factory;

  static char text[256];
  static char strFloat[16] = " ";
  static char strTrunc[16] = " ";
  static char strNum[16] = " ";
  uint8_t y = 1;

  strcpy(text, toStringBuilding(_building->m_type, _building->m_subType, false));
  strcat(text, space());
  strcat(text, lb());
  strcat(text, getRotationAsString(kUICatFactory, _building->m_subType.factory, _building->m_dir) );
  strcat(text, rb());

  //snprintf(text, 128, "%s (%s)",
  //  toStringBuilding(_building->m_type, _building->m_subType, false),
  //  getRotationAsString(kUICatFactory, _building->m_subType.factory, _building->m_dir));
  
  pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());

  snprintf(text, 256, tr(kTRFacProdTime), ftos(_building->m_mode.mode16 / (float)TICKS_PER_SEC, 16, strFloat));
  pd->graphics->drawText(text, 256, kUTF8Encoding, 3*TILE_PIX/2, TUT_Y_SPACING*6 - TUT_Y_SHFT +tY());

  snprintf(text, 256, tr(kTRFacTimeRemaining), ftos((_building->m_mode.mode16 -_building->m_progress) / (float)TICKS_PER_SEC, 16, strFloat));
  pd->graphics->drawText(text, 256, kUTF8Encoding, 25*TILE_PIX/2, TUT_Y_SPACING*6 - TUT_Y_SHFT +tY());

  size_t S = 0;

  snprintf(strTrunc, 16, "%.*s", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].out, /*plural=*/(_building->m_stored[S] != 1) ));
  snprintf(strNum, 16, " (%i)", _building->m_stored[S]);
  snprintf(text, 256, "%s", tr(kTROut));
  strcat(text, c5space());
  strcat(text, strTrunc);
  strcat(text, strNum);
  //snprintf(text, 256, tr(kTRFacOut), STR_PRINT_LEN, toStringCargoByType( FDesc[fst].out, /*plural=*/(_building->m_stored[0] != 1) ), _building->m_stored[0]);
  pd->graphics->drawText(text, 256, kUTF8Encoding, 3*TILE_PIX/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].out ].UIIcon, 1), 3*TILE_PIX/2 + trLen(kTROut), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  ++S;
  snprintf(strTrunc, 16, "%.*s", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in1, /*plural=*/(_building->m_stored[S] != 1) ));
  snprintf(strNum, 16, " (%i)", _building->m_stored[S]);
  snprintf(text, 256, tr(kTRFacIn), S);
  strcat(text, c5space());
  strcat(text, strTrunc);
  strcat(text, strNum);
  //snprintf(text, 256, tr(kTRFacIn1), STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in1, /*plural=*/(_building->m_stored[1] != 1) ), _building->m_stored[1]);
  pd->graphics->drawText(text, 256, kUTF8Encoding, 25*TILE_PIX/2, TUT_Y_SPACING*y - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in1 ].UIIcon, 1), 25*TILE_PIX/2 + trLen(kTRFacIn), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in2 == kNoCargo) return;

  ++S;
  snprintf(strTrunc, 16, "%.*s", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in2, /*plural=*/(_building->m_stored[S] != 1) ));
  snprintf(strNum, 16, " (%i)", _building->m_stored[S]);
  snprintf(text, 256, tr(kTRFacIn), S);
  strcat(text, c5space());
  strcat(text, strTrunc);
  strcat(text, strNum);
  //snprintf(text, 256, tr(kTRFacIn2), STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in2, /*plural=*/(_building->m_stored[2] != 1) ), _building->m_stored[2]);
  pd->graphics->drawText(text, 256, kUTF8Encoding, 3*TILE_PIX/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in2 ].UIIcon, 1), 3*TILE_PIX/2 + trLen(kTRFacIn), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in3 == kNoCargo) return;

  ++S;
  snprintf(strTrunc, 16, "%.*s", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in3, /*plural=*/(_building->m_stored[S] != 1) ));
  snprintf(strNum, 16, " (%i)", _building->m_stored[S]);
  snprintf(text, 256, tr(kTRFacIn), S);
  strcat(text, c5space());
  strcat(text, strTrunc);
  strcat(text, strNum);
  //snprintf(text, 256, tr(kTRFacIn3), STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in3, /*plural=*/(_building->m_stored[3] != 1) ), _building->m_stored[3]);
  pd->graphics->drawText(text, 256, kUTF8Encoding, 25*TILE_PIX/2, TUT_Y_SPACING*y - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in3 ].UIIcon, 1), 25*TILE_PIX/2 + trLen(kTRFacIn), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in4 == kNoCargo) return;

  ++S;
  snprintf(strTrunc, 16, "%.*s", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in4, /*plural=*/(_building->m_stored[S] != 1) ));
  snprintf(strNum, 16, " (%i)", _building->m_stored[S]);
  snprintf(text, 256, tr(kTRFacIn), S);
  strcat(text, c5space());
  strcat(text, strTrunc);
  strcat(text, strNum);
  //snprintf(text, 256, tr(kTRFacIn4), STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in4, /*plural=*/(_building->m_stored[4] != 1) ), _building->m_stored[4]);
  pd->graphics->drawText(text, 256, kUTF8Encoding, 3*TILE_PIX/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in4 ].UIIcon, 1), 3*TILE_PIX/2 + trLen(kTRFacIn), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in5 == kNoCargo) return;

  ++S;
  snprintf(strTrunc, 16, "%.*s", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in5, /*plural=*/(_building->m_stored[S] != 1) ));
  snprintf(strNum, 16, " (%i)", _building->m_stored[S]);
  snprintf(text, 256, tr(kTRFacIn), S);
  strcat(text, c5space());
  strcat(text, strTrunc);
  strcat(text, strNum);
  //snprintf(text, 256, tr(kTRFacIn5), STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in5, /*plural=*/(_building->m_stored[5] != 1) ), _building->m_stored[5]);
  pd->graphics->drawText(text, 256, kUTF8Encoding, 25*TILE_PIX/2, TUT_Y_SPACING*y - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in5 ].UIIcon, 1), 25*TILE_PIX/2 + trLen(kTRFacIn), TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

}