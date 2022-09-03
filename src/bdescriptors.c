#include "building.h"
#include "sprite.h"

#define DIS UINT16_MAX

// Note: User can never themselves build a kTunnelOut
struct ConveyorDescriptor_t CDesc[] = {
  {.subType = kBelt,       .unlock = 1,   .price = 10,         .UIIcon = SID(0,17), .spriteX = 0, .spriteY = 7},
  {.subType = kSplitL,     .unlock = 1,   .price = 50,         .UIIcon = SID(4,17), .spriteX = 1, .spriteY = 11},
  {.subType = kSplitI,     .unlock = 1,   .price = 50,         .UIIcon = SID(0,20), .spriteX = 0, .spriteY = 11},
  {.subType = kSplitT,     .unlock = 1,   .price = 50,         .UIIcon = SID(4,20), .spriteX = 2, .spriteY = 11},
  {.subType = kSpareConv0, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kSpareConv1, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kTunnelIn,   .unlock = 1,   .price = 1000,       .UIIcon = SID(8,17), .spriteX = 8, .spriteY = 11},
  {.subType = kTunnelOut,  .unlock = DIS, .price = UINT32_MAX, .UIIcon = SID(9,10), .spriteX = 9, .spriteY = 11},
  {.subType = kSpareConv2, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kSpareConv3, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kFilterL,    .unlock = 1,   .price = 250,        .UIIcon = SID(8,20), .spriteX = 3, .spriteY = 11},
  {.subType = kFilterI,    .unlock = 1,   .price = 250,        .UIIcon = SID(12,20), .spriteX = 10, .spriteY = 11},
  {.subType = kSpareConv4, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kSpareConv5, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kSpareConv6, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
  {.subType = kSpareConv7, .unlock = DIS, .price = 1,          .UIIcon = SID(0,0),  .spriteX = 0, .spriteY = 0},
};

#define PVFAST 60
#define PFAST 70
#define PMED 80
#define PSLOW 90
#define PVSLOW 90
struct PlantDescriptor_t PDesc[] = {
  {.subType = kCarrotPlant, .unlock = 1, .price = 5, .multi = 1.3f, .sprite = SID(10, 8), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kCarrot},
  {.subType = kPotatoPlant, .unlock = 1, .price = 10, .multi = 1.3f, .sprite = SID(14, 8), .time = PSLOW, .wetness = kWet, .soil = kPeatyGround, .out = kPotato},
  {.subType = kSparePlant0, .unlock = DIS, .price = 1, .multi = 1.3f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant1, .unlock = DIS, .price = 1, .multi = 1.3f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSunflowerPlant, .unlock = 1, .price = 50, .multi = 1.3f, .sprite = SID(15, 8), .time = PFAST, .wetness = kDry, .soil = kChalkyGround, .out = kSunflower},
  {.subType = kAppleTree, .unlock = 1, .price = 100, .multi = 1.3f, .sprite = SID(8, 8), .time = PVSLOW, .wetness = kMoist, .soil = kPeatyGround, .out = kApple},
  {.subType = kSeaweedPlant, .unlock = 1, .price = 200, .multi = 1.3f, .sprite = SID(3, 21), .time = PFAST, .wetness = kWater, .soil = kLake, .out = kSeaweed},
  {.subType = kSparePlant2, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant3, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},  
  {.subType = kCornPlant, .unlock = 1, .price = 150, .multi = 1.3f, .sprite = SID(2, 21), .time = PFAST, .wetness = kMoist, .soil = kChalkyGround, .out = kCorn},
  {.subType = kWheatPlant, .unlock = 1, .price = 300, .multi = 1.3f, .sprite = SID(11, 8), .time = PFAST, .wetness = kDry, .soil = kSiltyGround, .out = kWheat},
  {.subType = kSparePlant4, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant5, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kCactusPlant, .unlock = 1, .price = 500, .multi = 1.3f, .sprite = SID(0, 21), .time = PSLOW, .wetness = kDry, .soil = kSandyGround, .out = kCactus},
  {.subType = kLimeTree, .unlock = 1, .price = 1000, .multi = 1.3f, .sprite = SID(4, 21), .time = PVSLOW, .wetness = kMoist, .soil = kSandyGround, .out = kLime},
  {.subType = kSparePlant6, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant7, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kStrawberryPlant, .unlock = 1, .price = 1200, .multi = 1.3f, .sprite = SID(5, 21), .time = PMED, .wetness = kMoist, .soil = kClayGround, .out = kStrawberry},
  {.subType = kCocoaPlant, .unlock = 1, .price = 2500, .multi = 1.3f, .sprite = SID(7, 21), .time = PMED, .wetness = kWet, .soil = kClayGround, .out = kCocoBean},
  {.subType = kSparePlant8, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant9, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kCoffeePlant, .unlock = 1, .price = 3000, .multi = 1.3f, .sprite = SID(6, 21), .time = PSLOW, .wetness = kMoist, .soil = kLoamyGround, .out = kCoffeeBean},
  {.subType = kBambooPlant, .unlock = 1, .price = 4000, .multi = 1.3f, .sprite = SID(5, 23), .time = PVFAST, .wetness = kMoist, .soil = kLoamyGround, .out = kBamboo},
  {.subType = kSparePlant10, .unlock = DIS, .price = 1, .multi = 1.3f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant11, .unlock = DIS, .price = 1, .multi = 1.3f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},  
  {.subType = kSeaCucumberPlant, .unlock = 1, .price = 7500, .multi = 1.3f, .sprite = SID(8, 21), .time = PMED, .wetness = kWater, .soil = kOcean, .out = kSeaCucumber},
  {.subType = kRosePlant, .unlock = 1, .price = 10000, .multi = 1.3f, .sprite = SID(1, 25), .time = PMED, .wetness = kDry, .soil = kLoamyGround, .out = kRose},
  {.subType = kSparePlant12, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant13, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant14, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
  {.subType = kSparePlant15, .unlock = DIS, .price = 1, .multi = 1.0f, .sprite = SID(0, 0), .time = PMED, .wetness = kMoist, .soil = kSiltyGround, .out = kNoCargo},
};

struct UtilityDescriptor_t UDesc[] = {
  {.subType = kPath, .unlock = 1, .price = 100, .UIIcon = SID(9,19), .sprite = SID(0,0)},
  {.subType = kSign, .unlock = 1, .price = 250, .UIIcon = SID(9,15), .sprite = SID(7,19)},
  {.subType = kSpareUtil0, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kSpareUtil1, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kBin, .unlock = 1, .price = 1000, .UIIcon = SID(2,15), .sprite = SID(14,14)},
  {.subType = kWell, .unlock = 1, .price = 2500, .UIIcon = SID(0,15), .sprite = SID(12,14)},
  {.subType = kStorageBox, .unlock = 1, .price = 10000, .UIIcon = SID(8,15), .sprite = SID(8,15)},
  {.subType = kBuffferBox, .unlock = 1, .price = 15000, .UIIcon = SID(4,15), .sprite = SID(4,15)},
  {.subType = kSpareUtil2, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kSpareUtil3, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kConveyorGrease, .unlock = 1, .price = 1000, .UIIcon = SID(1,15), .sprite = SID(1,15)},
  {.subType = kObstructionRemover, .unlock = 1, .price = 100000, .UIIcon = SID(11,15), .sprite = SID(11,15)},
  {.subType = kLandfill, .unlock = 1, .price = 2500, .UIIcon = SID(3,15), .sprite = SID(0,3)},
  {.subType = kSpareUtil4, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kSpareUtil5, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kRetirement, .unlock = 1, .price = 1000000, .UIIcon = SID(10,15), .sprite = BID(4,1)},
  {.subType = kFence, .unlock = 1, .price = 5000, .UIIcon = SID(12,15), .sprite = BID(0,0)},
  {.subType = kSpareUtil6, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
  {.subType = kSpareUtil7, .unlock = DIS, .price = 1, .UIIcon = SID(0,0), .sprite = SID(0,0)},
};

struct ExtractorDescriptor_t EDesc[] = {
  {.subType = kCropHarvesterSmall, .invert = false, .unlock = 1, .price = 50, .UIIcon = SID(4,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kSpareExtractor0, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kChalkQuarry, .invert = false, .unlock = 1, .price = 75, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kChalk},
  {.subType = kSpareExtractor1, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kSaltMine, .invert = false, .unlock = 1, .price = 400, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kSalt},
  {.subType = kSpareExtractor2, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kPump, .invert = false, .unlock = 1, .price = 1000, .UIIcon = SID(12,11), .sprite = BID(0,3), .out = kWaterBarrel},
  {.subType = kSpareExtractor3, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kCropHarvesterLarge, .invert = true, .unlock = 1, .price = 20000, .UIIcon = SID(4,16), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kSpareExtractor4, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kCO2Extractor, .invert = true, .unlock = 1, .price = 30000, .UIIcon = SID(12,11), .sprite = BID(0,3), .out = kCO2},
  {.subType = kSpareExtractor5, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kSpareExtractor6, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
  {.subType = kSpareExtractor7, .invert = false, .unlock = DIS, .price = 0, .UIIcon = SID(0,0), .sprite = BID(0,0), .out = kNoCargo},
};

struct FactoryDescriptor_t FDesc[] = {
  {.subType = kVitiminFac, .unlock = 1, .invert = false, .price = 100, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kVitamin, .in1 = kCarrot, .in2 = kChalk, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kVegOilFac, .unlock = 1, .invert = false, .price = 250, .multi = 2.5f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kOil, .in1 = kSunflower, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac0, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac1, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCrispsFac, .unlock = 1, .invert = false, .price = 500, .multi = 2.0f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kCrisps, .in1 = kOil, .in2 = kPotato, .in3 = kSalt, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEthanolFac, .unlock = 1, .invert = false, .price = 750, .multi = 1.3f, .time = 3, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kEthanol, .in1 = kWaterBarrel, .in2 = kPotato, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHardCiderFac, .unlock = 1, .invert = false, .price = 1000, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kHardCider, .in1 = kEthanol, .in2 = kApple, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac2, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac3, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kAbattoir, .unlock = 1, .invert = false, .price = 2000, .multi = 2.0f, .time = 4, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kMeat, .in1 = kSunflower, .in2 = kSeaweed, .in3 = kWaterBarrel, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHydrogenFac, .unlock = 1, .invert = true, .price = 1500, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kHydrogen, .in1 = kWaterBarrel, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHOILFac, .unlock = 1, .invert = false, .price = 1750, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kHOil, .in1 = kOil, .in2 = kHydrogen, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCornDogFac, .unlock = 1, .invert = false, .price = 2500, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kCornDog, .in1 = kHOil, .in2 = kMeat, .in3 = kCorn, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kBeerFac, .unlock = 1, .invert = false, .price = 2000, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kBeer, .in1 = kEthanol, .in2 = kWaterBarrel, .in3 = kWheat, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac4, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac5, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kTequilaFac, .unlock = 1, .invert = false, .price = 3000, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kTequila, .in1 = kEthanol, .in2 = kCactus, .in3 = kWaterBarrel, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kHFCSFac, .unlock = 1, .invert = false, .price = 4000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kHFCS, .in1 = kCorn, .in2 = kWaterBarrel, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kGelatinFac, .unlock = 1, .invert = false, .price = 15000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kGelatin, .in1 = kMeat, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kJellyFac, .unlock = 1, .invert = false, .price = 30000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kJelly, .in1 = kGelatin, .in2 = kLime, .in3 = kHFCS, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kJelloShotFac, .unlock = 1, .invert = false, .price = 3500, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kJelloShot, .in1 = kJelly, .in2 = kTequila, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac6, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac7, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEmulsifierFac, .unlock = 1, .invert = false, .price = 2000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kEmulsifiers, .in1 = kSeaweed, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kIceCreamFac, .unlock = 1, .invert = false, .price = 10000, .multi = 1.3f, .time = 3, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kIceCream, .in1 = kStrawberry, .in2 = kHFCS, .in3 = kHOil, .in4 = kEmulsifiers, .in5 = kNoCargo},
  {.subType = kBatteryFarm, .unlock = 1, .invert = false, .price = 12000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,12), .sprite = BID(0,6), .out = kEggs, .in1 = kCorn, .in2 = kWaterBarrel, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kProteinFac, .unlock = 1, .invert = false, .price = 20000, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kProtein, .in1 = kEggs, .in2 = kSeaweed, .in3 = kChalk, .in4 = kVitamin, .in5 = kNoCargo},
  {.subType = kChocolateFac, .unlock = 1, .invert = false, .price = 22000, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kChocolate, .in1 = kCocoBean, .in2 = kHFCS, .in3 = kHOil, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPieFac, .unlock = 1, .invert = false, .price = 25000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kMeatPie, .in1 = kMeat, .in2 = kGelatin, .in3 = kWheat, .in4 = kPotato, .in5 = kNoCargo},
  {.subType = kMSGFac, .unlock = 1, .invert = false, .price = 20000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kMSG, .in1 = kWheat, .in2 = kSalt, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kTVDinnerFac, .unlock = 1, .invert = false, .price = 40000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kTVDinner, .in1 = kCarrot, .in2 = kMeatPie, .in3 = kMSG, .in4 = kEmulsifiers, .in5 = kNoCargo},
  {.subType = kCakeFac, .unlock = 1, .invert = false, .price = 30000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kCake, .in1 = kHFCS, .in2 = kEggs, .in3 = kWheat, .in4 = kChocolate, .in5 = kNoCargo},
  {.subType = kSpareFac8, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac9, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCaffeineFac, .unlock = 1, .invert = true, .price = 37500, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kCaffeine, .in1 = kCoffeeBean, .in2 = kCO2, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kEnergyDrinkFac, .unlock = 1, .invert = false, .price = 50000, .multi = 1.3f, .time = 2, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kEnergyDrink, .in1 = kLime, .in2 = kHFCS, .in3 = kCO2, .in4 = kCaffeine, .in5 = kNoCargo},
  {.subType = kRaveJuiceFac, .unlock = 1, .invert = false, .price = 70000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kRaveJuice, .in1 = kHardCider, .in2 = kEnergyDrink, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPerkPillFac, .unlock = 1, .invert = false, .price = 60000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kPerkPills, .in1 = kCaffeine, .in2 = kChalk, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kPackagingFac, .unlock = 1, .invert = true, .price = 50000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kPackaging, .in1 = kCactus, .in2 = kBamboo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kDessertFac, .unlock = 1, .invert = false, .price = 125000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kDessert, .in1 = kJelly, .in2 = kIceCream, .in3 = kApple, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kCateringKitFac, .unlock = 1, .invert = true, .price = 140000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kCateringKit, .in1 = kTVDinner, .in2 = kDessert, .in3 = kEnergyDrink, .in4 = kPackaging, .in5 = kNoCargo},
  {.subType = kPartyPackFac, .unlock = 1, .invert = true, .price = 160000, .multi = 1.3f, .time = 4, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kPartyPack, .in1 = kBeer, .in2 = kCake, .in3 = kJelloShot, .in4 = kRaveJuice, .in5 = kPackaging},
  {.subType = kSpareFac10, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac11, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kParfumeFac, .unlock = 1, .invert = false, .price = 170000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kParfume, .in1 = kSeaCucumber, .in2 = kStrawberry, .in3 = kEthanol, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kMiricalPowderFac, .unlock = 1, .invert = false, .price = 180000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kMiricalPowder, .in1 = kSeaCucumber, .in2 = kProtein, .in3 = kCocoBean, .in4 = kCoffeeBean, .in5 = kNoCargo},
  {.subType = kTurkishDelightFac, .unlock = 1, .invert = false, .price = 200000, .multi = 1.3f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,4), .out = kTurkishDelight, .in1 = kHFCS, .in2 = kRose, .in3 = kWaterBarrel, .in4 = kPotato, .in5 = kNoCargo},
  {.subType = kSpareFac12, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac13, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac14, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},
  {.subType = kSpareFac15, .unlock = DIS, .invert = false, .price = 0, .multi = 1.0f, .time = 1, .UIIcon = SID(12,10), .sprite = BID(0,0), .out = kNoCargo, .in1 = kNoCargo, .in2 = kNoCargo, .in3 = kNoCargo, .in4 = kNoCargo, .in5 = kNoCargo},

};

struct UnlockDescriptor_t UnlockDecs[] = {
  {.type = kNoBuilding, .subType.raw = 0, .fromSelling = 0, .ofCargo = kNoCargo}, // Dummt entry 1st, such that we get to unlock Carrots

  {.type = kPlant, .subType.plant = kCarrotPlant, .fromSelling = 0, .ofCargo = kCarrot}, // These are spaced out if the tutorial is active
  {.type = kExtractor, .subType.extractor = kCropHarvesterSmall, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kConveyor, .subType.conveyor = kBelt, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kExtractor, .subType.extractor = kChalkQuarry, .fromSelling = 10, .ofCargo = kCarrot},
  {.type = kFactory, .subType.factory = kVitiminFac, .fromSelling = 10, .ofCargo = kCarrot},

  {.type = kPlant, .subType.plant = kSunflowerPlant, .fromSelling = 5, .ofCargo = kVitamin},
  {.type = kConveyor, .subType.conveyor = kSplitL, .fromSelling = 5, .ofCargo = kVitamin},
  {.type = kConveyor, .subType.conveyor = kSplitI, .fromSelling = 5, .ofCargo = kVitamin},
  {.type = kConveyor, .subType.conveyor = kSplitT, .fromSelling = 5, .ofCargo = kVitamin},

  {.type = kPlant, .subType.plant = kPotatoPlant, .fromSelling = 20, .ofCargo = kSunflower},

  {.type = kFactory, .subType.factory = kVegOilFac, .fromSelling = 30, .ofCargo = kPotato},

  {.type = kExtractor, .subType.extractor = kSaltMine, .fromSelling = 40, .ofCargo = kOil},
  {.type = kFactory, .subType.factory = kCrispsFac, .fromSelling = 80, .ofCargo = kOil},

  {.type = kUtility, .subType.utility = kPath, .fromSelling = 50, .ofCargo = kCrisps},
  {.type = kExtractor, .subType.extractor = kPump, .fromSelling = 75, .ofCargo = kCrisps},
  {.type = kFactory, .subType.factory = kEthanolFac, .fromSelling = 100, .ofCargo = kCrisps},

  {.type = kPlant, .subType.plant = kAppleTree, .fromSelling = 60, .ofCargo = kEthanol},

  {.type = kFactory, .subType.factory = kHardCiderFac, .fromSelling = 25, .ofCargo = kApple},

  {.type = kConveyor, .subType.conveyor = kTunnelIn, .fromSelling = 70, .ofCargo = kHardCider},
  {.type = kPlant, .subType.plant = kSeaweedPlant, .fromSelling = 100, .ofCargo = kHardCider},

  {.type = kFactory, .subType.factory = kAbattoir, .fromSelling = 100, .ofCargo = kSeaweed},
  {.type = kFactory, .subType.factory = kHydrogenFac, .fromSelling = 200, .ofCargo = kSeaweed},

  {.type = kPlant, .subType.plant = kCornPlant, .fromSelling = 150, .ofCargo = kHydrogen},
  {.type = kFactory, .subType.factory = kHOILFac, .fromSelling = 200, .ofCargo = kHydrogen},
  {.type = kFactory, .subType.factory = kCornDogFac, .fromSelling = 250, .ofCargo = kHydrogen},

  {.type = kUtility, .subType.utility = kSign, .fromSelling = 50, .ofCargo = kCornDog},
  {.type = kPlant, .subType.plant = kWheatPlant, .fromSelling = 200, .ofCargo = kCornDog},

  {.type = kFactory, .subType.factory = kBeerFac, .fromSelling = 200, .ofCargo = kWheat},

  {.type = kUtility, .subType.utility = kBin, .fromSelling = 50, .ofCargo = kBeer},
  {.type = kConveyor, .subType.conveyor = kFilterL, .fromSelling = 100, .ofCargo = kBeer},
  {.type = kConveyor, .subType.conveyor = kFilterI, .fromSelling = 100, .ofCargo = kBeer},
  {.type = kPlant, .subType.plant = kCactusPlant, .fromSelling = 200, .ofCargo = kBeer}, // Unlocks tutorial 2

  {.type = kFactory, .subType.factory = kTequilaFac, .fromSelling = 10, .ofCargo = kCactus},

  {.type = kUtility, .subType.utility = kWell, .fromSelling = 100, .ofCargo = kTequila},
  {.type = kPlant, .subType.plant = kLimeTree, .fromSelling = 200, .ofCargo = kTequila},
  {.type = kFactory, .subType.factory = kHFCSFac, .fromSelling = 300, .ofCargo = kTequila},
  {.type = kFactory, .subType.factory = kGelatinFac, .fromSelling = 400, .ofCargo = kTequila},
  {.type = kFactory, .subType.factory = kJellyFac, .fromSelling = 500, .ofCargo = kTequila},

  {.type = kFactory, .subType.factory = kJelloShotFac, .fromSelling = 200, .ofCargo = kJelly},

  {.type = kPlant, .subType.plant = kStrawberryPlant, .fromSelling = 200, .ofCargo = kJelloShot},
  {.type = kExtractor, .subType.extractor = kCropHarvesterLarge, .fromSelling = 400, .ofCargo = kJelloShot},

  {.type = kUtility, .subType.utility = kStorageBox, .fromSelling = 100, .ofCargo = kStrawberry},
  {.type = kUtility, .subType.utility = kBuffferBox, .fromSelling = 100, .ofCargo = kStrawberry},
  {.type = kFactory, .subType.factory = kEmulsifierFac, .fromSelling = 200, .ofCargo = kStrawberry},
  {.type = kFactory, .subType.factory = kIceCreamFac, .fromSelling = 300, .ofCargo = kStrawberry},

  {.type = kUtility, .subType.utility = kConveyorGrease, .fromSelling = 250, .ofCargo = kIceCream},
  {.type = kFactory, .subType.factory = kBatteryFarm, .fromSelling = 500, .ofCargo = kIceCream},
  {.type = kFactory, .subType.factory = kProteinFac, .fromSelling = 750, .ofCargo = kIceCream},

  {.type = kPlant, .subType.plant = kCocoaPlant, .fromSelling = 150, .ofCargo = kProtein}, 
  {.type = kFactory, .subType.factory = kChocolateFac, .fromSelling = 500, .ofCargo = kProtein}, 

  {.type = kFactory, .subType.factory = kPieFac, .fromSelling = 250, .ofCargo = kChocolate},

  {.type = kUtility, .subType.utility = kObstructionRemover, .fromSelling = 400, .ofCargo = kMeatPie},
  {.type = kFactory, .subType.factory = kMSGFac, .fromSelling = 800, .ofCargo = kMeatPie},
  {.type = kFactory, .subType.factory = kTVDinnerFac, .fromSelling = 1200, .ofCargo = kMeatPie},

  {.type = kFactory, .subType.factory = kCakeFac, .fromSelling = 500, .ofCargo = kTVDinner},

  {.type = kPlant, .subType.plant = kCoffeePlant, .fromSelling = 250, .ofCargo = kCake},
  {.type = kExtractor, .subType.extractor = kCO2Extractor, .fromSelling = 350, .ofCargo = kCake},
  {.type = kFactory, .subType.factory = kCaffeineFac, .fromSelling = 450, .ofCargo = kCake},
  {.type = kFactory, .subType.factory = kEnergyDrinkFac, .fromSelling = 550, .ofCargo = kCake},

  {.type = kFactory, .subType.factory = kRaveJuiceFac, .fromSelling = 500, .ofCargo = kEnergyDrink},

  {.type = kUtility, .subType.utility = kLandfill, .fromSelling = 250, .ofCargo = kRaveJuice},
  {.type = kFactory, .subType.factory = kPerkPillFac, .fromSelling = 500, .ofCargo = kRaveJuice},

  {.type = kPlant, .subType.plant = kBambooPlant, .fromSelling = 200, .ofCargo = kPerkPills},
  {.type = kFactory, .subType.factory = kPackagingFac, .fromSelling = 400, .ofCargo = kPerkPills},
  {.type = kFactory, .subType.factory = kDessertFac, .fromSelling = 600, .ofCargo = kPerkPills},
  {.type = kFactory, .subType.factory = kCateringKitFac, .fromSelling = 800, .ofCargo = kPerkPills},

  // Note: Party Packs are used to unlock World 7
  {.type = kFactory, .subType.factory = kPartyPackFac, .fromSelling = 500, .ofCargo = kCateringKit},
  {.type = kPlant, .subType.plant = kSeaCucumberPlant, .fromSelling = 600, .ofCargo = kCateringKit},

  {.type = kFactory, .subType.factory = kParfumeFac, .fromSelling = 100, .ofCargo = kSeaCucumber},

  {.type = kUtility, .subType.utility = kRetirement, .fromSelling = 500, .ofCargo = kParfume},
  // Note: Health Supliment are used to unlock World 8
  {.type = kFactory, .subType.factory = kMiricalPowderFac, .fromSelling = 700, .ofCargo = kParfume},

  // Note: The Player gets given some rose bushes by placing the retirement home.
  // Gates the final two unlocks
  {.type = kPlant, .subType.plant = kRosePlant, .fromSelling = 5, .ofCargo = kRose},
  {.type = kUtility, .subType.utility = kFence, .fromSelling = 200, .ofCargo = kRose},
  {.type = kFactory, .subType.factory = kTurkishDelightFac, .fromSelling = 400, .ofCargo = kRose},

  // Esnure the final entry here is listed as FINAL_UNLOCK_TYPE, FINAL_UNLOCK_SUBTYPE in blists.h
};
