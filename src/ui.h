#pragma once
#include "game.h"

// All Main Menu items should come after kMenuBuy
enum kGameMode {kWander, kPlacement, kBuild, kPlantMode, kPick, kInspect, kDestroy, kMenuBuy, kMenuSell, kMenuPlayer};

//enum kMenuSelections {kMenuConveyor, kMenuSplitI, kMenuSplitL, kMenuSplitT, kMenuFilterL, kMenuTunnel, kMenuApple, kMenuCarrot, kMenuWheat, kMenuCheese, kMenuExtractor, kMenuBin};

void addUIToSpriteList(void);

void rotateCursor(bool _increment);

void moveCursor(uint32_t _button);

uint16_t getCursorRotation(void);

void UIDirtyBottom(void);

void UIDirtyRight(void);

void UIDirtyMain(void);

void updateUI(int _fc);

void updateBlueprint(void);

void drawUITop(const char* _text);

void doPurchace(void);

void doSale(void);

void doPlace(void);

void doPick(void);

void doDestroy(void);

void doPlayerMenuClick(void);

void setGameMode(enum kGameMode _mode);

enum kGameMode getGameMode(void);

void initiUI(void);

void resetUI(void);