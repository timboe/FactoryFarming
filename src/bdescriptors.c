#include "building.h"
#include "sprite.h"

#define DIS UINT32_MAX

// Note: User can never themselves build a kTunnelOut
struct ConveyorDescriptor_t CDesc[] = {
  {.subType = kBelt,      .unlock = 5,       .price = 10,         .UIIcon = SID(0,17), .spriteX = 0,  .spriteY = 7},
  {.subType = kSplitL,    .unlock = UINT32_MAX,      .price = 50,         .UIIcon = SID(4,17), .spriteX = 1,  .spriteY = 11},
  {.subType = kSplitI,    .unlock = UINT32_MAX,      .price = 50,         .UIIcon = SID(0,20),  .spriteX = 0,  .spriteY = 11},
  {.subType = kSplitT,    .unlock = UINT32_MAX,      .price = 50,         .UIIcon = SID(4,20),  .spriteX = 2,  .spriteY = 11},
  {.subType = kTunnelIn,  .unlock = UINT32_MAX,      .price = 100,        .UIIcon = SID(8,17), .spriteX = 8,  .spriteY = 11},
  {.subType = kTunnelOut, .unlock = UINT32_MAX, .price = UINT16_MAX, .UIIcon = SID(8,16), .spriteX = 9,  .spriteY = 11},
  {.subType = kFilterL,   .unlock = UINT32_MAX,      .price = 75,         .UIIcon = SID(8,20),  .spriteX = 3,  .spriteY = 11},
  {.subType = kFilterI,   .unlock = UINT32_MAX,      .price = 75,         .UIIcon = SID(12,20), .spriteX = 10, .spriteY = 11},
};

struct PlantDescriptor_t PDesc[] = {
  {.subType = kCarrotPlant, .unlock = UINT32_MAX, .price = 0, .sprite = SID(10, 8), .time = 50, .wetness = kMoist, .soil = kSiltyGround, .out = kCarrot},
  {.subType = kPotatoPlant, .unlock = UINT32_MAX, .price = 0, .sprite = SID(14, 8), .time = 50, .wetness = kWet, .soil = kPeatyGround, .out = kPotato},
  {.subType = kSunflowerPlant, .unlock = UINT32_MAX, .price = 0, .sprite = SID(15, 8), .time = 50, .wetness = kDry, .soil = kChalkyGround, .out = kSunflower},
  {.subType = kAppleTree, .unlock = UINT32_MAX, .price = 100, .sprite = SID(8, 8), .time = 50, .wetness = kMoist, .soil = kPeatyGround, .out = kApple},
  {.subType = kCornPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kChalkyGround, .out = kCorn},
  {.subType = kSeaweedPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kWet, .soil = kLake, .out = kSeaweed},
  {.subType = kWheatPlant, .unlock = UINT32_MAX, .price = 100, .sprite = SID(11, 8), .time = 50, .wetness = kDry, .soil = kSiltyGround, .out = kWheat},
  {.subType = kCactusPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kLimeTree, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kStrawberryPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kClayGround, .out = kStrawberry},
  {.subType = kHempPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kClayGround, .out = kHemp},
  {.subType = kCoffeePlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kLoamyGround, .out = kCoffeeBean},
  {.subType = kCocoPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kLoamyGround, .out = kCocoBean},
  {.subType = kSeaCucumberPlant, .unlock = UINT32_MAX, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kWet, .soil = kOcean, .out = kSeaCucumber},
};

struct UtilityDescriptor_t UDesc[] = {
  {.subType = kPath, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(9,19), .sprite = SID(0,0)},
  {.subType = kSign, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(9,15), .sprite = SID(7,19)},
  {.subType = kBin, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(2,15), .sprite = SID(14,14)},
  {.subType = kWell, .unlock = UINT32_MAX, .price = 1000, .UIIcon = SID(0,15), .sprite = SID(12,14)},
  {.subType = kStorageBox, .unlock = UINT32_MAX, .price = 1, .UIIcon = SID(8,15), .sprite = SID(8,15)},
  {.subType = kBuffferBox, .unlock = UINT32_MAX, .price = 1, .UIIcon = SID(4,15), .sprite = SID(4,15)},
  {.subType = kConveyorGrease, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(1,15), .sprite = SID(1,15)},
  {.subType = kLandfill, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(3,15), .sprite = SID(0,3)},
  {.subType = kRetirement, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(10,15), .sprite = BID(4,1)},
  {.subType = kFence, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(9,22), .sprite = BID(0,0)},
};

struct ExtractorDescriptor_t EDesc[] = {
  {.subType = kCropHarvesterSmall, .unlock = UINT32_MAX, .price = 50, .UIIcon = SID(4,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kChalkQuarry, .unlock = UINT32_MAX, .price = 75, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kChalk},
  {.subType = kSaltMine, .unlock = UINT32_MAX, .price = 10, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kSalt},
  {.subType = kPump, .unlock = UINT32_MAX, .price = 100, .UIIcon = SID(12,11), .sprite = BID(0,3), .out = kWaterBarrel},
  {.subType = kCropHarvesterLarge, .unlock = UINT32_MAX, .price = 500, .UIIcon = SID(8,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kCO2Extractor, .unlock = UINT32_MAX, .price = 500, .UIIcon = SID(2,4), .sprite = BID(0,3), .out = kCO2},
};

struct FactoryDescriptor_t FDesc[] = {
  {.subType = kVitiminFac, .unlock = UINT32_MAX, .price = 100, .time = 6, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kChalk, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kVegOilFac, .unlock = UINT32_MAX, .price = 1, .time = 2, .sprite = BID(0,4), .out = kOil, .in1 = kSunflower, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCrispsFac, .unlock = UINT32_MAX, .price = 1, .time = 2, .sprite = BID(0,4), .out = kCrisps, .in1 = kOil, .in2 = kPotato, .in3 = kSalt, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEthanolFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHardCiderFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kAbattoir, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHydrogenFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHOILFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCornDogFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBeerFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kTequilaFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMexBeerFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEmulsifierFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHFCSFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kIceCreamFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBatteryFarm, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kProteinFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kGelatinFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPieFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMSGFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kReadyMealFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kJellyFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCaffeineFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEnergyDrinkFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kRaveJuiceFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPerkPillFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSoftCakeFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDesertFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPackagingFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCateringKitFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDrinksKitFac, .unlock = UINT32_MAX, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
};

struct UnlockDescriptor_t UnlockDecs[] = {
  {.type = kNoBuilding, .subType.raw = 0, .fromSelling = 0, .of = kNoCargo}, // Dummt entry 1st, such that we get to unlock Carrots
  {.type = kPlant, .subType.plant = kCarrotPlant, .fromSelling = 0, .of = kCarrot},
  {.type = kExtractor, .subType.extractor = kCropHarvesterSmall, .fromSelling = 10, .of = kCarrot},
  {.type = kConveyor, .subType.conveyor = kBelt, .fromSelling = 10, .of = kCarrot},
  {.type = kExtractor, .subType.extractor = kChalkQuarry, .fromSelling = 10, .of = kCarrot},
  {.type = kFactory, .subType.factory = kVitiminFac, .fromSelling = 10, .of = kCarrot},
  {.type = kPlant, .subType.plant = kSunflowerPlant, .fromSelling = 1, .of = kVitamin},
  {.type = kPlant, .subType.plant = kPotatoPlant, .fromSelling = 20, .of = kSunflower},
  {.type = kExtractor, .subType.extractor = kSaltMine, .fromSelling = 30, .of = kPotato},
  {.type = kFactory, .subType.factory = kCrispsFac, .fromSelling = 30, .of = kPotato},
  {.type = kUtility, .subType.utility = kRetirement, .fromSelling = 30, .of = kCrisps}, // Must always be the last entry
};
