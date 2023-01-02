#include "building.h"

void setPrices() {

  #define DIS UINT16_MAX

  // Extractor output prices
  CargoDesc[kChalk].price = 1;
  CargoDesc[kSalt].price = 1;
  CargoDesc[kWaterBarrel].price = 2;
  CargoDesc[kCO2].price = 20;

  // Note: Plants must be done first, as their prices will then help to set factory output prices
  CargoDesc[kCarrot].price = 1;
  PDesc[kCarrotPlant].price = 1; // Careful here, player must not be able to go broke
  uint8_t skip = 1;
  for (int32_t i = 1; i < kNPlantSubTypes; ++i) {
    // Skip placeholders
    if (PDesc[i].price == DIS) {
      ++skip;
      continue;
    }

    PDesc[i].price = PDesc[i-skip].price * PDesc[i].multi; 
    CargoDesc[ PDesc[i].out ].price = CargoDesc[ PDesc[i-skip].out ].price * CargoDesc[ PDesc[i].out ].multi;

    #ifdef DEV
    pd->system->logToConsole("Plant: %s will cost %i * %.2f = %i, its output %s will cost %i * %.2f = %i",
      toStringBuilding(kPlant, (union kSubType) {.plant = i}, false),
      PDesc[i-skip].price,
      (double) PDesc[i].multi,
      PDesc[i].price,
      toStringCargoByType(PDesc[i].out, false),
      CargoDesc[ PDesc[i-skip].out ].price,
      (double) CargoDesc[ PDesc[i].out ].multi,
      CargoDesc[ PDesc[i].out ].price);
    #endif

    skip = 1;
  }

  skip = 1;
  FDesc[kVitiminFac].price = 100;
  CargoDesc[kVitamin].price = 8;
  for (int32_t i = 1; i < kNFactorySubTypes; ++i) {
    // Skip placeholders
    if (FDesc[i].price == DIS) { 
      ++skip;
      continue;
    }

    FDesc[i].price = FDesc[i-skip].price * FDesc[i].multi;
    // Get inputs prices
    uint32_t inputsPrice = CargoDesc[ FDesc[i].in1 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in2 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in3 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in4 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in5 ].price;

    CargoDesc[ FDesc[i].out ].price = inputsPrice * CargoDesc[ FDesc[i].out ].multi;  

    #ifdef DEV

    // Find who unlocks me
    enum kCargoType unlockedWith = kNoCargo;
    int16_t unlockAmount = 0;
    for (int32_t j = 0; j < getNUnlocks(); ++j) {
      if (UnlockDecs[j].type == kFactory && UnlockDecs[j].subType.factory == i) {
        unlockAmount = UnlockDecs[j].fromSelling;
        unlockedWith = UnlockDecs[j].ofCargo;
        break;
      }
    }

    pd->system->logToConsole("Factory: %s will cost %i * %.2f = %i, its output %s will cost %i * %.2f = %i. "
      "Need to sell %i of %s to unlock, and to sell %.2f at %i each to afford",
      toStringBuilding(kFactory, (union kSubType) {.factory = i}, false),
      FDesc[i-skip].price,
      (double) FDesc[i].multi,
      FDesc[i].price,
      //
      toStringCargoByType(FDesc[i].out, false),
      inputsPrice,
      (double) CargoDesc[ FDesc[i].out ].multi,
      CargoDesc[ FDesc[i].out ].price,
      //
      unlockAmount,
      toStringCargoByType(unlockedWith, false),
      (double) FDesc[i].price / CargoDesc[ unlockedWith ].price,
      CargoDesc[ unlockedWith ].price
    );
    #endif

    skip = 1;
  }

  // Extractor prices
  EDesc[kCropHarvesterSmall].price = 50;
  EDesc[kChalkQuarry].price = 75;
  EDesc[kSaltMine].price = FDesc[kCrispsFac].price * 0.75f;
  EDesc[kPump].price = FDesc[kEthanolFac].price * 0.75f;
  EDesc[kCropHarvesterLarge].price = FDesc[kJelloShotFac].price * 0.75f;
  EDesc[kCO2Extractor].price = FDesc[kCaffeineFac].price * 0.75f;

  // Utility prices
  UDesc[kPath].price = FDesc[kEthanolFac].price * 0.05f;
  UDesc[kSign].price = FDesc[kBeerFac].price * 0.20f;
  UDesc[kBin].price = FDesc[kTequilaFac].price * 0.25f;
  UDesc[kWell].price = FDesc[kHFCSFac].price * 0.40f;
  UDesc[kStorageBox].price = FDesc[kEmulsifierFac].price * 0.30f;
  UDesc[kBuffferBox].price = FDesc[kEmulsifierFac].price * 0.60f;
  UDesc[kFactoryUpgrade].price = FDesc[kChocolateFac].price * 0.80f;
  UDesc[kConveyorGrease].price = FDesc[kBatteryFarm].price * 0.02f;
  UDesc[kObstructionRemover].price = FDesc[kMSGFac].price * 0.40f;
  UDesc[kLandfill].price = FDesc[kPerkPillFac].price * 0.1f;
  UDesc[kRotavator].price = FDesc[kPerkPillFac].price * 0.60f;
  UDesc[kRetirement].price = FDesc[kMiraclePowderFac].price * 7.5f;
  UDesc[kFence].price = FDesc[kMiraclePowderFac].price * 0.05f;

  // Warp prices
  WDesc[kChalkWorld].price = FDesc[kBeerFac].price * 3; // First sand
  WDesc[kPeatWorld].price = FDesc[kJelloShotFac].price * 4; // First Clay
  WDesc[kSandWorld].price = FDesc[kCakeFac].price * 4; // First Loam
  WDesc[kClayWorld].price = FDesc[kRaveJuiceFac].price * 4;
  WDesc[kLoamWoarld].price = FDesc[kDessertFac].price * 4;
  WDesc[kWaterWorld].unlock = kPartyPack;
  WDesc[kWaterWorld].price = 200; // This is now number of items
  WDesc[kTranquilWorld].unlock = kMiraclePowder;
  WDesc[kTranquilWorld].price = 400; // This is now number of items
}