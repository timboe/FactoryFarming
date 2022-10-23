#include "factory.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../building.h"
#include "../ui.h"

/// ///


void factoryUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  
  const enum kFactorySubType fst = _building->m_subType.factory;
  // Production
  if (_building->m_stored[0] < 255 /*out*/ &&
    _building->m_stored[1] && 
    (FDesc[fst].in2 == kNoCargo || _building->m_stored[2]) &&
    (FDesc[fst].in3 == kNoCargo || _building->m_stored[3]) &&
    (FDesc[fst].in4 == kNoCargo || _building->m_stored[4]) &&
    (FDesc[fst].in5 == kNoCargo || _building->m_stored[5])) 
  {
    _building->m_progress += _tick;
    if (_building->m_progress >= FDesc[fst].time * TICKS_PER_SEC) {
      _building->m_progress -= (FDesc[fst].time * TICKS_PER_SEC);
      ++_building->m_stored[0];
      for (int32_t i = 1; i < 6; ++i) if (_building->m_stored[i]) --_building->m_stored[i];
    }
  }

  // Picking up
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

  // Placing down
  if (_building->m_stored[0] && _building->m_next[0]->m_cargo == NULL) {
    --_building->m_stored[0];
    newCargo(_building->m_next[0], FDesc[fst].out, _tick == NEAR_TICK_AMOUNT);
  }
}

bool canBePlacedFactory(struct Location_t* _loc) {
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

const char* toStringIngredients(enum kFactorySubType _type, uint16_t _n, bool* _isFlavour) {
  *_isFlavour = true;
  switch (_type) {
    case kVitiminFac:;
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "Glycerin, ";
        case 3: return "Beta ";
        case 4: return "Carotene ";
        case 5: return "LB";
        case 6: *_isFlavour = false; return "(";
        case 7: *_isFlavour = false; return toStringCargoByType(kCarrot, /*plural=*/ false);
        case 8: *_isFlavour = false; return ")";
        case 9: return ", ";
        case 10: return "Soy ";
        case 11: return "Lecithin, ";
        case 12: return "Calcium ";
        case 13: return "Carbonate ";
        case 14: return "LB";
        case 15: *_isFlavour = false; return "(";
        case 16: *_isFlavour = false; return toStringCargoByType(kChalk, /*plural=*/ false);
        case 17: *_isFlavour = false; return ")";

      }
      break;
    case kVegOilFac:;
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: *_isFlavour = false; return toStringCargoByType(kSunflower, /*plural=*/ true);
        case 2: return ", ";
        case 3: return "LB";
        case 4: return "Stabilizers";
        case 5: return ", ";
        case 6: return "Preservatives";
      }
      break;
    case kCrispsFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kPotato, /*plural=*/ true);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kOil, /*plural=*/ false);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kSalt, /*plural=*/ false);
        case 7: return ", ";
        case 8: return "Artificial ";
        case 9: return "Flavorings";
      }
      break;
    case kEthanolFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 3: return ", ";
        case 4: return "Distilled ";
        case 5: return "Vegetable ";
        case 6: return "Alcohol ";
        case 7: return "LB";
        case 8: *_isFlavour = false; return "(";
        case 9: *_isFlavour = false; return toStringCargoByType(kPotato, /*plural=*/ true);
        case 10: *_isFlavour = false; return ")";
      }
      break;
    case kHardCiderFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "Apple ";
        case 3: return "Juice ";
        case 4: *_isFlavour = false; return "(";
        case 5: *_isFlavour = false; return toStringCargoByType(kApple, /*plural=*/ true);
        case 6: *_isFlavour = false; return ")";
        case 7: return ", ";
        case 8: return "LB";
        case 9: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 10: return ", ";
        case 11: return "Artificial ";
        case 12: return "Coloring";
        case 13: return ", ";
        case 14: return "Stabilizers";
        case 15: return ", ";
        case 16: return "Preservatives";      
      }
    case kAbattoir:
      switch (_n) {
        case 0: *_isFlavour = false; return "Animal Feed: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kSunflower, /*plural=*/ false);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kSeaweed, /*plural=*/ false);
        case 7: return ", ";
        case 8: return "Hormones";
        case 9: return ", ";
        case 10: return "Steroids";
      }
      break;
    case kHydrogenFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Process: ";
        case 1: return "LB";
        case 2: return "Electrolysis ";
        case 3: return "of ";
        case 4: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
      }
      break;
    case kHOILFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: *_isFlavour = false; return toStringCargoByType(kOil, /*plural=*/ false);
        case 2: return ", ";
        case 3: *_isFlavour = false; return toStringCargoByType(kHydrogen, /*plural=*/ false);
        case 4: return ", ";
        case 5: return "Stabilizers";
        case 6: return ", ";
        case 7: return "Artificial ";
        case 8: return "Colorant ";
        case 9: return "(Sunset ";
        case 10: return "Orange)";
      }
      break;
    case kCornDogFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCorn, /*plural=*/ false);
        case 3: return ", ";
        case 4: return "Hydrogenated ";
        case 5: return "Vegetable ";
        case 6: return "Oil ";
        case 7: *_isFlavour = false; return "(";
        case 8: *_isFlavour = false; return toStringCargoByType(kHOil, /*plural=*/ false);
        case 9: *_isFlavour = false; return ")";
        case 10: return ", ";
        case 11: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 12: return ", ";
        case 13: return "Preservatives";
      }
    case kBeerFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "LB";
        case 7: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 8: return ", ";
        case 9: return "Hop ";
        case 10: return "Oil";
      }
      break;
    case kTequilaFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "Agave ";
        case 2: *_isFlavour = false; return "(";
        case 3: *_isFlavour = false; return toStringCargoByType(kCactus, /*plural=*/ false);
        case 4: *_isFlavour = false; return ")";
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 7: return ", ";
        case 8: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 9: return ", ";
        case 10: return "Artificial ";
        case 11: return "Flavorings";
      }
      break;
    case kHFCSFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCorn, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Preservatives";
        case 7: return ", ";
        case 8: return "Stabilizers";
      }
      break;
    case kGelatinFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 3: return ", ";
        case 4: return "Preservatives";
      }
      break;
    case kJellyFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "High ";
        case 3: return "Fructose ";
        case 4: return "Corn ";
        case 5: return "Syrup ";
        case 6: *_isFlavour = false; return "(";
        case 7: *_isFlavour = false; return toStringCargoByType(kHFCS, /*plural=*/ false);
        case 8: *_isFlavour = false; return ")";
        case 9: return ", ";
        case 10: return "LB";
        case 11: *_isFlavour = false; return toStringCargoByType(kGelatin, /*plural=*/ false);
        case 12: return ", ";
        case 13: *_isFlavour = false; return toStringCargoByType(kLime, /*plural=*/ false);
        case 14: return ", ";
        case 15: return "Artificial ";
        case 16: return "Coloring";
      }
      break;
    case kJelloShotFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kJelly, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kTequila, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Stabilizers";
        case 7: return ", ";
        case 8: return "Artificial ";
        case 9: return "Flavorings";
      }
      break;
    case kEmulsifierFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kSeaweed, /*plural=*/ false);
        case 3: return ", ";
        case 4: return "Preservatives";
        case 5: return ", ";
        case 6: return "LB";
        case 7: return "Stabilizers";
        case 8: return ", ";
        case 9: return "Anticaking ";
        case 10: return "Agents";
      }
      break;
    case kIceCreamFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "High ";
        case 3: return "Fructose ";
        case 4: return "Corn ";
        case 5: return "Syrup ";
        case 6: *_isFlavour = false; return "(";
        case 7: *_isFlavour = false; return toStringCargoByType(kHFCS, /*plural=*/ false);
        case 8: *_isFlavour = false; return ")";
        case 9: return ", ";
        case 10: return "Hydrogenated ";
        case 11: return "Vegetable ";
        case 12: return "Oil ";
        case 13: *_isFlavour = false; return "(";
        case 14: *_isFlavour = false; return toStringCargoByType(kHOil, /*plural=*/ false);
        case 15: *_isFlavour = false; return ")";
        case 16: return ", ";
        case 17: *_isFlavour = false; return toStringCargoByType(kEmulsifiers, /*plural=*/ true);
        case 18: return ", ";
        case 19: *_isFlavour = false; return toStringCargoByType(kStrawberry, /*plural=*/ false);
      }
      break;
    case kBatteryFarm:
      switch (_n) {
        case 0: *_isFlavour = false; return "Animal Feed: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kWaterBarrel, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kCorn, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Antibiotics";
        case 7: return ", ";
        case 8: return "Dewormer";
        case 9: return ", ";
        case 10: return "Insecticides";
      }
      break;
    case kProteinFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kEggs, /*plural=*/ true);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kSeaweed, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "LB";
        case 7: *_isFlavour = false; return toStringCargoByType(kChalk, /*plural=*/ false);
        case 8: return ", ";
        case 9: *_isFlavour = false; return toStringCargoByType(kVitamin, /*plural=*/ true);
        case 10: return ", ";
        case 11: return "Stabilizers";
        case 12: return ", ";
        case 13: return "Anticaking ";
        case 14: return "Agents";
      }
      break;
    case kChocolateFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "High ";
        case 3: return "Fructose ";
        case 4: return "Corn ";
        case 5: return "Syrup ";
        case 6: *_isFlavour = false; return "(";
        case 7: *_isFlavour = false; return toStringCargoByType(kHFCS, /*plural=*/ false);
        case 8: *_isFlavour = false; return ")";
        case 9: return ", ";
        case 10: return "Hydrogenated ";
        case 11: return "Vegetable ";
        case 12: return "Oil ";
        case 13: *_isFlavour = false; return "(";
        case 14: *_isFlavour = false; return toStringCargoByType(kHOil, /*plural=*/ false);
        case 15: *_isFlavour = false; return ")";
        case 16: return ", ";
        case 17: *_isFlavour = false; return toStringCargoByType(kCocoBean, /*plural=*/ true);
      }
      break;
    case kPieFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kPotato, /*plural=*/ true);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kGelatin, /*plural=*/ false);
        case 7: return ", ";
        case 8: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 9: return ", ";
        case 10: return "Preservatives";
        case 11: return ", ";
        case 12: return "Artificial ";
        case 13: return "Flavorings";
      }
      break;
    case kMSGFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "Gluten ";
        case 3: *_isFlavour = false; return "(";
        case 4: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 5: *_isFlavour = false; return ")";
        case 6: return ", ";
        case 7: return "Sodium ";
        case 8: *_isFlavour = false; return "(";
        case 9: *_isFlavour = false; return toStringCargoByType(kSalt, /*plural=*/ false);
        case 10: *_isFlavour = false; return ")";
      }
      break;
    case kTVDinnerFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCarrot, /*plural=*/ true);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kMeatPie, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Monosodium ";
        case 7: return "Glutamate";
        case 8: return "LB";
        case 9: *_isFlavour = false; return "(";
        case 10: *_isFlavour = false; return toStringCargoByType(kMSG, /*plural=*/ false);
        case 11: *_isFlavour = false; return ")";
        case 12: return ", ";
        case 13: *_isFlavour = false; return toStringCargoByType(kEmulsifiers, /*plural=*/ true);
        case 14: return ", ";
        case 15: return "Stabilizers";
      }
      break;
    case kCakeFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "High ";
        case 3: return "Fructose ";
        case 4: return "Corn ";
        case 5: return "Syrup ";
        case 6: *_isFlavour = false; return "(";
        case 7: *_isFlavour = false; return toStringCargoByType(kHFCS, /*plural=*/ false);
        case 8: *_isFlavour = false; return ")";
        case 9: return ", ";
        case 10: *_isFlavour = false; return toStringCargoByType(kEggs, /*plural=*/ true);
        case 11: return ", ";
        case 12: *_isFlavour = false; return toStringCargoByType(kWheat, /*plural=*/ false);
        case 13: return ", ";
        case 14: *_isFlavour = false; return toStringCargoByType(kChocolate, /*plural=*/ false);
      }
      break;
    case kCaffeineFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Requirements: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCoffeeBean, /*plural=*/ true);
        case 3: return ", ";
        case 4: return "Solvent ";
        case 5: *_isFlavour = false; return "(";
        case 6: *_isFlavour = false; return toStringCargoByType(kCO2, /*plural=*/ false);
        case 7: *_isFlavour = false; return ")";
      }
      break;
    case kEnergyDrinkFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: return "High ";
        case 3: return "Fructose ";
        case 4: return "Corn ";
        case 5: return "Syrup ";
        case 6: *_isFlavour = false; return "(";
        case 7: *_isFlavour = false; return toStringCargoByType(kHFCS, /*plural=*/ false);
        case 8: *_isFlavour = false; return ")";
        case 9: return ", ";
        case 10: *_isFlavour = false; return toStringCargoByType(kCO2, /*plural=*/ false);
        case 11: return ", ";
        case 12: *_isFlavour = false; return toStringCargoByType(kCaffeine, /*plural=*/ false);
        case 13: return ", ";
        case 14: *_isFlavour = false; return toStringCargoByType(kLime, /*plural=*/ false);
        case 15: return ", ";
        case 16: return "Artificial ";
        case 17: return "Flavorings";
      }
      break;
    case kRaveJuiceFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kHardCider, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kEnergyDrink, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Preservatives";
      }
      break;
    case kPerkPillFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kCaffeine, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kChalk, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Titanium ";
        case 7: return "Dioxide";
      }
      break;
    case kPackagingFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Requirements: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kBamboo, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kCactus, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Bleach";
      }
      break;
    case kDessertFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kJelly, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kIceCream, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Fruit ";
        case 7: return "Syrup ";
        case 8: *_isFlavour = false; return "(";
        case 9: *_isFlavour = false; return toStringCargoByType(kApple, /*plural=*/ true);
        case 10: *_isFlavour = false; return ")";
        case 11: return ", ";
        case 12: return "Preservatives";
      }
      break;
    case kCateringKitFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Pack Content: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kTVDinner, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kDessert, /*plural=*/ false);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kEnergyDrink, /*plural=*/ false);
        case 7: return ". Box ";
        case 8: return "LB";
        case 9: *_isFlavour = false; return "(";
        case 10: *_isFlavour = false; return toStringCargoByType(kPackaging, /*plural=*/ false);
        case 11: *_isFlavour = false; return ")";
      }
      break;
    case kChineseReayMealFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kMeat, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kEggs, /*plural=*/ true);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kBamboo, /*plural=*/ false);
        case 7: return ", ";
        case 8: *_isFlavour = false; return toStringCargoByType(kMSG, /*plural=*/ false);
        case 9: return ", ";
        case 10: return "Preservatives";
        case 11: return ", ";
        case 12: return "Artificial ";
        case 13: return "Flavorings";
      }
      break;
    case kPartyPackFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Pack Content: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kBeer, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kCake, /*plural=*/ false);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kEnergyDrink, /*plural=*/ false);
        case 7: return ", ";
        case 8: *_isFlavour = false; return toStringCargoByType(kJelloShot, /*plural=*/ true);
        case 9: return ". Box ";
        case 10: *_isFlavour = false; return "(";
        case 11: *_isFlavour = false; return toStringCargoByType(kPackaging, /*plural=*/ false);
        case 12: *_isFlavour = false; return ")";
      }
      break;
    case kParfumeFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kEthanol, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kSeaCucumber, /*plural=*/ false);
        case 5: return ", ";
        case 6: return "Formaldehyde";        
        case 7: return ", ";
        case 8: *_isFlavour = false; return toStringCargoByType(kStrawberry, /*plural=*/ true);
      }
      break;
    case kMiraclePowderFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "LB";
        case 2: *_isFlavour = false; return toStringCargoByType(kProtein, /*plural=*/ false);
        case 3: return ", ";
        case 4: *_isFlavour = false; return toStringCargoByType(kCocoBean, /*plural=*/ false);
        case 5: return ", ";
        case 6: *_isFlavour = false; return toStringCargoByType(kCoffeeBean, /*plural=*/ false);
        case 7: return ", ";
        case 8: *_isFlavour = false; return toStringCargoByType(kSeaCucumber, /*plural=*/ false);
      } break;
    case kTurkishDelightFac:
      switch (_n) {
        case 0: *_isFlavour = false; return "Ingredients: ";
        case 1: return "High ";
        case 2: return "Fructose ";
        case 3: return "Corn ";
        case 4: return "Syrup ";
        case 5: *_isFlavour = false; return "(";
        case 6: *_isFlavour = false; return toStringCargoByType(kHFCS, /*plural=*/ false);
        case 7: *_isFlavour = false; return ")";
        case 8: return ", ";
        case 9: *_isFlavour = false; return toStringCargoByType(kRose, /*plural=*/ false);
        case 10: return ", ";
        case 11: return "Starch ";
        case 12: *_isFlavour = false; return "(";
        case 13: *_isFlavour = false; return toStringCargoByType(kPotato, /*plural=*/ false);
        case 14: *_isFlavour = false; return ")"; 
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

  static char text[128];
  uint8_t y = 1;
  snprintf(text, 128, "%s (%s)",
    toStringBuilding(_building->m_type, _building->m_subType, false),
    getRotationAsString(kUICatFactory, _building->m_subType.factory, _building->m_dir));
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  snprintf(text, 128, "Prod. Time: %is", FDesc[fst].time);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*6 - TUT_Y_SHFT);

  snprintf(text, 128, "Time Remaining: %is", FDesc[fst].time - (_building->m_progress) / TICKS_PER_SEC);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*13, TUT_Y_SPACING*6 - TUT_Y_SHFT);

  snprintf(text, 128, "Out:      %.*s (%i)", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].out, /*plural=*/(_building->m_stored[0] != 1) ), _building->m_stored[0]);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].out ].UIIcon, 1), TILE_PIX*3 + 12, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  snprintf(text, 128, "In 1:       %.*s (%i)", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in1, /*plural=*/(_building->m_stored[1] != 1) ), _building->m_stored[1]);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*13, TUT_Y_SPACING*y - TUT_Y_SHFT);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in1 ].UIIcon, 1), TILE_PIX*14 + 12, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in2 == kNoCargo) return;

  snprintf(text, 128, "In 2:       %.*s (%i)", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in2, /*plural=*/(_building->m_stored[2] != 1) ), _building->m_stored[2]);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in2 ].UIIcon, 1), TILE_PIX*3 + 12, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in3 == kNoCargo) return;

  snprintf(text, 128, "In 3:       %.*s (%i)", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in3, /*plural=*/(_building->m_stored[3] != 1) ), _building->m_stored[3]);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*13, TUT_Y_SPACING*y - TUT_Y_SHFT);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in3 ].UIIcon, 1), TILE_PIX*14 + 12, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in4 == kNoCargo) return;

  snprintf(text, 128, "In 4:       %.*s (%i)", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in4, /*plural=*/(_building->m_stored[4] != 1) ), _building->m_stored[4]);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in4 ].UIIcon, 1), TILE_PIX*3 + 12, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  if (FDesc[fst].in5 == kNoCargo) return;

  snprintf(text, 128, "In 5:       %.*s (%i)", STR_PRINT_LEN, toStringCargoByType( FDesc[fst].in5, /*plural=*/(_building->m_stored[5] != 1) ), _building->m_stored[5]);
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*13, TUT_Y_SPACING*y - TUT_Y_SHFT);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ FDesc[fst].in5 ].UIIcon, 1), TILE_PIX*14 + 12, TUT_Y_SPACING*y - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

}