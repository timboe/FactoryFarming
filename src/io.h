#pragma once
#include "game.h"

enum kSaveLoadRequest {kDoNothing, kDoSave, kDoLoad, kDoNewWorld, kDoReset, kDoTitle};

void synchronousSave(void);

void setSlot(uint8_t _slot);

void setForceSlot(int8_t _slot);

uint8_t getSlot(void);

void scanSlots(void);

bool hasSaveData(void);

bool hasWorld(uint8_t _slot);

void hardReset(void);

void doIO(enum kSaveLoadRequest _first, enum kSaveLoadRequest _andThen);

enum kSaveLoadRequest currentIOAction(void);

bool IOOperationInProgress(void);

void enactIO(void);