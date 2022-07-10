#pragma once
#include "game.h"

bool save(void);

bool load(int32_t _forceSlot);

void setSlot(uint8_t _slot);

uint8_t getSlot(void);

void scanSlots(void);

bool hasSaveData(void);

bool hasWorld(uint8_t _slot);

void hardReset(void);
