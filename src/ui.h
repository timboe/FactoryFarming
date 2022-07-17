#pragma once
#include "game.h"
#include "player.h"

// All Main Menu items should come after kMenuBuy
enum kGameMode {kWanderMode, kPlaceMode, kBuildMode, kPlantMode, kPickMode, kInspectMode, kDestroyMode, 
	            kMenuBuy, kMenuSell, kMenuPlayer, kMenuWarp, kMenuExport, kMenuImport, kNGameModes};

enum kUICat {kUICatTool, kUICatPlant, kUICatConv, kUICatExtractor, kUICatFactory, kUICatUtility, 
	         kUICatCargo, kUICatWarp, kUICatImportN, kUICatImportE, kUICatImportS, kUICatImportW, kNUICats}; 

#define ROW_WDTH 9

#define MAX_ROW_PER_CAT 3

#define MAX_PER_CAT (ROW_WDTH * MAX_ROW_PER_CAT)

#define MAX_ROWS (MAX_ROW_PER_CAT*kNUICats + kNUICats)

#define MAX_ROWS_VISIBLE 4

#define TOP_TITLE_OFFSET 34

void queueSave(void);

void addUIToSpriteList(void);

void rotateCursor(bool _increment);

void moveCursor(uint32_t _button);

uint16_t getCursorRotation(void);

enum kUICat getUIContentCategory(void);

uint16_t getUIContentID(void);

void UIDirtyBottom(void);

void UIDirtyRight(void);

void UIDirtyMain(void);

void updateUI(int _fc);

void updateBlueprint(void);

void drawUITop(const char* _text);

int32_t getUnlockCost(enum kUICat _c, int32_t _i);

int32_t getPrice(enum kUICat _c, int32_t _i);

uint16_t getOwned(enum kUICat _c, int32_t _i);

void modOwned(enum kUICat _c, int32_t _i, bool _add);

uint16_t getNSubTypes(enum kUICat _c);

uint16_t getUIIcon(enum kUICat _c, uint16_t _i);

enum kBuildingType getCatBuildingSubType(enum kUICat _c);

LCDSprite* getCannotAffordSprite(void);

LCDBitmap* getInfoBitmap(void);

const char* getRotationAsString(enum kUICat _cat, int16_t _selectedID, uint16_t _rotation);

const char* toStringTutorial(enum kUITutorialStage _stage, uint16_t _n);

bool checkReturnDismissTutorialMsg(void);

void showTutorialMsg(enum kUITutorialStage _stage);

void setUIContentHeader(int32_t _row, enum kUICat _c);

void setUIContentItem(int32_t _row, int32_t _col, enum kUICat _c, uint16_t _i, uint16_t _r);

void setUIContentStickySelected(int32_t _row, int32_t _col, uint8_t _selID);

LCDBitmap* getUIContentItemBitmap(enum kUICat _c, uint16_t _i, uint16_t _r);

void setGameMode(enum kGameMode _mode);

enum kGameMode getGameMode(void);

void roundedRect(uint16_t _o, uint16_t _w, uint16_t _h, uint16_t _r, LCDColor _c);

void initiUI(void);

void resetUI(void);