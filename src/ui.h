#pragma once
#include "game.h"

enum kMenuSelections {kMenuConveyor, kMenuSplitI, kMenuSplitL, kMenuSplitT, kMenuApple, kMenuCheese, kMenuBin};

void addUIToSpriteList(void);

void modUISelectedID(bool _increment);

void modUISelectedRotation(bool _increment);

uint16_t getUISelectedID(void);

uint16_t getUISelectedRotation(void);

void UIDirtyBottom(void);

void UIDirtyRight(void);

void updateUI(int _fc, enum kGameMode _gm);

void updateBlueprint(void);

void drawUITop(const char* _text);

void initiUI(void);