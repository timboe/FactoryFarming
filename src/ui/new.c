#include "new.h"
#include "../player.h"

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

bool checkHasNewToShow(struct Player_t* _p) {
  const uint32_t currentLevel = _p->m_buildingsUnlockedTo;
  if (UnlockDecs[currentLevel].type == kUtility && UnlockDecs[currentLevel].subType.utility == kRetirement) {
    // This is always the last unlock - cannot go on from here
    return false;
  }
  const bool haveUnlocked = (_p->m_soldCargo[ UnlockDecs[ currentLevel+1 ].ofCargo ] >= UnlockDecs[ currentLevel+1 ].fromSelling);
  // Tutorial
  if (getTutorialStage() < kTutBuildConveyor && UnlockDecs[currentLevel+1].type == kConveyor) {
    return false; // Need to progress the tutorial too to unlock conveyors
  }
  // Tutorial
  if (getTutorialStage() < kTutBuildQuarry && UnlockDecs[currentLevel+1].type == kExtractor && UnlockDecs[currentLevel+1].subType.extractor == kChalkQuarry) {
    return false; // Need to progress the tutorial too to unlock chalk quarry
  }
  // Tutorial
  if (getTutorialStage() < kTutBuildVitamin && UnlockDecs[currentLevel+1].type == kFactory) {
    return false; // Need to progress the tutorial too to unlock factory
  }
  return haveUnlocked;
}

bool checkShowNew() {
  struct Player_t* p = getPlayer();

  if (!checkHasNewToShow(p)) {
    return false;
  }

  const uint32_t testLevel = p->m_buildingsUnlockedTo + 1;
  m_nextBuilding = UnlockDecs[testLevel].type;
  m_nextSubType = UnlockDecs[testLevel].subType;
  p->m_buildingsUnlockedTo += 1;
  setGameMode(kMenuNew);
  return true;
}

const char* getNewText() {
  switch(m_nextBuilding) {
    case kConveyor: switch (m_nextSubType.conveyor) {
      case kBelt: return "Moves Cargo North, South, East or West";
      case kSplitI: return  "Splits incoming cargo 2 ways (180 degrees)";
      case kSplitL: return "Splits incoming cargo 2 ways (90 degrees)";
      case kSplitT: return "Splits incoming cargo 3 ways";
      case kFilterI: return "Filters on the first Cargo to pass over";
      case kFilterL: return "Filters on the first Cargo to pass over";
      case kTunnelIn: case kTunnelOut: return "Moves Cargo underground for 1 tile";
      case kNConvSubTypes: return "";
    }
    case kPlant: switch (m_nextSubType.plant) {
      case kCarrotPlant: return "Plant to grow Carrots";
      case kSunflowerPlant: return "Plant to harvest Sunflowers";
      case kPotatoPlant: return "Plant to grow Potatoes";
      case kAppleTree: return "Plant to grow Apples";
      case kCornPlant: return "Plant to grow Corn";
      case kSeaweedPlant: return  "Seaweed grows in Lakes";
      case kWheatPlant: return "Plant to grow Wheat";
      case kCactusPlant: return "Plant to grow Cactus";
      case kLimeTree: return "Plant to grow Limes";
      case kStrawberryPlant:  return "Plant to grow Strawberries";  
      case kHempPlant: return "Plant to grow Hemp Fibers";
      case kCoffeePlant: return "Plant to grow Coffee Beans";
      case kCocoPlant: return "Plant to grow Coco Beans";
      case kSeaCucumberPlant: return "Plant in the Ocean to grow Sea Cucumbers";
      //case kP4:; case kP5:; case kP6:; case kP7:; case kP8:; case kP9:; case kP10:; case kP11:; case kP12:; return "Placeholder";
      case kNPlantSubTypes: return "";
    }
    case kUtility: switch (m_nextSubType.utility) {
      case kPath: return "Movement speed is enhanced while on a path";
      case kSign: return "Displays a Cargo of your choosing";
      case kBin: return "Cargo placed here will get destroyed";
      case kWell: return "Place to dampen the surrounding soil";
      case kStorageBox: return "Stores up to 3 different types of Cargo";
      case kBuffferBox: return "Like the Storage Box, but also self-empties";
      case kConveyorGrease: return "Upgrades Conveyor speed from x1 to x2";
      case kObstructionRemover: return "Clears Obstructed Ground allowing it to be built on";
      case kLandfill: return "Fills in water. Can built on, but not planted on.";
      case kRetirement: return "A relaxing space to enjoy your amassed wealth.";
      case kFence: return "A charming wooden fence.";

      case kNUtilitySubTypes: return "";
    }
    case kExtractor: switch (m_nextSubType.extractor) {
      case kCropHarvesterSmall: return "Collects Cargo, can hold three different types";
      case kPump: return "Must be built on water. Produced Water Barrels";
      case kChalkQuarry: return "Must be built on Chalky Soil. Produces Chalk";
      case kSaltMine: return "Must be built on Peaty Soil. Produces Salt";
      case kCropHarvesterLarge: return "Collects Cargo, can hold three different types";
      case kCO2Extractor: return "Liquefies Carbon Dioxide out of the air";
      case kNExtractorSubTypes: return "";
    }
    case kFactory: switch (m_nextSubType.factory) {
      case kVitiminFac: return "Manufactures low-grade Vitamin Pills";
      case kVegOilFac: return "Crush Vegetable Oil from Sunflowers";
      case kCrispsFac: return "Manufactures Potato Chips. A fried favorite";
      //case kF4: case kF5: case kF6: case kF7: case kF8: case kF9: case kF10: case kF11: return "FPlaceholder"; 
      case kNFactorySubTypes: return "";
    }
    default: break;
  }
  return "??";
}