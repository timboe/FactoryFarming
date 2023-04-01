#include "new.h"
#include "../player.h"
#include "../sound.h"
#include "../buildings/special.h"

uint32_t m_nextPrice;
enum kBuildingType m_nextBuilding;
union kSubType m_nextSubType;

/// ///

enum kBuildingType getNewBuildingType() {
  return m_nextBuilding;
}

uint32_t getNewID() {
  return m_nextSubType.raw;
}

enum kNewReturnStatus checkHasNewToShow(struct Player_t* _p) {
  const uint32_t currentLevel = _p->m_buildingsUnlockedTo;
  if (UnlockDecs[currentLevel].type == FINAL_UNLOCK_TYPE && UnlockDecs[currentLevel].subType.raw == FINAL_UNLOCK_SUBTYPE) {
    // This is always the last unlock - cannot go on from here
    return kNewNoUnlockedAll;
  }

  uint16_t needToSell = UnlockDecs[ currentLevel+1 ].fromSelling;
  #ifdef FAST_PROGRESS
  if (needToSell > FAST_PROGRESS_SALES) needToSell = FAST_PROGRESS_SALES;
  #endif

  const bool haveUnlocked = (_p->m_soldCargo[ UnlockDecs[ currentLevel+1 ].ofCargo ] >= needToSell);

  // Tutorial
  if (getTutorialStage() < kTutBuildConveyor && UnlockDecs[currentLevel+1].type == kConveyor) {
    return kNewNoNeedsTutorial; // Need to progress the tutorial too to unlock conveyors
  }
  // Tutorial
  if (getTutorialStage() < kTutBuildQuarry && UnlockDecs[currentLevel+1].type == kExtractor && UnlockDecs[currentLevel+1].subType.extractor == kChalkQuarry) {
    return kNewNoNeedsTutorial; // Need to progress the tutorial too to unlock chalk quarry
  }
  // Tutorial
  if (getTutorialStage() < kTutBuildVitamin && UnlockDecs[currentLevel+1].type == kFactory) {
    return kNewNoNeedsTutorial; // Need to progress the tutorial too to unlock factory
  }

  return haveUnlocked ? kNewYes : kNewNoNeedsFarming;
}

bool checkShowNew() {
  struct Player_t* p = getPlayer();

  if (checkHasNewToShow(p) != kNewYes) {
    return false;
  }

  const uint32_t testLevel = p->m_buildingsUnlockedTo + 1;
  m_nextBuilding = UnlockDecs[testLevel].type;
  m_nextSubType = UnlockDecs[testLevel].subType;
  p->m_buildingsUnlockedTo += 1;
  setGameMode(kMenuNew);
  sfx(kSfxUnlock);
  resetUnlockPing();
  return true;
}

const char* getNewText() {
  switch(m_nextBuilding) {
    case kConveyor: switch (m_nextSubType.conveyor) {
      case kBelt: return tr(kTRNewBelt);
      case kSplitI: return  tr(kTRNewSplit2_180);
      case kSplitL: return tr(kTRNewSplit2_90);
      case kSplitT: return tr(kTRNewSplit3);
      case kOverflowI: case kOverflowL: return tr(kTRNewOverflow);
      case kFilterI: case kFilterL: return tr(kTRNewFilter);
      case kTunnelIn: case kTunnelOut: return tr(kTRNewTunnel);
      case kSpareConv0: case kSpareConv1: case kSpareConv4: case kSpareConv5:
      case kSpareConv6: case kSpareConv7:
      case kNConvSubTypes:;
    }
    case kPlant: switch (m_nextSubType.plant) {
      case kCarrotPlant: return tr(kTRNewCarrotPlant);
      case kSunflowerPlant: return tr(kTRNewSunflowerPlant);
      case kPotatoPlant: return tr(kTRNewPotatoPlant);
      case kAppleTree: return tr(kTRNewAppleTree);
      case kCornPlant: return tr(kTRNewCornPlant);
      case kSeaweedPlant: return  tr(kTRNewSeaweedPlant);
      case kWheatPlant: return tr(kTRNewWheatPlant);
      case kCactusPlant: return tr(kTRNewCactusPlant);
      case kLimeTree: return tr(kTRNewLimeTree);
      case kStrawberryPlant:  return tr(kTRNewStrawberryPlant);  
      case kCocoaPlant: return tr(kTRNewCocoaPlant);
      case kCoffeePlant: return tr(kTRNewCoffeePlant);
      case kBambooPlant: return tr(kTRNewBambooPlant);
      case kSeaCucumberPlant: return tr(kTRNewSeaCucumberPlant);
      case kRosePlant: return tr(kTRNewRosePlant);
      case kSparePlant0: case kSparePlant1: case kSparePlant2: case kSparePlant3:
      case kSparePlant4: case kSparePlant5: case kSparePlant6: case kSparePlant7: 
      case kSparePlant8: case kSparePlant9: case kSparePlant10: case kSparePlant11:
      case kSparePlant12: case kSparePlant13: case kSparePlant14: case kSparePlant15: 
      case kNPlantSubTypes:;
    }
    case kUtility: switch (m_nextSubType.utility) {
      case kPath: return tr(kTRNewPath);
      case kSign: return tr(kTRNewSign);
      case kBin: return tr(kTRNewBin);
      case kWell: return tr(kTRNewWell);
      case kStorageBox: return tr(kTRNewStorageBox);
      case kBuffferBox: return tr(kTRNewBuffferBox);
      case kFactoryUpgrade: return tr(kTRNewFactoryUpgrade); // New to v1.1
      case kConveyorGrease: return tr(kTRNewConveyorGrease);
      case kObstructionRemover: return tr(kTRNewObstructionRemover);
      case kLandfill: return tr(kTRNewLandfill);
      case kRotavator: return tr(kTRNewRotavator); // New to v1.1 
      case kRetirement: return tr(kTRNewRetirement);
      case kFence: return tr(kTRNewFence);
      case kSpareUtil0: case kSpareUtil1: case kSpareUtil3:
      case kSpareUtil5: case kSpareUtil6: case kSpareUtil7: 
      case kNUtilitySubTypes:;
    }
    case kExtractor: switch (m_nextSubType.extractor) {
      case kCropHarvesterSmall: return tr(kTRNewCropHarvesterSmall);
      case kPump: return tr(kTRNewPump);
      case kChalkQuarry: return tr(kTRNewChalkQuarry);
      case kSaltMine: return tr(kTRNewSaltMine);
      case kCropHarvesterLarge: return tr(kTRNewCropHarvesterLarge);
      case kCO2Extractor: return tr(kTRNewCO2Extractor);
      case kSpareExtractor0: case kSpareExtractor1: case kSpareExtractor2: case kSpareExtractor3:
      case kSpareExtractor4: case kSpareExtractor5: case kSpareExtractor6: case kSpareExtractor7:
      case kNExtractorSubTypes:;
    }
    case kFactory: switch (m_nextSubType.factory) {
      case kVitiminFac: return tr(kTRNewVitiminFac);
      case kVegOilFac: return tr(kTRNewVegOilFac);
      case kCrispsFac: return tr(kTRNewCrispsFac);
      case kEthanolFac: return tr(kTRNewEthanolFac); 
      case kHardCiderFac: return tr(kTRNewHardCiderFac); 
      case kAbattoir: return tr(kTRNewAbattoir);
      case kHydrogenFac: return tr(kTRNewHydrogenFac);
      case kHOILFac: return tr(kTRNewHOILFac);
      case kCornDogFac: return tr(kTRNewCornDogFac);
      case kBeerFac: return tr(kTRNewBeerFac);
      case kTequilaFac: return tr(kTRNewTequilaFac);
      case kHFCSFac: return tr(kTRNewHFCSFac);
      case kGelatinFac: return tr(kTRNewGelatinFac);
      case kJellyFac: return tr(kTRNewJellyFac);
      case kJelloShotFac: return tr(kTRNewJelloShotFac);
      case kEmulsifierFac: return tr(kTRNewEmulsifierFac);
      case kIceCreamFac: return tr(kTRNewIceCreamFac);
      case kBatteryFarm: return tr(kTRNewBatteryFarm);
      case kProteinFac: return tr(kTRNewProteinFac);
      case kChocolateFac: return tr(kTRNewChocolateFac);
      case kPieFac: return tr(kTRNewPieFac);
      case kMSGFac: return tr(kTRNewMSGFac);
      case kTVDinnerFac: return tr(kTRNewTVDinnerFac);
      case kCaffeineFac: return tr(kTRNewCaffeineFac);
      case kEnergyDrinkFac: return tr(kTRNewEnergyDrinkFac);
      case kRaveJuiceFac: return tr(kTRNewRaveJuiceFac);
      case kPerkPillFac: return tr(kTRNewPerkPillFac);
      case kCakeFac: return tr(kTRNewCakeFac);
      case kDessertFac: return tr(kTRNewDessertFac);
      case kPackagingFac: return tr(kTRNewPackagingFac);
      case kCateringKitFac: return tr(kTRNewCateringKitFac);
      case kChineseReayMealFac: return tr(kTRNewChineseReayMealFac);
      case kPartyPackFac: return tr(kTRNewPartyPackFac);
      case kParfumeFac: return tr(kTRNewParfumeFac);
      case kMiraclePowderFac: return tr(kTRNewMiraclePowderFac);
      case kTurkishDelightFac: return tr(kTRNewTurkishDelightFac);
      case kSpareFac0: case kSpareFac1: case kSpareFac2: case kSpareFac3:
      case kSpareFac4: case kSpareFac5: case kSpareFac6: case kSpareFac7: 
      case kSpareFac8: case kSpareFac9: case kSpareFac10: case kSpareFac11:
      case kSpareFac12: case kSpareFac13: case kSpareFac14: case kSpareFac15: 
      case kNFactorySubTypes:;
    }
    default: break;
  }
  return "??";
}