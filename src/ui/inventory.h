#pragma once
#include "../game.h"
#include "../ui.h"

void doInventoryClick(void);

void doPlace(void);

void doPick(void);

void doDestroy(void);

void populateContentInventory(void);

void populateInfoInventory(void);

bool needsNavHint(uint8_t* _bottomID, uint8_t* _topID);