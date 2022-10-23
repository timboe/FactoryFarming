#pragma once

enum kConvSubType{
  kBelt, 
  kSplitL, 
  kSplitI, 
  kSplitT, 
  kSpareConv0, // SPARE SLOT (Game Expansions)
  kSpareConv1, // SPARE SLOT (Game Expansions)
  kTunnelIn, 
  kTunnelOut,
  kSpareConv2, // SPARE SLOT (Game Expansions)
  kSpareConv3,  // SPARE SLOT (Game Expansions)
  kFilterL, 
  kFilterI,
  kSpareConv4, // SPARE SLOT (Game Expansions)
  kSpareConv5, // SPARE SLOT (Game Expansions)
  kSpareConv6, // SPARE SLOT (Game Expansions)
  kSpareConv7, // SPARE SLOT (Game Expansions)
  kNConvSubTypes
};

enum kPlantSubType{
  kCarrotPlant, // Silty
  kSunflowerPlant, // Chalky
  kSparePlant0, // SPARE SLOT (Game Expansions)
  kSparePlant1, // SPARE SLOT (Game Expansions)
  kPotatoPlant,  // Peaty
  kAppleTree, // Peaty
  kSeaweedPlant, // Lake
  kSparePlant2, // SPARE SLOT (Game Expansions)
  kSparePlant3, // SPARE SLOT (Game Expansions)
  kCornPlant, // Chalky
  kWheatPlant, // Silty
  kSparePlant4, // SPARE SLOT (Game Expansions)
  kSparePlant5, // SPARE SLOT (Game Expansions)
  kCactusPlant, // Desert
  kLimeTree, // Desert
  kSparePlant6, // SPARE SLOT (Game Expansions)
  kSparePlant7, // SPARE SLOT (Game Expansions)
  kStrawberryPlant, // Clay - Need to do more with this
  kCocoaPlant, // Clay 
  kSparePlant8, // SPARE SLOT (Game Expansions)
  kSparePlant9, // SPARE SLOT (Game Expansions)
  kCoffeePlant, // Loamy
  kBambooPlant, // Loamy
  kSparePlant10, // SPARE SLOT (Game Expansions)
  kSparePlant11, // SPARE SLOT (Game Expansions)
  kSeaCucumberPlant, // Ocean
  kRosePlant, // Loamy (end game)
  kSparePlant12, // SPARE SLOT (Game Expansions)
  kSparePlant13, // SPARE SLOT (Game Expansions)
  kSparePlant14, // SPARE SLOT (Game Expansions)
  kSparePlant15, // SPARE SLOT (Game Expansions)
  kNPlantSubTypes
};

enum kUtilitySubType{
  kPath,
  kSign,
  kSpareUtil0, // SPARE SLOT (Game Expansions)
  kSpareUtil1, // SPARE SLOT (Game Expansions)
  kBin, 
  kWell, 
  kStorageBox,
  kBuffferBox,
  kSpareUtil2, // SPARE SLOT (Game Expansions)
  kSpareUtil3, // SPARE SLOT (Game Expansions)
  kConveyorGrease, 
  kObstructionRemover,
  kLandfill, 
  kSpareUtil4, // SPARE SLOT (Game Expansions)
  kSpareUtil5, // SPARE SLOT (Game Expansions)
  kRetirement,
  kFence,
  kSpareUtil6, // SPARE SLOT (Game Expansions)
  kSpareUtil7, // SPARE SLOT (Game Expansions)
  kNUtilitySubTypes
};

enum kExtractorSubType{
  kCropHarvesterSmall, 
  kSpareExtractor0, // SPARE SLOT (Game Expansions)
  kChalkQuarry,
  kSpareExtractor1, // SPARE SLOT (Game Expansions)
  kSaltMine, 
  kSpareExtractor2, // SPARE SLOT (Game Expansions)
  kPump, 
  kSpareExtractor3, // SPARE SLOT (Game Expansions)
  kCropHarvesterLarge,
  kSpareExtractor4, // SPARE SLOT (Game Expansions)
  kCO2Extractor, 
  kSpareExtractor5, // SPARE SLOT (Game Expansions)
  kSpareExtractor6, // SPARE SLOT (Game Expansions)
  kSpareExtractor7, // SPARE SLOT (Game Expansions)
  kNExtractorSubTypes
};

enum kFactorySubType{
  kVitiminFac, 
  kVegOilFac, 
  kSpareFac0, // SPARE SLOT (Game Expansions)
  kSpareFac1, // SPARE SLOT (Game Expansions)
  kCrispsFac, 
  kEthanolFac, 
  kHardCiderFac,
  kSpareFac2, // SPARE SLOT (Game Expansions)
  kSpareFac3, // SPARE SLOT (Game Expansions) 
  kAbattoir,
  kHydrogenFac,
  kHOILFac,
  kCornDogFac,
  kBeerFac,
  kSpareFac4, // SPARE SLOT (Game Expansions)
  kSpareFac5, // SPARE SLOT (Game Expansions)
  kTequilaFac,
  kHFCSFac,
  kGelatinFac,
  kJellyFac,
  kJelloShotFac,
  kSpareFac6, // SPARE SLOT (Game Expansions)
  kSpareFac7, // SPARE SLOT (Game Expansions)
  kEmulsifierFac,
  kIceCreamFac,
  kBatteryFarm,
  kProteinFac,
  kChocolateFac,
  kPieFac,
  kMSGFac,
  kTVDinnerFac,
  kCakeFac,
  kSpareFac8, // SPARE SLOT (Game Expansions)
  kSpareFac9, // SPARE SLOT (Game Expansions)
  kCaffeineFac,
  kEnergyDrinkFac,
  kRaveJuiceFac,
  kPerkPillFac,
  kPackagingFac,
  kDessertFac,
  kCateringKitFac,
  kChineseReayMealFac,
  kPartyPackFac,
  kSpareFac10, // SPARE SLOT (Game Expansions)
  kSpareFac11, // SPARE SLOT (Game Expansions)
  kParfumeFac,
  kMiraclePowderFac,
  kTurkishDelightFac,
  kSpareFac12, // SPARE SLOT (Game Expansions)
  kSpareFac13, // SPARE SLOT (Game Expansions)
  kSpareFac14, // SPARE SLOT (Game Expansions)
  kSpareFac15, // SPARE SLOT (Game Expansions)
  kNFactorySubTypes
};

enum kSpecialSubType{
  kShop, 
  kSellBox, 
  kExportBox, 
  kImportBox, 
  kWarp, 
  kNSpecialSubTypes
};

#define FINAL_UNLOCK_TYPE kFactory
#define FINAL_UNLOCK_SUBTYPE kTurkishDelightFac