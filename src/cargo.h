#pragma once
#include "game.h"

enum kCargoType {
  kNoCargo, 
  kCarrot, 
  kChalk, 
  kVitamin, 
  kSunflower, 
  kPotato, 
  kOil, 
  kSalt, 
  kCrisps, 
  kApple, 
  kWaterBarrel, 
  kEthanol,
  kHardCider,
  kCorn,
  kSeaweed,
  kMeat,
  kSpareCargo0,
  kSpareCargo1,
  kSpareCargo2,
  kSpareCargo3,
  kHydrogen,
  kHOil,
  kCornDog,
  kWheat, 
  kBeer,
  kCactus,
  kTequila,
  kLime,
  kHFCS,
  kGelatin,
  kJelly,
  kJelloShot,
  kStrawberry,
  kEmulsifiers, 
  kIceCream,
  kEggs,
  kSpareCargo4,
  kSpareCargo5,
  kSpareCargo6,
  kSpareCargo7,
  kProtein,
  kCocoBean,
  kChocolate,
  kMeatPie,
  kMSG,
  kTVDinner,
  kCake,
  kCoffeeBean,
  kCO2,
  kCaffeine,
  kEnergyDrink,
  kRaveJuice,
  kPerkPills,
  kBamboo,
  kPackaging,
  kDessert,
  kCateringKit,
  kSpareCargo8,
  kSpareCargo9,
  kSpareCargo10,
  kSpareCargo11,
  kFooYoung,
  kPartyPack,
  kSeaCucumber,
  kParfume,
  kMiricalPowder,
  kRose,
  kTurkishDelight,
  kSpareCargo12,
  kSpareCargo13,
  kSpareCargo14,
  kSpareCargo15,
  kNCargoType
};

struct CargoDescriptor_t {
  enum kCargoType subType;
  uint32_t price;
  uint16_t UIIcon;
};
extern const struct CargoDescriptor_t CargoDesc[];

///

struct Cargo_t{
  // Persistent
  uint16_t m_index; // My location in the cargo array.Created at RESET ("const")
  enum kCargoType m_type;
  // int pix_x, pix_y; - read from sprite on save

  // Transient
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Location 0 is unused - OWNED
};

///

struct Location_t;

uint16_t getNCargo(void);

const char* toStringCargo(struct Cargo_t* _cargo, bool _plural);

const char* toStringCargoByType(enum kCargoType _type, bool _plural);

struct Cargo_t* cargoManagerNewCargo(enum kCargoType _type);

struct Cargo_t* cargoManagerGetByIndex(uint16_t _index);

void cargoManagerFreeCargo(struct Cargo_t* _cargo);

bool newCargo(struct Location_t* loc, enum kCargoType _type, bool _addToDisplay);

bool canBePlacedCargo(struct Location_t* loc);

void initCargo(void);

void resetCargo(void);

void serialiseCargo(struct json_encoder* je);

void deserialiseValueCargo(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneCargo(json_decoder* jd, const char* _name, json_value_type _type);