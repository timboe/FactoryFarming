#include "new.h"
#include "../player.h"

uint32_t m_nextPrice;
enum kUICat m_nextBuilding;
union kSubType m_nextSubType;

/// ///

enum kUICat getNewCategory() {
  return m_nextBuilding;
}

uint32_t getNewID() {
  return m_nextSubType.raw;
}

bool checkShowNew() {
  struct Player_t* p = getPlayer();

  // Look for any upgrade which has not yet been seen
  const uint32_t seenTo = p->m_moneyHighWaterMarkMenu;
  const uint32_t ceiling = p->m_moneyHighWaterMark;

  m_nextPrice = UINT32_MAX;
  m_nextBuilding = 0;
  m_nextSubType.raw = 0;

  for (int32_t i = 0; i < kNConvSubTypes; ++i) {
    const uint32_t unlock = CDesc[i].unlock;
    if (unlock > seenTo && unlock <= ceiling && unlock < m_nextPrice) {
      m_nextBuilding = kUICatConv;
      m_nextSubType.conveyor = i;
      m_nextPrice = unlock;
    }
  }

  for (int32_t i = 0; i < kNPlantSubTypes; ++i) {
    const uint32_t unlock = PDesc[i].unlock;
    if (unlock > seenTo && unlock <= ceiling && unlock < m_nextPrice) {
      m_nextBuilding = kUICatPlant;
      m_nextSubType.plant = i;
      m_nextPrice = unlock;
    }
  }

  for (int32_t i = 0; i < kNExtractorSubTypes; ++i) {
    const uint32_t unlock = EDesc[i].unlock;
    if (unlock > seenTo && unlock <= ceiling && unlock < m_nextPrice) {
      m_nextBuilding = kUICatExtractor;
      m_nextSubType.extractor = i;
      m_nextPrice = unlock;
    }
  }

  for (int32_t i = 0; i < kNUtilitySubTypes; ++i) {
    const uint32_t unlock = UDesc[i].unlock;
    if (unlock > seenTo && unlock <= ceiling && unlock < m_nextPrice) {
      m_nextBuilding = kUICatUtility;
      m_nextSubType.utility = i;
      m_nextPrice = unlock;
    }
  }

  for (int32_t i = 0; i < kNFactorySubTypes; ++i) {
    const uint32_t unlock = FDesc[i].unlock;
    if (unlock > seenTo && unlock <= ceiling && unlock < m_nextPrice) {
      m_nextBuilding = kUICatFactory;
      m_nextSubType.factory = i;
      m_nextPrice = unlock;
    }
  }

  if (m_nextPrice == UINT32_MAX) {
    return false;
  }

  p->m_moneyHighWaterMarkMenu = m_nextPrice;
  setGameMode(kMenuNew);
  return true;
}

const char* getNewText() {
  switch(m_nextBuilding) {
    case kUICatConv: switch (m_nextSubType.conveyor) {
      case kBelt: return "Moves Cargo North, South, East or West";
      case kSplitI: return  "Splits incoming cargo 2 ways (180 degrees)";
      case kSplitL: return "Splits incoming cargo 2 ways (90 degrees)";
      case kSplitT: return "Splits incoming cargo 3 ways";
      case kFilterI: return "Filters on the first Cargo to pass over";
      case kFilterL: return "Filters on the first Cargo to pass over";
      case kTunnelIn: case kTunnelOut: return "Moves Cargo underground for 1 tile";
      case kNConvSubTypes: return "";
    }
    case kUICatPlant: switch (m_nextSubType.plant) {
      case kCarrotPlant: return "Plant to grow Carrots";
      case kSunflowerPlant: return "Plant to harvest Sunflowers";
      case kPotatoPlant: return "Plant to grow Potatoes";
      case kAppleTree: return  "Plant to grow Apples";
      case kWheatPlant: return "Plant to grow Wheat";
      case kP4:; case kP5:; case kP6:; case kP7:; case kP8:; case kP9:; case kP10:; case kP11:; case kP12:; return "Placeholder";
      case kNPlantSubTypes: return "";
    }
    case kUICatUtility: switch (m_nextSubType.utility) {
      case kBin: return "Cargo placed here will get destroyed";
      case kWell: return "Place to dampen the surrounding soil";
      case kStorageBox: return "Stores up to 3 different types of Cargo";
      case kConveyorGrease: return "Upgrades Conveyor speed from x1 to x2";
      case kNUtilitySubTypes: return "";
    }
    case kUICatExtractor: switch (m_nextSubType.extractor) {
      case kCropHarvesterSmall: return "Collects Cargo, can hold three different types";
      case kPump: return "Must be built on water. Produced Water Barrels";
      case kChalkQuarry: return "Must be built on Chalky Soil. Produces Chalk";
      case kSaltMine: return "Must be built on Peaty Soil. Produces Salt";
      case kCropHarvesterLarge: return "Collects Cargo, can hold three different types";
      case kNExtractorSubTypes: return "";
    }
    case kUICatFactory: switch (m_nextSubType.factory) {
      case kVitiminFac: return "Manufactures low-grade Vitamin Pills";
      case kVegOilFac: return "Crush Vegetable Oil from Sunflowers";
      case kCrispsFac: return "Manufactures Potato Chips. A fried favorite.";
      case kF4: case kF5: case kF6: case kF7: case kF8: case kF9: case kF10: case kF11: return "FPlaceholder"; 
      case kNFactorySubTypes: return "";
    }
    default: break;
  }
  return "??";
}