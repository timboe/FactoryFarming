#include "building.h"
#include "sprite.h"

#define DIS UINT32_MAX

// Note: User can never themselves build a kTunnelOut
const struct ConveyorDescriptor_t CDesc[] = {
  {.subType = kBelt,      .unlock = 5,       .price = 10,         .UIIcon = SID(0,17), .spriteX = 0,  .spriteY = 7},
  {.subType = kSplitL,    .unlock = 10,      .price = 50,         .UIIcon = SID(4,17), .spriteX = 1,  .spriteY = 11},
  {.subType = kSplitI,    .unlock = 15,      .price = 50,         .UIIcon = SID(0,20),  .spriteX = 0,  .spriteY = 11},
  {.subType = kSplitT,    .unlock = 20,      .price = 50,         .UIIcon = SID(4,20),  .spriteX = 2,  .spriteY = 11},
  {.subType = kTunnelIn,  .unlock = 25,      .price = 100,        .UIIcon = SID(8,17), .spriteX = 8,  .spriteY = 11},
  {.subType = kTunnelOut, .unlock = UINT32_MAX, .price = UINT16_MAX, .UIIcon = SID(8,16), .spriteX = 9,  .spriteY = 11},
  {.subType = kFilterL,   .unlock = 30,      .price = 75,         .UIIcon = SID(8,20),  .spriteX = 3,  .spriteY = 11},
  {.subType = kFilterI,   .unlock = 35,      .price = 75,         .UIIcon = SID(12,20), .spriteX = 10, .spriteY = 11},
};

const struct PlantDescriptor_t PDesc[] = {
  {.subType = kCarrotPlant, .unlock = 1, .price = 0, .sprite = SID(10, 8), .time = 50, .wetness = kMoist, .soil = kSiltyGround, .out = kCarrot},
  {.subType = kPotatoPlant, .unlock = 2, .price = 0, .sprite = SID(14, 8), .time = 50, .wetness = kWet, .soil = kPeatyGround, .out = kPotato},
  {.subType = kSunflowerPlant, .unlock = 3, .price = 0, .sprite = SID(15, 8), .time = 50, .wetness = kDry, .soil = kChalkyGround, .out = kSunflower},
  {.subType = kAppleTree, .unlock = 36, .price = 100, .sprite = SID(8, 8), .time = 50, .wetness = kMoist, .soil = kPeatyGround, .out = kApple},
  {.subType = kCornPlant, .unlock = 72, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kChalkyGround, .out = kCorn},
  {.subType = kSeaweedPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kLake, .out = kSeaweed},
  {.subType = kWheatPlant, .unlock = 37, .price = 100, .sprite = SID(11, 8), .time = 50, .wetness = kDry, .soil = kSiltyGround, .out = kWheat},
  {.subType = kCactusPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kLimeTree, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kStrawberryPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kClayGround, .out = kStrawberry},
  {.subType = kHempPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kClayGround, .out = kHemp},
  {.subType = kCoffeePlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kLoamyGround, .out = kCoffeeBean},
};

const struct UtilityDescriptor_t UDesc[] = {
  {.subType = kPath, .unlock = 38, .price = 100, .UIIcon = SID(2,15), .sprite = SID(0,0)},
  {.subType = kBin, .unlock = 38, .price = 100, .UIIcon = SID(2,15), .sprite = SID(14,14)},
  {.subType = kWell, .unlock = 39, .price = 1000, .UIIcon = SID(0,15), .sprite = SID(12,14)},
  {.subType = kStorageBox, .unlock = 40, .price = 1, .UIIcon = SID(8,15), .sprite = SID(8,15)},
  {.subType = kBuffferBox, .unlock = 40, .price = 1, .UIIcon = SID(4,15), .sprite = SID(4,15)},
  {.subType = kConveyorGrease, .unlock = 41, .price = 100, .UIIcon = SID(1,15), .sprite = SID(1,15)},
  {.subType = kLandfill, .unlock = 665, .price = 100, .UIIcon = SID(3,15), .sprite = SID(0,3)},
};

const struct ExtractorDescriptor_t EDesc[] = {
  {.subType = kCropHarvesterSmall, .unlock = 42, .price = 50, .UIIcon = SID(4,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kChalkQuarry, .unlock = 44, .price = 75, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kChalk},
  {.subType = kSaltMine, .unlock = 55, .price = 10, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kSalt},
  {.subType = kPump, .unlock = 43, .price = 100, .UIIcon = SID(12,11), .sprite = BID(0,3), .out = kWaterBarrel},
  {.subType = kCropHarvesterLarge, .unlock = 45, .price = 500, .UIIcon = SID(8,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kCO2Extractor, .unlock = 45, .price = 500, .UIIcon = SID(2,4), .sprite = BID(0,3), .out = kCO2},
};

const struct FactoryDescriptor_t FDesc[] = {
  {.subType = kVitiminFac, .unlock = 46, .price = 100, .time = 6, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kChalk, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kVegOilFac, .unlock = 56, .price = 1, .time = 2, .sprite = BID(0,4), .out = kOil, .in1 = kSunflower, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCrispsFac, .unlock = 57, .price = 1, .time = 2, .sprite = BID(0,4), .out = kCrisps, .in1 = kOil, .in2 = kPotato, .in3 = kSalt, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEthanolFac, .unlock = 78, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHardCiderFac, .unlock = 79, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kAbattoir, .unlock = 80, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHydrogenFac, .unlock = 81, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHOILFac, .unlock = 82, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCornDogFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBeerFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kTequilaFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMexBeerFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEmulsifierFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHFCSFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kIceCreamFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBatteryFarm, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kProteinFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kGelatinFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPieFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMSGFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kReadyMealFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kJellyFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCaffeineFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEnergyDrinkFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kRaveJuiceFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPerkPillFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSoftCakeFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDesertFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPackagingFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCateringKitFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDrinksKitFac, .unlock = 33, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
};