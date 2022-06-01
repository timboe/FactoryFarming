#pragma once
#include "game.h"

enum kCargoType {kNoCargo, kApple, kCheese};

struct Cargo_t{
  enum kCargoType m_type;
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Location 0 is unused
  bool m_inUse;
  uint16_t m_index; // My location in the cargo array.
};

struct Location_t;

uint16_t getNCargo(void);

struct Cargo_t* cargoManagerNewCargo(void);

void cargoManagerFreeCargo(struct Cargo_t* _cargo);

bool newCargo(struct Location_t* loc, enum kCargoType _type, bool _addedByPlayer);

void initCargo(void);