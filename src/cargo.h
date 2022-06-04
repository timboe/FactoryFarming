#pragma once
#include "game.h"

enum kCargoType {kNoCargo, kApple, kCheese};

struct Cargo_t{
  // Persistent
  uint16_t m_index; // My location in the cargo array.
  enum kCargoType m_type;
  // int x, y; - read from sprite on save

  // Transient
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Location 0 is unused - OWNED
};

struct Location_t;

uint16_t getNCargo(void);

struct Cargo_t* cargoManagerNewCargo(enum kCargoType _type);

void cargoManagerFreeCargo(struct Cargo_t* _cargo);

bool newCargo(struct Location_t* loc, enum kCargoType _type, bool _addedByPlayer);

void initCargo(void);

void resetCargo(void);

void serialiseCargo(struct json_encoder* je);

void deserialiseValueCargo(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneCargo(json_decoder* jd, const char* _name, json_value_type _type);