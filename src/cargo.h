#pragma once
#include "game.h"

enum kCargoType {
  kNoCargo, 
  kCarrot, 
  kChalk, 
  kVitamin, 
  kPotato, 
  kSunflower, 
  kOil, 
  kSalt, 
  kCrisps, 
  kApple, 
  kWaterBarrel, 
  kEthanol,
  kCorn,
  kSeaweed,
  kMeat,
  kHydrogen,
  kHOil,
  kCornDog,
  kWheat, 
  kBeer,
  kCactus,
  kTequila,
  kLime,
  kMexBeer,
  kEmulsifiers, 
  kHFCS,
  kStrawberry,
  kIceCream,
  kEggs,
  kHemp,
  kProtein,
  kGelatin,
  kMeatPie,
  kMSG,
  kReadyMeal,
  kJelly,
  kCoffeeBean,
  kCO2,
  kCaffeine,
  kEnergyDrink,
  kRaveJuice,
  kPerkPills,
  kSoftCake,
  kDessert,
  kPackaging,
  kCateringKit,
  kDrinksKit,
  //kCa0, kCa1, kCa2, kCa3, kCa4, kCa5, kCa6, kCa7, kCa8, kCa9, kCa10, kCa11, kCa12, kCa13, kCa14, kCa15, 
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

const char* toStringCargo(struct Cargo_t* _cargo);

const char* toStringCargoByType(enum kCargoType _type);

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