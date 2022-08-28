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
  kPotatoPlant,  // Peaty
  kSparePlant0, // SPARE SLOT (Game Expansions)
  kSparePlant1, // SPARE SLOT (Game Expansions)
  kSunflowerPlant, // Chalky
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
  kMexBeerFac,
  kEmulsifierFac,
  kSpareFac6, // SPARE SLOT (Game Expansions)
  kSpareFac7, // SPARE SLOT (Game Expansions)
  kHFCSFac,
  kIceCreamFac,
  kBatteryFarm,
  kProteinFac,
  kGelatinFac,
  kPieFac,
  kMSGFac,
  kReadyMealFac,
  kJellyFac,
  kSpareFac8, // SPARE SLOT (Game Expansions)
  kSpareFac9, // SPARE SLOT (Game Expansions)
  kCaffeineFac,
  kEnergyDrinkFac,
  kRaveJuiceFac,
  kPerkPillFac,
  kSoftCakeFac,
  kDessertFac,
  kPackagingFac,
  kCateringKitFac,
  kDrinksKitFac,
  kSpareFac10, // SPARE SLOT (Game Expansions)
  kSpareFac11, // SPARE SLOT (Game Expansions)
  kTurkishDelightFac,
  kSpareFac12, // SPARE SLOT (Game Expansions)
  kSpareFac13, // SPARE SLOT (Game Expansions)
  kSpareFac14, // SPARE SLOT (Game Expansions)
  kSpareFac15, // SPARE SLOT (Game Expansions)
  //kF0, kF1, kF2, kF3, kF4, kF5, kF6, kF7, kF8, kF9, kF10, kF11, kF12, kF13, kF14, kF15,
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

/*

digraph G {

    Carrots[T1] -> Vitamins
    Chalk -> Vitamins
    
    Sunflower[T1] -> Oil
    
    Potatoes[T2] -> Crisps
    Salt -> Crisps
    Oil -> Crisps
    
    Potatoes -> Ethanol
    Water -> Ethanol
    
    Ethanol -> HardCider
    Apples [T2] -> HardCider

    //Corn -> Biofuel
    //Sunflower -> Biofuel

    Sunflower -> Meat
    Seaweed [T W] -> Meat
    Water -> Meat

    Water -> Hydrogen

    Oil -> HOil
    Hydrogen -> HOil

    HOil -> CornDog
    Meat -> CornDog
    Corn[T3] -> CornDog
    
    Ethanol -> Beer
    Water -> Beer
    Wheat [T3] -> Beer


    // UNLOCK NEW WORLDS
    Ethanol -> Tequila
    Cactus [T4] -> Tequila
    
    Beer -> MexBeer
    Tequila -> MexBeer
    Lime [T4] -> MexBeer
    
    //Lime -> Bleach
    //Water -> Bleach
    //Chalk -> Bleach

    Seaweed -> Emulsifiers

    Corn [T3] -> HFCS
    Water -> HFCS

    Strawberry [T5] -> IceCream
    Emulsifiers -> IceCream
    HOil -> IceCream
    HFCS -> IceCream

    Corn -> Eggs
    Water -> Eggs  
    
    Eggs -> Protein
    Hemp [T5] -> Protien # Change to Seaweed
    #Emulsifiers -> Protien
    Chalk -> Protien
    Vitamins -> Protien
    
    Meat -> Gelatin
    
    Meat -> Pie
    Gelatin -> Pie
    Wheat [T3] -> Pie
    Water -> Pie
    Potatoes [T2] -> Pie

    Wheat [T3] -> MSG
    Salt -> MSG
    
    HFCS -> ReadyMeal
    Carrots [T1] -> ReadyMeal
    Meat -> ReadyMeal
    MSG -> ReadyMeal
    Emulsifiers -> ReadyMeal

    Gelatin -> Jelly
    Lime -> Jelly
    HFCS -> Jelly
    
    CO2 -> Caffeine
    Coffee [T6] -> Caffeine
    
    Water -> EnergyDrink (remove this?)
    HFCS -> EnergyDrink
    Caffeine -> EnergyDrink
    CO2 -> EnergyDrink
    Strawberry [T5] -> Energy Drink (late addition)
    
    HardCider -> RaveJuice
    EnergyDrink -> RaveJuice
    
    Caffeine -> PerkPills
    Chalk -> PerkPills
    
    HOil -> Twinkie
    HFCS -> Twinkie
    Eggs -> Twinkie
    Wheat -> Twinkie
    Emulsifiers -> Twinkie
    

    Jelly -> Dessert
    IceCream -> Dessert
    Apples [T2] -> Dessert

    Bamboo [T6] -> Packaging
    Cactus -> Packaging
    
    
    ReadyMeal -> PackagedDinner
    Desert -> PackagedDinner
    EnergyDrink -> PackagedDinner
    Packaging -> PackagedDinner

    Tequila -> BoozeSet
    MexBeer -> BoozeSet
    HardCider -> BoozeSet
    RaveJuice -> BoozeSet
    Packaging -> BoozeSet
    
    

    

}*/