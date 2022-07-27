#pragma once
#include "game.h"
#include "player.h"

// All Main Menu items should come after kMenuBuy
enum kGameMode {kWanderMode, kPlaceMode, kBuildMode, kPlantMode, kPickMode, kInspectMode, kDestroyMode, 
	            kMenuBuy, kMenuNew, kMenuSell, kMenuPlayer, kMenuWarp, kMenuExport, kMenuImport, kMenuMain, kTitles, kNGameModes};

enum kUICat {kUICatTool, kUICatPlant, kUICatConv, kUICatExtractor, kUICatFactory, kUICatUtility, 
	         kUICatCargo, kUICatWarp, kUICatImportN, kUICatImportE, kUICatImportS, kUICatImportW, kNUICats}; 

#define ROW_WDTH 9

#define MAX_ROW_PER_CAT 3

#define MAX_PER_CAT (ROW_WDTH * MAX_ROW_PER_CAT)

// We need an extra buffer of kNUICats on the end,
// however no one screen ever uses every cat all at once.
// so this is obtained automatically
#define MAX_ROWS (MAX_ROW_PER_CAT*kNUICats)

#define MAX_ROWS_VISIBLE 4

#define MAX_ROWS_VISIBLE_MAINMENU 9

#define TOP_TITLE_OFFSET 34

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

void updateUITitles(int _fc);

void updateBlueprint(void);

void drawUITop(const char* _text);

int32_t getUnlockCost(enum kUICat _c, int32_t _i);

int32_t getPrice(enum kUICat _c, int32_t _i);

uint16_t getOwned(enum kUICat _c, int32_t _i);

void modOwned(enum kUICat _c, int32_t _i, bool _add);

uint16_t getNSubTypes(enum kUICat _c);

uint16_t getUIIcon(enum kUICat _c, uint16_t _i);

void modTitleCursor(bool _increment);

uint16_t getTitleCursorSelected(void);

enum kBuildingType getCatBuildingSubType(enum kUICat _c);

LCDSprite* getCannotAffordSprite(void);

LCDSprite* getSaveSprite(void);

LCDSprite* getLoadSprite(void);

LCDSprite* getGenSprite(void);

LCDBitmap* getInfoBitmap(void);

LCDBitmap* getMainmenuUIBitmap(uint32_t _i);

const char* getRotationAsString(enum kUICat _cat, int16_t _selectedID, uint16_t _rotation);

const char* toStringTutorial(enum kUITutorialStage _stage, uint16_t _n);

bool checkReturnDismissTutorialMsg(void);

void showTutorialMsg(enum kUITutorialStage _stage);

void setUIContentMainMenu(int32_t _row, bool _isHeader);

void setUIContentHeader(int32_t _row, enum kUICat _c);

void setUIContentItem(int32_t _row, int32_t _col, enum kUICat _c, uint16_t _i, uint16_t _r);

void setUIContentStickySelected(int32_t _row, int32_t _col, uint8_t _selID);

LCDBitmap* getUIContentItemBitmap(enum kUICat _c, uint16_t _i, uint16_t _r);

void setGameMode(enum kGameMode _mode);

enum kGameMode getGameMode(void);

void roundedRect(uint16_t _o, uint16_t _w, uint16_t _h, uint16_t _r, LCDColor _c);

void initiUI(void);

void resetUI(void);

