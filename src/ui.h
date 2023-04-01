#pragma once
#include "game.h"
#include "player.h"
 
enum kGameMode {
  kWanderMode, 
  kTruckModeNew,
  kTruckModeLoad,
  kPlaceMode, 
  kBuildMode, 
  kPlantMode, 
  kPickMode, 
  kInspectMode, 
  kDestroyMode, // This is a break point, all Main Menu items should come after kMenuBuy
  kMenuBuy, 
  kMenuNew, 
  kMenuSell, 
  kMenuPlayer, 
  kMenuWarp, 
  kMenuExport, 
  kMenuImport, 
  kMenuSettings,
  kMenuCredits, 
  kTitles, 
  kNGameModes
};

enum kUICat {
  kUICatTool,
  kUICatPlant, 
  kUICatConv, 
  kUICatExtractor, 
  kUICatFactory, 
  kUICatUtility, 
  kUICatCargo, 
  kUICatWarp, 
  kUICatImportN, 
  kUICatImportE, 
  kUICatImportS, 
  kUICatImportW, 
  kNUICats
}; 

#define ROW_WDTH 9

// CAUTION: 9*5 was too small for cargo, bump to 10 and add runtime check
#define MAX_ROW_PER_CAT 10

#define MAX_PER_CAT (ROW_WDTH * MAX_ROW_PER_CAT)

// We need an extra buffer of kNUICats on the end,
// however no one screen ever uses every cat all at once.
// so this is obtained automatically
// But we might need more for the Setings UI 
#define MAX_ROWS (MAX_ROW_PER_CAT*kNUICats)

#define MAX_ROWS_VISIBLE 4

#define MAX_ROWS_VISIBLE_SETTINGSMENU 9

#define TOP_TITLE_OFFSET 34

void resetUnlockPing(void);

void addUIToSpriteList(void);

void rotateCursor(bool _increment);

void setCursorRotation(int8_t _value);

void moveCursor(uint32_t _button);

void setPlotCursorToWorld(enum kWorldType _wt);

uint16_t getCursorRotation(void);

enum kUICat getUIContentCategory(void);

uint16_t getUIContentID(void);

void UIDirtyBottom(void);

void UIDirtyRight(void);

void UIDirtyMain(void);

void updateUI(int _fc);

void updateLangUI(void);

void updateUITitles(int _fc);

void updateUICredits(int _fc);

void updateBlueprint(bool _beep);

void drawUITop(const char* _text, LCDBitmapDrawMode _mode);

int32_t getUnlockLevel(enum kUICat _c, int32_t _i);

int32_t getPrice(enum kUICat _c, int32_t _i);

uint16_t getOwned(enum kUICat _c, int32_t _i);

void modOwned(enum kUICat _c, int32_t _i, bool _add);

uint16_t getNSubTypes(enum kUICat _c);

uint16_t getUIIcon(enum kUICat _c, uint16_t _i);

void modTitleCursor(bool _increment);

void modCredits(bool _increment);

void modMultiplier(bool _increment);

uint16_t getTitleCursorSelected(void);

uint8_t getBuySellMultiplier(void);

void setBuySellMultiplier(uint8_t _v);

enum kBuildingType getCatBuildingSubType(enum kUICat _c);

enum kUICat getBuildingTypeCat(enum kBuildingType _b);

LCDSprite* getCannotAffordSprite(void);

void addSaveLoadProgressSprite(int32_t _doneX, int32_t _ofY);

LCDSprite* getSaveSprite(void);

LCDSprite* getLoadSprite(void);

LCDSprite* getGenSprite(void);

LCDBitmap* getInfoBitmap(void);

LCDBitmap* getSettingsMenuUIBitmap(uint32_t _i);

const char* getRotationAsString(enum kUICat _cat, int16_t _selectedID, uint16_t _rotation);

const char* toStringTutorial(enum kUITutorialStage _stage, uint16_t _n);

const char* toStringHeader(enum kUICat _c, bool _plural);

void snprintf_c(char* _buf, uint8_t _bufSize, int _n);

bool checkReturnDismissTutorialMsg(void);

void showTutorialMsg(enum kUITutorialStage _stage);

void setUIContentSettingsMenu(int32_t _row, bool _isHeader);

void setUIContentHeader(int32_t _row, enum kUICat _c);

void setUIContentItem(int32_t _row, int32_t _col, enum kUICat _c, uint16_t _i, uint16_t _r);

void setUIContentStickySelected(int32_t _row, int32_t _col, uint8_t _selID);

LCDBitmap* getUIContentItemBitmap(enum kUICat _c, uint16_t _i, uint16_t _r);

void setGameMode(enum kGameMode _mode);

enum kGameMode getGameMode(void);

void roundedRect(uint16_t _o, uint16_t _w, uint16_t _h, uint16_t _r, LCDColor _c);

void initiUI(void);

void resetUI(void);

