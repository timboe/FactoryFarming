#pragma once
#include "game.h"

enum kSaveLoadRequest {kDoNothing, kDoSave, kDoLoad, kDoNewWorld, kDoSaveDelete, kDoScanSlots, kDoResetPlayer, kDoTitle};

void synchronousSave(void);

void setSave(uint8_t _save);

void setSlot(uint8_t _slot);

void setForceSlot(int8_t _slot);

uint8_t getSlot(void);

uint8_t getSave(void);

void scanSlots(void);

bool hasSaveData(uint8_t _save);

bool hasWorld(uint8_t _slot);

void hardReset(void);

void doIO(enum kSaveLoadRequest _first, enum kSaveLoadRequest _andThen);

enum kSaveLoadRequest currentIOAction(void);

bool IOOperationInProgress(void);

void enactIO(void);