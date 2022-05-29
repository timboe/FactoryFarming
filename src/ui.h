#pragma once
#include "game.h"

enum kMenuSelections {kMenuConveyor, kMenuSplitI, kMenuSplitL, kMenuSplitT};

LCDSprite* getUISpriteBottom(void);

LCDSprite* getUISpriteRight(void);

void modUISelectedID(bool _increment);

void modUISelectedRotation(bool _increment);

uint16_t getUISelectedID(void);

uint16_t getUISelectedRotation(void);

void UIDirtyBottom(void);

void UIDirtyRight(void);

void updateUI(void);

void updateBlueprint(void);

void initiUI(void);