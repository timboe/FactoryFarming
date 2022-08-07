#include "building.h"
#include "sprite.h"

#define DIS UINT32_MAX

// Note: User can never themselves build a kTunnelOut
struct ConveyorDescriptor_t CDesc[] = {
  {.subType = kBelt,      .unlock = DIS,       .price = 10,         .UIIcon = SID(0,17), .spriteX = 0,  .spriteY = 7},
  {.subType = kSplitL,    .unlock = DIS,      .price = 50,         .UIIcon = SID(4,17), .spriteX = 1,  .spriteY = 11},
  {.subType = kSplitI,    .unlock = DIS,      .price = 50,         .UIIcon = SID(0,20),  .spriteX = 0,  .spriteY = 11},
  {.subType = kSplitT,    .unlock = DIS,      .price = 50,         .UIIcon = SID(4,20),  .spriteX = 2,  .spriteY = 11},
  {.subType = kTunnelIn,  .unlock = DIS,      .price = 100,        .UIIcon = SID(8,17), .spriteX = 8,  .spriteY = 11},
  {.subType = kTunnelOut, .unlock = UINT32_MAX, .price = UINT16_MAX, .UIIcon = SID(8,16), .spriteX = 9,  .spriteY = 11},
  {.subType = kFilterL,   .unlock = DIS,      .price = 75,         .UIIcon = SID(8,20),  .spriteX = 3,  .spriteY = 11},
  {.subType = kFilterI,   .unlock = DIS,      .price = 75,         .UIIcon = SID(12,20), .spriteX = 10, .spriteY = 11},
};

struct PlantDescriptor_t PDesc[] = {
  {.subType = kCarrotPlant, .unlock = DIS, .price = 0, .sprite = SID(10, 8), .time = 50, .wetness = kMoist, .soil = kSiltyGround, .out = kCarrot},
  {.subType = kPotatoPlant, .unlock = DIS, .price = 0, .sprite = SID(14, 8), .time = 50, .wetness = kWet, .soil = kPeatyGround, .out = kPotato},
  {.subType = kSunflowerPlant, .unlock = DIS, .price = 0, .sprite = SID(15, 8), .time = 50, .wetness = kDry, .soil = kChalkyGround, .out = kSunflower},
  {.subType = kAppleTree, .unlock = DIS, .price = 100, .sprite = SID(8, 8), .time = 50, .wetness = kMoist, .soil = kPeatyGround, .out = kApple},
  {.subType = kCornPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kChalkyGround, .out = kCorn},
  {.subType = kSeaweedPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kWet, .soil = kLake, .out = kSeaweed},
  {.subType = kWheatPlant, .unlock = DIS, .price = 100, .sprite = SID(11, 8), .time = 50, .wetness = kDry, .soil = kSiltyGround, .out = kWheat},
  {.subType = kCactusPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kLimeTree, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kStrawberryPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kClayGround, .out = kStrawberry},
  {.subType = kHempPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kClayGround, .out = kHemp},
  {.subType = kCoffeePlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kLoamyGround, .out = kCoffeeBean},
  {.subType = kCocoPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kMoist, .soil = kLoamyGround, .out = kCocoBean},
  {.subType = kSeaCucumberPlant, .unlock = DIS, .price = 1, .sprite = SID(2, 4), .time = 50, .wetness = kWet, .soil = kOcean, .out = kSeaCucumber},
};

struct UtilityDescriptor_t UDesc[] = {
  {.subType = kPath, .unlock = DIS, .price = 100, .UIIcon = SID(9,19), .sprite = SID(0,0)},
  {.subType = kSign, .unlock = DIS, .price = 100, .UIIcon = SID(9,15), .sprite = SID(7,19)},
  {.subType = kBin, .unlock = DIS, .price = 100, .UIIcon = SID(2,15), .sprite = SID(14,14)},
  {.subType = kWell, .unlock = DIS, .price = 1000, .UIIcon = SID(0,15), .sprite = SID(12,14)},
  {.subType = kStorageBox, .unlock = DIS, .price = 1, .UIIcon = SID(8,15), .sprite = SID(8,15)},
  {.subType = kBuffferBox, .unlock = DIS, .price = 1, .UIIcon = SID(4,15), .sprite = SID(4,15)},
  {.subType = kConveyorGrease, .unlock = 0, .price = 100, .UIIcon = SID(1,15), .sprite = SID(1,15)},
  {.subType = kObstructionRemover, .unlock = 0, .price = 100, .UIIcon = SID(11,15), .sprite = SID(11,15)},
  {.subType = kLandfill, .unlock = DIS, .price = 100, .UIIcon = SID(3,15), .sprite = SID(0,3)},
  {.subType = kRetirement, .unlock = DIS, .price = 100, .UIIcon = SID(10,15), .sprite = BID(4,1)},
  {.subType = kFence, .unlock = DIS, .price = 100, .UIIcon = SID(9,22), .sprite = BID(0,0)},
};

struct ExtractorDescriptor_t EDesc[] = {
  {.subType = kCropHarvesterSmall, .unlock = DIS, .price = 50, .UIIcon = SID(4,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kChalkQuarry, .unlock = DIS, .price = 75, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kChalk},
  {.subType = kSaltMine, .unlock = DIS, .price = 10, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kSalt},
  {.subType = kPump, .unlock = DIS, .price = 100, .UIIcon = SID(12,11), .sprite = BID(0,3), .out = kWaterBarrel},
  {.subType = kCropHarvesterLarge, .unlock = DIS, .price = 500, .UIIcon = SID(8,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kCO2Extractor, .unlock = DIS, .price = 500, .UIIcon = SID(2,4), .sprite = BID(0,3), .out = kCO2},
};

struct FactoryDescriptor_t FDesc[] = {
  {.subType = kVitiminFac, .unlock = DIS, .price = 100, .time = 6, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kChalk, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kVegOilFac, .unlock = DIS, .price = 1, .time = 2, .sprite = BID(0,4), .out = kOil, .in1 = kSunflower, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCrispsFac, .unlock = DIS, .price = 1, .time = 2, .sprite = BID(0,4), .out = kCrisps, .in1 = kOil, .in2 = kPotato, .in3 = kSalt, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEthanolFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHardCiderFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kAbattoir, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHydrogenFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHOILFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCornDogFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBeerFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kTequilaFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMexBeerFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEmulsifierFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHFCSFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kIceCreamFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBatteryFarm, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kProteinFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kGelatinFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPieFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMSGFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kReadyMealFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kJellyFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCaffeineFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEnergyDrinkFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kRaveJuiceFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPerkPillFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSoftCakeFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDesertFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPackagingFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCateringKitFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDrinksKitFac, .unlock = DIS, .price = 1, .time = 1, .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
};

struct UnlockDescriptor_t UnlockDecs[] = {
  {.type = kNoBuilding, .subType.raw = 0, .fromSelling = 0, .ofCargo = kNoCargo}, // Dummt entry 1st, such that we get to unlock Carrots
  {.type = kPlant, .subType.plant = kCarrotPlant, .fromSelling = 0, .ofCargo = kCarrot},
  {.type = kExtractor, .subType.extractor = kCropHarvesterSmall, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kConveyor, .subType.conveyor = kBelt, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kExtractor, .subType.extractor = kChalkQuarry, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kFactory, .subType.factory = kVitiminFac, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kPlant, .subType.plant = kSunflowerPlant, .fromSelling = 1, .ofCargo = kVitamin},
  {.type = kPlant, .subType.plant = kPotatoPlant, .fromSelling = 20, .ofCargo = kSunflower},
  {.type = kExtractor, .subType.extractor = kSaltMine, .fromSelling = 30, .ofCargo = kPotato},
  {.type = kFactory, .subType.factory = kCrispsFac, .fromSelling = 30, .ofCargo = kPotato},
  {.type = kUtility, .subType.utility = kRetirement, .fromSelling = 30, .ofCargo = kCrisps}, // Must always be the last entry
};
