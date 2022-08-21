#pragma once

enum kConvSubType{
  kBelt, 
  kSplitL, 
  kSplitI, 
  kSplitT, 
  kTunnelIn, 
  kTunnelOut, 
  kFilterL, 
  kFilterI, 
  //kC0, kC1, kC2, kC3, kC4, kC5, kC6, kC7, kC8, kC9, kC10, kC11, kC12, kC13, kC14, kC15, 
  kNConvSubTypes
};

enum kPlantSubType{
  kCarrotPlant, // Silty
  kPotatoPlant,  // Peaty
  kSunflowerPlant, // Chalky
  kAppleTree, // Peaty
  kCornPlant, // Chalky
  kSeaweedPlant, // Lake
  kWheatPlant, // Silty
  kCactusPlant, // Desert
  kLimeTree, // Desert
  kStrawberryPlant, // Clay - Need to do more with this
  kCocoaPlant, // Clay 
  kCoffeePlant, // Loamy
  kBambooPlant, // Loamy
  kSeaCucumberPlant, // Ocean
  //kP0, kP1, kP2, kP3, kP4, kP5, kP6, kP7, kP8, kP9, kP10, kP11, kP12, kP13, kP14, kP15, 
  kNPlantSubTypes
};

enum kUtilitySubType{
  kPath,
  kSign,
  kBin, 
  kWell, 
  kStorageBox,
  kBuffferBox, 
  kConveyorGrease, 
  kObstructionRemover,
  kLandfill, 
  kRetirement,
  kFence,
  //kU0, kU1, kU2, kU3, kU4, kU5, kU6, kU7, kU8, kU9, kU10, kU11, kU12, kU13, kU14, kU15,
  kNUtilitySubTypes
};

enum kExtractorSubType{
  kCropHarvesterSmall, 
  kChalkQuarry, 
  kSaltMine, 
  kPump, 
  kCropHarvesterLarge,
  kCO2Extractor, 
  //kE0, kE1, kE2, kE3, kE4, kE5, kE6, kE7, kE8, kE9, kE10, kE11, kE12, kE13, kE14, kE15,
  kNExtractorSubTypes
};

enum kFactorySubType{
  kVitiminFac, 
  kVegOilFac, 
  kCrispsFac, 
  kEthanolFac, 
  kHardCiderFac, 
  kAbattoir,
  kHydrogenFac,
  kHOILFac,
  kCornDogFac,
  kBeerFac,
  kTequilaFac,
  kMexBeerFac,
  kEmulsifierFac,
  kHFCSFac,
  kIceCreamFac,
  kBatteryFarm,
  kProteinFac,
  kGelatinFac,
  kPieFac,
  kMSGFac,
  kReadyMealFac,
  kJellyFac,
  kCaffeineFac,
  kEnergyDrinkFac,
  kRaveJuiceFac,
  kPerkPillFac,
  kSoftCakeFac,
  kDessertFac,
  kPackagingFac,
  kCateringKitFac,
  kDrinksKitFac,
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

    Bamboo -> Packaging
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