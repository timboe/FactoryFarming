#pragma once
#include "game.h"

enum kCargoType {kNoCargo, kApple};

struct Cargo_t{
  enum kCargoType m_type;
  LCDSprite* m_sprite;
  bool m_inUse;
  uint16_t m_index; // My location in the cargo array.
};

uint16_t getNCargo(void);

struct Cargo_t* cargoManagerNewCargo(void);

void cargoManagerFreeCargo(struct Cargo_t* _cargo);

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type);

void initCargo(void);