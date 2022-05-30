#pragma once
#include "game.h"

enum kCargoType {kNoCargo, kApple, kCheese};

struct Cargo_t{
  enum kCargoType m_type;
  LCDSprite* m_sprite[5]; // Only locations 1, 2 and 4 are used
  bool m_inUse;
  uint16_t m_index; // My location in the cargo array.
};

struct Location_t;

uint16_t getNCargo(void);

struct Cargo_t* cargoManagerNewCargo(void);

void cargoManagerFreeCargo(struct Cargo_t* _cargo);

bool newCargo(struct Location_t* loc, enum kCargoType _type);

void initCargo(void);