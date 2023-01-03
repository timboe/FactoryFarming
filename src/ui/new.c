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
      case kBelt: return "Moves cargo North, South, East or West";
      case kSplitI: return  "Splits incoming cargo 2 ways (180 degrees)";
      case kSplitL: return "Splits incoming cargo 2 ways (90 degrees)";
      case kSplitT: return "Splits incoming cargo 3 ways";
      case kOverflowI: case kOverflowL: return "Overflow used when main exit is blocked";
      case kFilterI: case kFilterL: return "Filters on the first cargo to pass over";
      case kTunnelIn: case kTunnelOut: return "Moves cargo underground for 1 tile";
      case kSpareConv0: case kSpareConv1: case kSpareConv4: case kSpareConv5:
      case kSpareConv6: case kSpareConv7:
      case kNConvSubTypes:;
    }
    case kPlant: switch (m_nextSubType.plant) {
      case kCarrotPlant: return "Plant to grow carrots";
      case kSunflowerPlant: return "Plant to harvest sunflowers";
      case kPotatoPlant: return "Plant to grow potatoes";
      case kAppleTree: return "Plant to grow apples";
      case kCornPlant: return "Plant to grow corn";
      case kSeaweedPlant: return  "Seaweed grows in lakes";
      case kWheatPlant: return "Plant to grow wheat";
      case kCactusPlant: return "Plant to grow cacti";
      case kLimeTree: return "Plant to grow limes";
      case kStrawberryPlant:  return "Plant to grow strawberries";  
      case kCocoaPlant: return "Plant to grow coco beans";
      case kCoffeePlant: return "Plant to grow coffee beans";
      case kBambooPlant: return "Plant to grow bamboo stalks";
      case kSeaCucumberPlant: return "Place in the ocean to farm sea cucumbers";
      case kRosePlant: return "Tend to these Roses in your retirement.";
      case kSparePlant0: case kSparePlant1: case kSparePlant2: case kSparePlant3:
      case kSparePlant4: case kSparePlant5: case kSparePlant6: case kSparePlant7: 
      case kSparePlant8: case kSparePlant9: case kSparePlant10: case kSparePlant11:
      case kSparePlant12: case kSparePlant13: case kSparePlant14: case kSparePlant15: 
      case kNPlantSubTypes:;
    }
    case kUtility: switch (m_nextSubType.utility) {
      case kPath: return "Movement speed is enhanced while on a path";
      case kSign: return "Displays a cargo of your choosing";
      case kBin: return "Cargo placed here will be destroyed";
      case kWell: return "Place to dampen the surrounding soil";
      case kStorageBox: return "Stores up to 3 different types of cargo";
      case kBuffferBox: return "Like the storage box, but also self-empties";
      case kFactoryUpgrade: return "Place adjacent to factories to reduce production time."; // New to v1.1
      case kConveyorGrease: return "Upgrades conveyor speed from x1 to x2";
      case kObstructionRemover: return "Clears obstructed ground, can then be built on";
      case kLandfill: return "Fills in water. Can be built on, but not planted on.";
      case kRotavator: return "Turns nearby land into loamy soil."; // New to v1.1 
      case kRetirement: return "A relaxing space to enjoy your amassed wealth.";
      case kFence: return "A charming wooden fence.";
      case kSpareUtil0: case kSpareUtil1: case kSpareUtil3:
      case kSpareUtil5: case kSpareUtil6: case kSpareUtil7: 
      case kNUtilitySubTypes:;
    }
    case kExtractor: switch (m_nextSubType.extractor) {
      case kCropHarvesterSmall: return "Collects cargo, can hold three different types";
      case kPump: return "Must be built on water. Produced water buckets";
      case kChalkQuarry: return "Must be built on chalky soil. Produces chalk";
      case kSaltMine: return "Must be built on peaty soil. Produces salt";
      case kCropHarvesterLarge: return "Collects cargo, can hold three different types";
      case kCO2Extractor: return "Liquefies carbon dioxide right out of the air";
      case kSpareExtractor0: case kSpareExtractor1: case kSpareExtractor2: case kSpareExtractor3:
      case kSpareExtractor4: case kSpareExtractor5: case kSpareExtractor6: case kSpareExtractor7:
      case kNExtractorSubTypes:;
    }
    case kFactory: switch (m_nextSubType.factory) {
      case kVitiminFac: return "Manufactures low-grade vitamin pills";
      case kVegOilFac: return "Crush vegetable oil from sunflowers";
      case kCrispsFac: return "Manufactures potato chips. A fried favorite!";
      case kEthanolFac: return "Brew & distill pure alcohol"; 
      case kHardCiderFac: return "Mix up some cheap hard 'cider'"; 
      case kAbattoir: return "Feed goes in, 'meat' comes out";
      case kHydrogenFac: return "Splits water and captures the hydrogen";
      case kHOILFac: return "Hydrogenates oil to prevent spoiling";
      case kCornDogFac: return "Crunchy and meaty";
      case kBeerFac: return "Produces something which resembles beer";
      case kTequilaFac: return "At least it has some agave in it";
      case kHFCSFac: return "So very, very, sweet...";
      case kGelatinFac: return "Used to set a variety of liquids";
      case kJellyFac: return "A wobbly favorite";
      case kJelloShotFac: return "An interesting mix of flavors";
      case kEmulsifierFac: return "Stops things from separating out";
      case kIceCreamFac: return "It's dairy free! Is that a good thing?";
      case kBatteryFarm: return "Corn goes in, eggs come out";
      case kProteinFac: return "Maximum bulk, minimum fuss";
      case kChocolateFac: return "It's mostly sugar...";
      case kPieFac: return "What's for dinner? Meat pie!";
      case kMSGFac: return "Makes everything taste better";
      case kTVDinnerFac: return "A meal for one, served in a tray";
      case kCaffeineFac: return "Extracts the caffeine from coffee beans";
      case kEnergyDrinkFac: return "Caffeine high & sugar rush all-in-one";
      case kRaveJuiceFac: return "This stuff should probably be illegal...";
      case kPerkPillFac: return "Will keep you awake far too long";
      case kCakeFac: return "Chocolatey, soft, and an infinite shelf life";
      case kDessertFac: return "Sweet tooth?";
      case kPackagingFac: return "Scaling up with custom packaging";
      case kCateringKitFac: return "Packaged catering kits, great markup";
      case kChineseReayMealFac: return "Gelatinous, and dehydrating";
      case kPartyPackFac: return "Everything a good party needs";
      case kParfumeFac: return "Fancy stuff";
      case kMiraclePowderFac: return "What can't it cure?! (Anything...)";
      case kTurkishDelightFac: return "A delectable delicacy";
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