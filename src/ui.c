#include <math.h>
#include "ui.h"
#include "ui/inventory.h"
#include "ui/settings.h"
#include "ui/sell.h"
#include "ui/shop.h"
#include "ui/warp.h"
#include "ui/import.h"
#include "ui/export.h"
#include "ui/new.h"
#include "sprite.h"
#include "player.h"
#include "input.h"
#include "cargo.h"
#include "building.h"
#include "location.h"
#include "generate.h"
#include "cargo.h"
#include "render.h"
#include "sound.h"
#include "io.h"
#include "buildings/special.h"
#include "buildings/factory.h"
#include "buildings/conveyor.h"
#include "buildings/extractor.h"
#include "buildings/plant.h"
#include "buildings/utility.h"

enum kGameMode m_mode = 0;

LCDSprite* m_UISpriteBottom = NULL;
LCDBitmap* m_UIBitmapBottom = NULL;

LCDSprite* m_UISpriteRight = NULL;
LCDBitmap* m_UIBitmapRight = NULL;
LCDBitmap* m_UIBitmapRightRotated = NULL;

LCDSprite* m_UISpriteSave = NULL;
LCDSprite* m_UISpriteSaveLoadProgress = NULL;
LCDSprite* m_UISpriteLoad = NULL;
LCDSprite* m_UISpriteGen = NULL;

LCDSprite* m_UISpriteBacking = NULL;

LCDBitmap* m_UIBitmapSave = NULL;
LCDBitmap* m_UIBitmapSaveLoadProgress = NULL;
LCDBitmap* m_UIBitmapLoad = NULL;
LCDBitmap* m_UIBitmapGen = NULL;

// Title Screen
LCDSprite* m_UISpriteSplash = NULL;
#ifdef DEMO
LCDSprite* m_UISpriteDemo = NULL;
#endif
LCDSprite* m_UISpriteTitleVersion = NULL;
LCDBitmap* m_UIBitmapTitleVersion = NULL;
LCDSprite* m_UISpriteTitleSelected = NULL;
int16_t m_UITitleOffset = 0;
int16_t m_UITitleSelected = 0;
LCDSprite* m_UISpriteTitleNew[3] = {NULL};
LCDSprite* m_UISpriteTitleCont[3] = {NULL};
LCDBitmap* m_UIBitmapTitleNew[3] = {NULL};
LCDBitmap* m_UIBitmapTitleCont[3] = {NULL};
//
LCDSprite* m_UISpriteTitleFac = NULL;
LCDSprite* m_UISpriteTitleFarm = NULL;
LCDSprite* m_UISpriteTitleFacFarm = NULL;
LCDSprite* m_UISpriteTitleLang = NULL;
LCDSprite* m_UISpriteTitleLangArrow = NULL;
LCDBitmap* m_UIBitmapTitleFac = NULL;
LCDBitmap* m_UIBitmapTitleFarm = NULL;
LCDBitmap* m_UIBitmapTitleFacFarm = NULL;
LCDBitmap* m_UIBitmapTitleLang = NULL;

LCDSprite* m_UISpriteTop = NULL;
LCDBitmap* m_UIBitmapTop = NULL;
int16_t m_UITopOffset = 0;
bool m_UITopVisible = false;

LCDBitmap* m_UIBitmapDev = NULL;
LCDSprite* m_UISpriteDev = NULL;

bool m_UIDirtyBottom = false;

bool m_UIDirtyRight = false;

bool m_UIDirtyMain = false;

// Menu screens

// "New"
LCDSprite* m_UISpriteNewBanner = NULL;
LCDBitmap* m_UIBitmapNewBanner[kNUICats] = {NULL};

LCDSprite* m_UISpriteNewSplash = NULL;
LCDBitmap* m_UIBitmapNewSplash = NULL;

LCDSprite* m_UISpriteNewItem = NULL;
LCDBitmap* m_UIBitmapNewItem = NULL;

LCDSprite* m_UISpriteNewText = NULL;
LCDBitmap* m_UIBitmapNewText = NULL;

// Main Menu

LCDSprite* m_UISpriteSettingsMenuItem[MAX_ROWS] = {NULL};
LCDBitmap* m_UIBitmapSettingsMenuItem[MAX_ROWS] = {NULL};

LCDSprite* m_UISpriteSettingsMenuCursor = NULL;

// Other Menus

LCDSprite* m_UISpriteCursor = NULL;

LCDSprite* m_UISpriteSelected = NULL;
LCDSprite* m_UISpriteCannotAfford = NULL;
LCDSprite* m_UISpriteInfo = NULL;
LCDBitmap* m_UIBitmapInfo = NULL;

LCDSprite* m_UISpriteScrollBarInner = NULL;
LCDBitmap* m_UIBitmapScrollBarInner = NULL;

LCDSprite* m_UISpriteScrollBarShortOuter = NULL;
LCDBitmap* m_UIBitmapScrollBarShortOuter = NULL;

LCDSprite* m_UISpriteScrollBarLongOuter = NULL;
LCDBitmap* m_UIBitmapScrollBarLongOuter = NULL;

LCDSprite* m_UISpriteFull = NULL; // Main window backing
LCDBitmap* m_UIBitmapFull = NULL;

#define TUTORIAL_WIDTH DEVICE_PIX_X
#define TUTORIAL_HEIGHT (TILE_PIX*6)

#define INGREDIENTS_WIDTH (TILE_PIX*11)
#define INGREDIENTS_HEIGHT (TILE_PIX*6)

LCDSprite* m_UISpriteIngredients = NULL; // Main window backing
LCDBitmap* m_UIBitmapIngredients[kNFactorySubTypes] = {NULL};

LCDSprite* m_UISpriteTutorialMain = NULL; 
LCDBitmap* m_UIBitmapTutorialMain = NULL;

LCDSprite* m_UISpriteTutorialHint = NULL; 
LCDBitmap* m_UIBitmapTutorialHint = NULL;

LCDSprite* m_UISpriteHeaders[kNUICats] = {NULL}; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 
LCDBitmap* m_UIBitmapHeaders[kNUICats] = {NULL}; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 

LCDSprite* m_UISpriteItems[kNUICats][MAX_PER_CAT][4] = {NULL}; // Final 4 is for rotation states
LCDBitmap* m_UIBitmapItems[kNUICats][MAX_PER_CAT][4] = {NULL};

LCDSprite* m_UISpriteStickySelected[4] = {NULL};

uint16_t m_selCol[kNGameModes] = {0}, m_selRow[kNGameModes] = {1}, m_selRotation = 0;
uint16_t m_selRowOffset[kNGameModes] = {0}, m_cursorRowAbs[kNGameModes] = {1};

LCDSprite* m_contentSprite[MAX_ROWS][ROW_WDTH] = {NULL};
LCDSprite* m_contentStickySelected[MAX_ROWS][ROW_WDTH] = {NULL};
enum kUICat m_contentCat[MAX_ROWS][ROW_WDTH] = {0};
uint16_t m_contentID[MAX_ROWS][ROW_WDTH] = {0};
uint16_t m_contentMaxRow = 0;

bool m_rowIsTitle[MAX_ROWS] = {false};

bool m_unlockPing = true;

int16_t m_creditsCounter = 0;

uint8_t m_buySellMultiplier[kNGameModes] = {1};

// Checks that the cursor selection is OK
void checkSel(void);

const char* newBannerText(enum kUICat _c);

//

void drawUIBottom(void);

void drawUIRight(void);

void drawUIMain(void);

void drawUIInspect(void);

void renderTutorialInspectRect(bool _bothSides);

void renderNewUI(void);

/// ///

void resetUnlockPing() {
  m_unlockPing = true;
}


LCDBitmap* getUIContentItemBitmap(enum kUICat _c, uint16_t _i, uint16_t _r) {
  return m_UIBitmapItems[_c][_i][_r];
}

enum kGameMode getGameMode() {
  return m_mode;
}

void UIDirtyBottom() {
  m_UIDirtyBottom = true;
}

void UIDirtyRight() {
  m_UIDirtyRight = true;
}

void UIDirtyMain() {
  m_UIDirtyMain = true;
}

void addSaveLoadProgressSprite(int32_t _doneX, int32_t _ofY) {
  char text[128];
  snprintf(text, 128, "%i/%i", (int)_doneX, (int)_ofY);
  setRoobert10();
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), text, 128, kUTF8Encoding, 0);
  pd->graphics->clearBitmap(m_UIBitmapSaveLoadProgress, kColorClear);
  pd->graphics->pushContext(m_UIBitmapSaveLoadProgress);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  pd->graphics->drawText(text, 128, kUTF8Encoding, (TILE_PIX*6-width)/2, tY());
  pd->graphics->popContext();
  pd->sprite->addSprite(m_UISpriteSaveLoadProgress);
}

LCDSprite* getSaveSprite() { return m_UISpriteSave; } 

LCDSprite* getLoadSprite() { return m_UISpriteLoad; }

LCDSprite* getGenSprite() { return m_UISpriteGen; }

uint16_t getCursorRotation() {
  return m_selRotation;
}

LCDBitmap* getSettingsMenuUIBitmap(uint32_t _i) {
  return m_UIBitmapSettingsMenuItem[_i];
}

enum kUICat getUIContentCategory() {
  enum kUICat c = m_mode;
  if (m_mode == kPlaceMode || m_mode == kPlantMode || m_mode == kBuildMode) c = kMenuPlayer;
  return m_contentCat[ m_selRow[c] ][ m_selCol[c] ];
}

uint16_t getUIContentID() {
  enum kUICat c = m_mode;
  if (m_mode == kPlaceMode || m_mode == kPlantMode || m_mode == kBuildMode) c = kMenuPlayer;
  return m_contentID[ m_selRow[c] ][ m_selCol[c] ];
}

void snprintf_c(char* _buf, uint8_t _bufSize, int _n) {
    if (_n < 1000) {
        snprintf(_buf+strlen(_buf), _bufSize, "%d", _n);
        return;
    }
    snprintf_c(_buf, _bufSize, _n / 1000);
    snprintf(_buf+strlen(_buf), _bufSize, ",%03d", _n %1000);
}

//LCDSprite* getUIHeader(enum kUICat _c) {
//  return m_UISpriteHeaders[_c];/
//}

//LCDSprite* getUIItem(enum kUICat _c, uint16_t _i, uint16_t _r) {
//  return m_UISpriteItems[_c][_i][_r];
//}

void setCursorRotation(int8_t _value) {
  m_selRotation = _value;
  sfx(kSfxRotate);
  UIDirtyRight();
  updateBlueprint(/*beep*/ false);
}

void rotateCursor(bool _increment) {
  if (_increment) {
    m_selRotation = (m_selRotation == kDirN-1 ? 0 : m_selRotation + 1);
  } else {
    m_selRotation = (m_selRotation == 0 ? kDirN-1 : m_selRotation - 1);
  }
  if (m_mode >= kMenuPlayer) UIDirtyMain();
  sfx(kSfxRotate);
  UIDirtyRight();
  updateBlueprint(/*beep*/ false);
}

void modTitleCursor(bool _increment) {
  sfx(kSfxD);
  if (_increment) {
    m_UITitleSelected = (m_UITitleSelected == N_SAVES-1 ? 0 : m_UITitleSelected + 1);
  } else {
    m_UITitleSelected = (m_UITitleSelected == 0 ? N_SAVES-1 : m_UITitleSelected - 1);
  }  
}

void modCredits(bool _increment) {
  if (_increment) {
    m_creditsCounter += 8;
  } else {
    m_creditsCounter -= 9; // We always add one anyway
  }  
}

uint8_t getBuySellMultiplier() {
  return m_buySellMultiplier[m_mode];
}

void setBuySellMultiplier(uint8_t _v) {
  m_buySellMultiplier[m_mode] = _v;
}

void modMultiplier(bool _increment) {
  if (_increment) {
    switch (m_buySellMultiplier[m_mode]) {
      case 1: case 0: m_buySellMultiplier[m_mode] = 5; sfx(kSfxRotate); break;
      case 5: m_buySellMultiplier[m_mode] = 10; sfx(kSfxRotate); break;
      case 10: m_buySellMultiplier[m_mode] = 50; sfx(kSfxRotate); break;
      case 50: m_buySellMultiplier[m_mode] = 100; sfx(kSfxRotate); break;
      case 100: m_buySellMultiplier[m_mode] = 100; sfx(kSfxNo); break;
    }
  } else {
    switch (m_buySellMultiplier[m_mode]) {
      case 1: case 0: m_buySellMultiplier[m_mode] = 1; sfx(kSfxNo); break;
      case 5: m_buySellMultiplier[m_mode] = 1; sfx(kSfxRotate); break;
      case 10: m_buySellMultiplier[m_mode] = 5; sfx(kSfxRotate); break;
      case 50: m_buySellMultiplier[m_mode] = 10; sfx(kSfxRotate); break;
      case 100: m_buySellMultiplier[m_mode] = 50; sfx(kSfxRotate); break;
    }
  }
  UIDirtyRight();
}

uint16_t getTitleCursorSelected() {
  return m_UITitleSelected;
}

void updateUICredits(int _fc) {
  int16_t y = ++m_creditsCounter;
  if (y < 0) y = 0;;

  pd->sprite->moveTo(m_UISpriteSplash, DEVICE_PIX_X/2, DEVICE_PIX_Y/2 - y/2);
  pd->sprite->setDrawMode(m_UISpriteSplash, kDrawModeInverted);

  for (int32_t i = CREDITS_FROM_ROW; i < CREDITS_TO_ROW; ++i) {
    pd->sprite->setVisible(m_UISpriteSettingsMenuItem[i], 1);
    pd->sprite->setDrawMode(m_UISpriteSettingsMenuItem[i], kDrawModeInverted);
    pd->sprite->moveTo(m_UISpriteSettingsMenuItem[i], DEVICE_PIX_X/2, DEVICE_PIX_Y + ((i - CREDITS_FROM_ROW + 1) * TILE_PIX) - y/2);
  }

  if (m_creditsCounter/2 > (CREDITS_TO_ROW - CREDITS_FROM_ROW)*TILE_PIX + DEVICE_PIX_Y) {
    setGameMode(kWanderMode);
  }

}

void updateUITitles(int _fc) {
  pd->sprite->setDrawMode(m_UISpriteSplash, kDrawModeCopy);
  #ifdef TITLE_LOGO_ONLY
  pd->sprite->setVisible(m_UISpriteSplash, 1);
  const int32_t o = round( TILE_PIX/2 * fabs( sin( 0.1f * _fc ) ) );
  pd->sprite->moveTo(m_UISpriteSplash, DEVICE_PIX_X/2, DEVICE_PIX_Y/2 - o - TILE_PIX );
  pd->sprite->moveTo(m_UISpriteTitleFac, DEVICE_PIX_X/4, TILE_PIX*4 - o);
  pd->sprite->moveTo(m_UISpriteTitleFarm, (3*DEVICE_PIX_X)/4 - TILE_PIX*2, TILE_PIX*10  - o);

  pd->sprite->setVisible(m_UISpriteTitleSelected, 0);
  for (int32_t i = 0; i < 3; ++i) {
    pd->sprite->setVisible(m_UISpriteTitleNew[i], 0);
    pd->sprite->setVisible(m_UISpriteTitleCont[i], 0);
  }
  return;
  #endif

  // Disable for now
  pd->sprite->setVisible(m_UISpriteTitleFac, false);
  pd->sprite->setVisible(m_UISpriteTitleFarm, false);

  pd->sprite->setVisible(m_UISpriteTitleLang, !m_UITitleOffset);
  pd->sprite->setVisible(m_UISpriteTitleLangArrow, _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  pd->sprite->setVisible(m_UISpriteTitleSelected, _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  pd->sprite->moveTo(m_UISpriteTitleSelected, 
    m_UITitleSelected*TILE_PIX*7 + (7*TILE_PIX)/2 + (m_UITitleSelected+1)*TILE_PIX, 
    DEVICE_PIX_Y + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset)*2);

  if (m_UITitleOffset) {
    pd->sprite->setVisible(m_UISpriteTitleLangArrow, false);

    --m_UITitleOffset;
    pd->sprite->moveTo(m_UISpriteSplash,
      DEVICE_PIX_X/2,
      DEVICE_PIX_Y/2 - (UI_TITLE_OFFSET - m_UITitleOffset) );
    pd->sprite->moveTo(m_UISpriteTitleFac,
      DEVICE_PIX_X/4,
      TILE_PIX*4 + 8 - (UI_TITLE_OFFSET - m_UITitleOffset));
    pd->sprite->moveTo(m_UISpriteTitleFarm,
      (3*DEVICE_PIX_X)/4 - TILE_PIX*2,
      TILE_PIX*10 + 8 - (UI_TITLE_OFFSET - m_UITitleOffset));
    pd->sprite->moveTo(m_UISpriteTitleFacFarm,
      DEVICE_PIX_X/2,
      TILE_PIX*13 + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset));
    for (int32_t i = 0; i < 3; ++i) {
      pd->sprite->moveTo(m_UISpriteTitleNew[i], 
        i*TILE_PIX*7 + (7*TILE_PIX)/2 + (i+1)*TILE_PIX, 
        DEVICE_PIX_Y + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset)*2);
      pd->sprite->moveTo(m_UISpriteTitleCont[i], 
        i*TILE_PIX*7 + (7*TILE_PIX)/2 + (i+1)*TILE_PIX, 
        DEVICE_PIX_Y + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset)*2);
    }
  } else {
    const int32_t offset = round( TILE_PIX/2 * fabs( sin( 0.1f * _fc ) ) );
    pd->sprite->moveTo(m_UISpriteSplash,
      DEVICE_PIX_X/2,
      DEVICE_PIX_Y/2 - offset - (3*TILE_PIX)/4 );
    pd->sprite->moveTo(m_UISpriteTitleFac,
      DEVICE_PIX_X/4,
      TILE_PIX*4 - offset);
    pd->sprite->moveTo(m_UISpriteTitleFarm,
      (3*DEVICE_PIX_X)/4 - TILE_PIX*2,
      TILE_PIX*10 - offset);
  }
}

void updateUI(int _fc) {

  if (m_mode == kMenuCredits) return updateUICredits(_fc);
  
  const bool flash = _fc % (TICK_FREQUENCY/4) == 0;
  if ((m_mode >= kMenuBuy) && flash) {
    // Flashing cursor
    UIDirtyMain();
  } else if (flash && (m_mode == kPlaceMode || m_mode == kBuildMode || m_mode == kPlantMode)) {
    // Flashing blueprint 
    const bool flashOn = _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4; 
    updateBlueprint(!flashOn);
    pd->sprite->setVisible(getPlayer()->m_blueprint[getZoom()], flashOn);
  }
  if ((_fc + TICK_OFFSET_SPRITELIST) % FAR_TICK_FREQUENCY == 0) {
    // Update bottom ticker
    UIDirtyBottom();
    UIDirtyRight();

    const enum kUITutorialStage ts = getTutorialStage();
    if (ts < TUTORIAL_DISABLED) {
      // Tutorial
      if (ts == kTutPlantCarrots && m_mode == kWanderMode && getTutorialProgress() >= 10) {
        nextTutorialStage();
      }
      // Tutorial
      if (ts == kTutBuildConveyor && m_mode == kWanderMode && getTutorialProgress()) {
        nextTutorialStage();
      }
      // Tutorial
      if (ts == kTutBuildVitamin && m_mode == kWanderMode && getTutorialProgress()) {
        nextTutorialStage();
      }
      // Tutorial
      if (ts == kTutExports && m_mode == kWanderMode && hasExported()) {
        nextTutorialStage();
      }
      // Tutorial
      if (ts == kTutImports && m_mode == kWanderMode && hasImported()) {
        nextTutorialStage();
      }
    }
    // Unlock new worlds
    #ifndef DEMO
    if (getImportBox()->m_dir != SN && m_mode == kWanderMode && getPlayer()->m_buildingsUnlockedTo >= getCactusUnlock()) {
      unlockOtherWorlds();
    }
    #endif
  }

  if (m_mode == kWanderMode) {
    if (!m_UITopVisible) {
      if (_fc % (TICK_FREQUENCY/2) == 0) {
        const bool ic = isCamouflaged();
        if      (distanceFromBuy()  < ACTIVATE_DISTANCE) drawUITop(tr(kTRTopShop), kDrawModeCopy);
        else if (distanceFromSell() < ACTIVATE_DISTANCE) drawUITop(tr(kTRTopSales), kDrawModeCopy);
        else if (!ic && distanceFromWarp() < ACTIVATE_DISTANCE) drawUITop(tr(kTRTopWarp), kDrawModeCopy);
        else if (!ic && distanceFromOut() < ACTIVATE_DISTANCE) drawUITop(tr(kTRTopExports), kDrawModeCopy); // Or Deliveries TODO
        else if (!ic && distanceFromIn() < ACTIVATE_DISTANCE) drawUITop(tr(kTRTopImports), kDrawModeCopy); // Or Shipping TODO
        else if (getSlot() == WORLD_SAVE_SLOTS-1 && distanceFromRetirement() < ACTIVATE_DISTANCE) drawUITop(tr(kTRTopCredits), kDrawModeCopy);
      }
    } else {
      if (distanceFromBuy() >= ACTIVATE_DISTANCE && 
          distanceFromSell() >= ACTIVATE_DISTANCE &&
          distanceFromWarp() >= ACTIVATE_DISTANCE &&
          distanceFromOut() >= ACTIVATE_DISTANCE &&
          distanceFromIn() >= ACTIVATE_DISTANCE &&
          distanceFromRetirement() >= ACTIVATE_DISTANCE) drawUITop(NULL, kDrawModeCopy);
    }
  }

  if (m_UITopVisible && m_UITopOffset < TOP_TITLE_OFFSET) {
    m_UITopOffset += 4;
    pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2, TILE_PIX - TOP_TITLE_OFFSET + m_UITopOffset + 1 );
  } else if (!m_UITopVisible && m_UITopOffset > 0) {
    m_UITopOffset -= 4;
    pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2, TILE_PIX - TOP_TITLE_OFFSET + m_UITopOffset + 1 );
  }

  if (m_UIDirtyRight) {
    m_UIDirtyRight = false;
    drawUIRight();
  }
  if (m_UIDirtyBottom) {
    m_UIDirtyBottom = false;
    drawUIBottom();
  }
  if (m_UIDirtyMain && m_mode >= kMenuBuy) {
    m_UIDirtyMain = false;
    drawUIMain();
  }
}

void updateBlueprint(bool _beep) {
  uint8_t zoom = getZoom();
  struct Player_t* player = getPlayer();
  enum kGameMode gm = getGameMode();
  LCDSprite* bp = player->m_blueprint[zoom];
  LCDSprite* bpRadius = player->m_blueprintRadius[zoom];

  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  uint16_t selectedRot = getCursorRotation();
  struct Location_t* pl = getPlayerLocation();
  const uint16_t owned = getOwned(selectedCat, selectedID);

  // Fence are placed one tile away, dependent on rotation
  if (selectedCat == kUICatUtility && selectedID == kFence) {
    // We handle the x and y offsets separately due to torus wrapping, blueprints are not transformed by this
    int32_t bpX = (TILE_PIX*pl->m_x + TILE_PIX/2.0);
    int32_t bpY = (TILE_PIX*pl->m_y + TILE_PIX/2.0);
    switch (selectedRot) {
      case SN: pl = getLocation(pl->m_x, pl->m_y - 1); bpY -= TILE_PIX;  break;
      case NS: pl = getLocation(pl->m_x, pl->m_y + 1); bpY += TILE_PIX; break;
      case WE: pl = getLocation(pl->m_x + 1, pl->m_y); bpX += TILE_PIX; break;
      case EW: pl = getLocation(pl->m_x - 1, pl->m_y); bpX -= TILE_PIX; break;
    }
    pd->sprite->moveTo(bp, bpX*zoom, bpY*zoom);
  }

  pd->sprite->setDrawMode(bp, kDrawModeCopy);

  if (gm == kPickMode || gm == kDestroyMode) {
    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
    switch (getRadius()) {
      case 1: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap1x1[zoom], kBitmapUnflipped); break;
      case 3: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped); break;
      case 5: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap5x5[zoom], kBitmapUnflipped); break;
    }
  } else if (gm == kPlaceMode) { // Of conveyors, cargo or utility
    if (selectedCat == kUICatUtility && (selectedID == kConveyorGrease || selectedID == kObstructionRemover)) {
      switch (getRadius()) {
        case 1: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap1x1[zoom], kBitmapUnflipped); break;
        case 3: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped); break;
        case 5: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap5x5[zoom], kBitmapUnflipped); break;
      }
    } else {
      pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    }
    bool canPlace = true;
    if (selectedCat == kUICatUtility && selectedID != kBuffferBox) selectedRot = 0; // Only buffer boxes have rotation in this cat
    switch (selectedCat) {
      case kUICatConv:; canPlace = canBePlacedConveyor(pl, selectedRot, (union kSubType) {.conveyor = selectedID});
                        pd->sprite->setImage(bp, getSprite16_byidx( CDesc[selectedID].UIIcon + selectedRot, zoom), kBitmapUnflipped); break;
      case kUICatUtility:; canPlace = canBePlacedUtility(pl, (union kSubType) {.utility = selectedID});
                           if (selectedID == kRetirement) pd->sprite->setImage(bp, getSprite48_byidx( UDesc[selectedID].sprite, zoom), kBitmapUnflipped);
                           else pd->sprite->setImage(bp, getSprite16_byidx( UDesc[selectedID].UIIcon + selectedRot, zoom), kBitmapUnflipped);
                           break;
      case kUICatCargo:; canPlace = canBePlacedCargo(pl);
                         pd->sprite->setImage(bp, getSprite16_byidx( CargoDesc[selectedID].UIIcon, zoom), kBitmapUnflipped); break;
      default: break;
    }
    canPlace &= (owned > 0);

    if (!canPlace && selectedCat == kUICatUtility && selectedID == kRetirement) {
      pd->sprite->setImage(bp, getRetirementNoBitmap(zoom), kBitmapUnflipped);
      if (_beep) sfx(kSfxNo);
    } else if (!canPlace) {
      pd->sprite->setImage(bp, getSprite16(1, 16, zoom), kBitmapUnflipped);
      if (_beep) sfx(kSfxNo);
    }

  } else if (gm == kPlantMode) { // Of crops

    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    if (owned && canBePlacedPlant(pl, (union kSubType) {.plant = selectedID})) {
      const struct Tile_t* t = getTile_fromLocation( pl );
      const int8_t gb = getGroundBonus( PDesc[selectedID].soil, (enum kGroundType) t->m_groundType );
      const int8_t wb = getWaterBonus( PDesc[selectedID].wetness, getWetness( t->m_wetness ) );
      pd->sprite->setImage(bp, getSprite16_byidx( getPlantSmilieSprite(gb+wb), zoom), kBitmapUnflipped);
    } else {
      if (_beep) {
        // Don't beep if it's another plant - this is annoying
        if ( ! (pl->m_building && pl->m_building->m_type == kPlant) ) {
          sfx(kSfxNo);
        }
      }
      pd->sprite->setImage(bp, getSprite16(1, 16, zoom), kBitmapUnflipped);
    }

  } else if (gm == kBuildMode) { // Of factories and harvesters

    if (selectedCat == kUICatExtractor) {
      switch (selectedID) {
        case kCropHarvesterLarge: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap9x9[zoom], kBitmapUnflipped); break;
        case kCropHarvesterSmall: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap7x7[zoom], kBitmapUnflipped); break;
        default: pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
      }
    } else {
      pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
    }
    bool canPlace = true;
    switch (selectedCat) {
      case kUICatExtractor:; canPlace = canBePlacedExtractor(pl, (union kSubType) {.extractor = selectedID});
                             pd->sprite->setDrawMode(bp, EDesc[selectedID].invert ? kDrawModeInverted : kDrawModeCopy);
                             pd->sprite->setImage(bp, getSprite48_byidx( EDesc[selectedID].sprite + selectedRot, zoom), kBitmapUnflipped); break;
      case kUICatFactory:; canPlace = canBePlacedFactory(pl);
                           pd->sprite->setDrawMode(bp, FDesc[selectedID].invert ? kDrawModeInverted : kDrawModeCopy);
                           pd->sprite->setImage(bp, getSprite48_byidx( FDesc[selectedID].sprite + selectedRot, zoom), kBitmapUnflipped); break;
      default: break;
    }
    canPlace &= (owned > 0);
    if (!canPlace) {
      pd->sprite->setImage(bp, getSprite48(1, 2, zoom), kBitmapUnflipped);
      if (_beep) sfx(kSfxNo);
    }

  } else { // Clear blueprint

    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 

  }
}

void addUIToSpriteList() {
  #ifdef TITLE_LOGO_ONLY
  pd->sprite->addSprite(m_UISpriteSplash);
  return;
  #endif

  #ifdef HIDE_UI
  return;
  #endif

  struct Player_t* p = getPlayer();

  if (m_mode == kMenuCredits) {
    pd->sprite->addSprite(m_UISpriteSplash);
    for (int32_t i = CREDITS_FROM_ROW; i < CREDITS_TO_ROW; ++i) {
      pd->sprite->addSprite(m_UISpriteSettingsMenuItem[i]);
    }
    return;
  } else if (m_mode == kTitles) {
    pd->sprite->addSprite(m_UISpriteSplash);
    pd->sprite->addSprite(m_UISpriteTitleVersion);
    pd->sprite->addSprite(m_UISpriteTitleFac);
    pd->sprite->addSprite(m_UISpriteTitleFarm);
    pd->sprite->addSprite(m_UISpriteTitleFacFarm);
    pd->sprite->addSprite(m_UISpriteTitleLang);
    pd->sprite->addSprite(m_UISpriteTitleLangArrow);
    #ifdef DEMO
    pd->sprite->addSprite(m_UISpriteDemo);
    #endif
    pd->sprite->addSprite(m_UISpriteTitleSelected);
    for (int32_t i = 0; i < 3; ++i) {
      #ifdef DEMO
      if (i != 1) continue;
      #endif
      if (hasSaveData(i)) pd->sprite->addSprite(m_UISpriteTitleCont[i]);
      else  pd->sprite->addSprite(m_UISpriteTitleNew[i]);
    }
    return;
  }

  pd->sprite->addSprite(m_UISpriteBottom);
  pd->sprite->addSprite(m_UISpriteTop);

  pd->sprite->addSprite(m_UISpriteRight);
  if (p->m_enableDebug) {
    pd->sprite->addSprite(m_UISpriteDev);
  }

  if (p->m_enableTutorial < TUTORIAL_FINISHED || m_mode == kInspectMode) { // 254: finished. 255: disabled
    pd->sprite->addSprite(m_UISpriteTutorialMain);
    pd->sprite->addSprite(m_UISpriteTutorialHint);
  }

  if (m_mode >= kMenuBuy) {
    pd->sprite->addSprite(m_UISpriteFull);
    pd->sprite->addSprite(m_UISpriteBacking);
    if (m_mode == kMenuNew) {
      pd->sprite->addSprite(m_UISpriteNewBanner);
      pd->sprite->addSprite(m_UISpriteNewSplash);
      pd->sprite->addSprite(m_UISpriteNewItem);
      pd->sprite->addSprite(m_UISpriteNewText);
    } else if (m_mode == kMenuSettings) {
      for (int32_t i = 0; i < MAX_ROWS; ++i) {
        pd->sprite->addSprite(m_UISpriteSettingsMenuItem[i]);
      }
      pd->sprite->addSprite(m_UISpriteSettingsMenuCursor);
      pd->sprite->addSprite(m_UISpriteScrollBarLongOuter);
      pd->sprite->addSprite(m_UISpriteScrollBarInner);
    } else {    
      pd->sprite->addSprite(m_UISpriteCursor);
      pd->sprite->addSprite(m_UISpriteSelected);
      pd->sprite->addSprite(m_UISpriteCannotAfford);
      pd->sprite->addSprite(m_UISpriteInfo);
      pd->sprite->addSprite(m_UISpriteIngredients);
      pd->sprite->addSprite(m_UISpriteScrollBarShortOuter);
      pd->sprite->addSprite(m_UISpriteScrollBarInner);
      for (int32_t i = 0; i < 4; ++i) {
        pd->sprite->addSprite(m_UISpriteStickySelected[i]);
      }
      for (enum kUICat c = 0; c < kNUICats; ++c) {

        // Not all c need to be added for all menus
        switch (m_mode) {
          case kMenuBuy: case kMenuPlayer: if (c > kUICatCargo) { continue; }; break;
          case kMenuSell: case kMenuExport: if (c != kUICatCargo) { continue; }; break;
          case kMenuWarp: if (c != kUICatWarp) { continue; }; break;
          case kMenuImport: if (c <= kUICatCargo) { continue; }; break;
          default: break;
        }

        if (m_UISpriteHeaders[c] != NULL) {
          pd->sprite->addSprite(m_UISpriteHeaders[c]);
        }
        for (int32_t i = 0; i < MAX_PER_CAT; ++i) {
          #ifdef ALLOW_ROTATED_MENU_ICONS
          for (int32_t r = 0; r < 4; ++r) {
            if (m_UISpriteItems[c][i][r] != NULL) {
              pd->sprite->addSprite(m_UISpriteItems[c][i][r]);
            }
          }
          #else
          if (m_UISpriteItems[c][i][0] != NULL) {
            pd->sprite->addSprite(m_UISpriteItems[c][i][0]);
          }
          #endif
        }
      }
    }
  }
}

void showTutorialMsg(enum kUITutorialStage _stage) {
  if (_stage >= TUTORIAL_FINISHED) return; // Finished = 254, disabled = 255

  pd->sprite->setVisible(m_UISpriteTutorialMain, 1);
  char text[128];
  uint8_t y = 0;
  pd->graphics->clearBitmap(m_UIBitmapTutorialHint, kColorClear);
  pd->graphics->pushContext(m_UIBitmapTutorialMain);
  renderTutorialInspectRect(true);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert10();
  int tutN = _stage+1, ofN = kTutBreakOne;
  if (_stage > kTutBreakOne) {
    tutN = _stage;
    ofN = kTutFinishedTwo;
  }
  #ifdef DEMO
  tutN = 1;
  ofN = 1;
  #endif
  snprintf(text, 128, tr(kTRTutStage), tutN, ofN);
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), text, 128, kUTF8Encoding, 0);
  pd->graphics->drawText(text, 128, kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  for (int32_t l = 0; l < 5; ++l) {
    const char* txt = toStringTutorial(_stage, l);
    width = pd->graphics->getTextWidth(getRoobert10(), txt, strlen(txt), kUTF8Encoding, 0);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  }
  pd->graphics->popContext();

  pd->graphics->pushContext(m_UIBitmapTutorialHint);
  y = 1;
  setRoobert10();
  for (int32_t l = 5; l < 9; ++l) {
    const char* txt = toStringTutorial(_stage, l);
    width = pd->graphics->getTextWidth(getRoobert10(), txt, strlen(txt), kUTF8Encoding, 0);
    // We have the right hand border here, so offset to the left a little more
    width += TILE_PIX/2;
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2 + 1, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - TUT_Y_SHFT + 1 +tY());
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2 - 1, TUT_Y_SPACING*(y) - TUT_Y_SHFT +tY());
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - TUT_Y_SHFT - 1 +tY());
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - TUT_Y_SHFT +tY());
  }
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);
}

bool checkReturnDismissTutorialMsg() {
  if (pd->sprite->isVisible(m_UISpriteTutorialMain)) {
    pd->sprite->setVisible(m_UISpriteTutorialMain, 0);
    sfx(kSfxB);
    // Tutorial - end of a tutorial stage
    if (getTutorialStage() == kTutFinishedOne || getTutorialStage() == kTutFinishedTwo) {
      nextTutorialStage();
    }
    return true;
  }
  return false;
}

const char* getRotationAsString(enum kUICat _cat, int16_t _selectedID, uint16_t _rotation) {
  if (_cat != kUICatConv) {
    switch (_rotation) {
      case 0: return tr(kTRN);
      case 1: return tr(kTRE);
      case 2: return tr(kTRS);
      case 3: return tr(kTRW);
    }
  }

  switch (_selectedID) {
    case kBelt: case kTunnelIn: case kTunnelOut:
      switch (_rotation) {
        case 0: return tr(kTRN);
        case 1: return tr(kTRE);
        case 2: return tr(kTRS);
        case 3: return tr(kTRW);
      }
    case kSplitI: case kFilterI: case kOverflowI:
      switch (_rotation) {
        case 0: return tr(kTRWE);
        case 1: return tr(kTRNS);
        case 2: return tr(kTRWE);
        case 3: return tr(kTRNS);
      }
    case kSplitL: case kFilterL: case kOverflowL:
      switch (_rotation) {
        case 0: return tr(kTRNE);
        case 1: return tr(kTRES);
        case 2: return tr(kTRSW);
        case 3: return tr(kTRSW);
      }
    case kSplitT:
      switch (_rotation) {
        case 0: return tr(kTRWNE);
        case 1: return tr(kTRNES);
        case 2: return tr(kTRESW);
        case 3: return tr(kTRSWN);
      }
    default: return "!";
  }
  return "!";
}

void renderTutorialInspectRect(bool _bothSides) {
  pd->graphics->clearBitmap(m_UIBitmapTutorialMain, kColorWhite);
  pd->graphics->drawRect(0, 0, TUTORIAL_WIDTH, TUTORIAL_HEIGHT, kColorBlack);
  pd->graphics->drawRect(3, 3, TUTORIAL_WIDTH-6, TUTORIAL_HEIGHT-6, kColorBlack);
  pd->graphics->drawRect(4, 4, TUTORIAL_WIDTH-8, TUTORIAL_HEIGHT-8, kColorBlack);
  pd->graphics->drawBitmap(getSprite16(9, 10, 2), TILE_PIX/2, TILE_PIX/2 - TUT_Y_SHFT, kBitmapUnflipped);
  if (_bothSides) pd->graphics->drawBitmap(getSprite16(9, 10, 2), TUTORIAL_WIDTH - (TILE_PIX/2) - TILE_PIX*2, TILE_PIX/2 - TUT_Y_SHFT, kBitmapUnflipped);
  pd->graphics->drawBitmap(getSprite16(10, 10, 1), TUTORIAL_WIDTH - (TILE_PIX/2) - TILE_PIX, TUTORIAL_HEIGHT - (TILE_PIX/2) - TILE_PIX, kBitmapUnflipped);
}

void drawUIInspect() {
  // Tutorial sprite is used for inspection too
  pd->sprite->setVisible(m_UISpriteTutorialMain, 1);
  static char text[256];
  static char textB[128];
  uint8_t y = 0;
  pd->graphics->pushContext(m_UIBitmapTutorialMain);
  renderTutorialInspectRect(false);

  // Draw grid
  for (int32_t b = 0; b < 4; ++b) {
    pd->graphics->drawRect(TILE_PIX, TUT_Y_SPACING*(3+b) - TUT_Y_SHFT, SCREEN_PIX_X-2*TILE_PIX, TUT_Y_SPACING + 1, kColorBlack);
  }

  struct Location_t* loc = getPlayerLocation();
  struct Tile_t* t = getTile_fromLocation(loc);
  if (isWaterTile(loc->m_x, loc->m_y)) {
    if (loc->m_cargo) {
      strcpy(textB, toStringSoil((enum kGroundType) t->m_groundType));
      strcat(textB, Cspace());
      strcat(textB, toStringCargoByType(loc->m_cargo->m_type, /*plural=*/false));

      snprintf(text, 256, "(%i, %i)  %s", loc->m_x, loc->m_y, textB);
    } else {
      strcpy(textB, toStringSoil((enum kGroundType) t->m_groundType));

      snprintf(text, 256, "(%i, %i)  %s", loc->m_x, loc->m_y, textB);
    }
  } else {
    if (loc->m_cargo) {
      strcpy(textB, toStringWetness(getWetness(t->m_wetness)));
      strcat(textB, space());
      strcat(textB, toStringSoil((enum kGroundType) t->m_groundType));
      strcat(textB, Cspace());
      strcat(textB, toStringCargoByType(loc->m_cargo->m_type, /*plural=*/false));

      snprintf(text, 256, "(%i, %i)  %s", loc->m_x, loc->m_y, textB);
    } else {
      strcpy(textB, toStringWetness(getWetness(t->m_wetness)));
      strcat(textB, space());
      strcat(textB, toStringSoil((enum kGroundType) t->m_groundType));

      snprintf(text, 256, "(%i, %i)  %s", loc->m_x, loc->m_y, textB);
    }
  }


  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert10();
  pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());

  if (getTile_fromLocation(loc)->m_groundType == kObstructedGround) {
    ++y;
    snprintf(text, 256, "%s", tr(kTRObstructedGroundHelp1));
    pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
    snprintf(text, 256, "%s", tr(kTRObstructedGroundHelp2));
    pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT +tY());
  }
  
  if (loc->m_building) {
    switch (loc->m_building->m_type) {
      case kConveyor:; drawUIInspectConveyor(loc->m_building); break;
      case kPlant:; drawUIInspectPlant(loc->m_building); break;
      case kUtility:; drawUIInspectUtility(loc->m_building); break;
      case kExtractor:; drawUIInspectExtractor(loc->m_building); break;
      case kFactory:; drawUIInspectFactory(loc->m_building); break;
      case kSpecial:; drawUIInspectSpecial(loc->m_building); break;
      case kNoBuilding:; case kNBuildingTypes:; break;;
    }
  }
  pd->graphics->popContext();

  pd->graphics->setDrawMode(kDrawModeCopy);
}

void drawUIBottom() {
  if (m_mode == kInspectMode) return drawUIInspect();

  static char text[256];
  setRoobert10();

  if (getPlayer()->m_enableDebug) {
    struct Player_t* p = getPlayer();
    snprintf(text, 256, "SL:%u, NT:%u, FT:%u, B:%u, C:%u, P:(%i,%i)", 
      pd->sprite->getSpriteCount(), getNearTickCount(), getFarTickCount(), getNBuildings(), getNCargo(), (int)p->m_pix_x, (int)p->m_pix_y );
    pd->graphics->pushContext(m_UIBitmapDev);
    pd->graphics->fillRect(0, 0, DEVICE_PIX_X, TILE_PIX, kColorClear);
    pd->graphics->setDrawMode(kDrawModeFillBlack);
    pd->graphics->drawText(text, 256, kUTF8Encoding, 2*TILE_PIX + 1, tY());
    pd->graphics->drawText(text, 256, kUTF8Encoding, 2*TILE_PIX, +1 +tY());
    pd->graphics->drawText(text, 256, kUTF8Encoding, 2*TILE_PIX - 1, tY());
    pd->graphics->drawText(text, 256, kUTF8Encoding, 2*TILE_PIX, -1 +tY());
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(text, 256, kUTF8Encoding, 2*TILE_PIX, tY());
    pd->graphics->popContext();
  }

  pd->graphics->pushContext(m_UIBitmapBottom);
  pd->graphics->fillRect(0, 0, DEVICE_PIX_X, TILE_PIX, kColorBlack);
  pd->graphics->setDrawMode(kDrawModeFillWhite);

  struct Location_t* loc = getPlayerLocation();
  struct Tile_t* t = getTile_fromLocation(loc);
  if (isWaterTile(loc->m_x, loc->m_y)) {

    if (loc->m_building && loc->m_cargo) {
      strcpy(text, toStringSoil((enum kGroundType)t->m_groundType));
      strcat(text, Cspace());
      strcat(text, toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));
      strcat(text, Cspace());
      strcat(text, toStringCargo(loc->m_cargo, /*plural=*/false));

      //snprintf(text, 256, "%s, %s, %s", 
      //  toStringSoil((enum kGroundType)t->m_groundType), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true), toStringCargo(loc->m_cargo, /*plural=*/false));
    } else if (loc->m_building) {
      strcpy(text, toStringSoil((enum kGroundType)t->m_groundType));
      strcat(text, Cspace());
      strcat(text, toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));

      //snprintf(text, 256, "%s, %s", 
      //  toStringSoil((enum kGroundType)t->m_groundType), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));
    } else if (loc->m_cargo) {
      strcpy(text, toStringSoil((enum kGroundType)t->m_groundType));
      strcat(text, Cspace());
      strcat(text, toStringCargo(loc->m_cargo, /*plural=*/false));

      //snprintf(text, 256, "%s, %s", 
      //  toStringSoil((enum kGroundType)t->m_groundType), toStringCargo(loc->m_cargo, /*plural=*/false));
    } else {
      strcpy(text, toStringSoil((enum kGroundType)t->m_groundType));

      //snprintf(text, 256, "%s", 
      //  toStringSoil((enum kGroundType)t->m_groundType));
    }

  } else {

    if (loc->m_building && loc->m_cargo) {
      strcpy(text, toStringWetness(getWetness(t->m_wetness)));
      strcat(text, space());
      strcat(text, toStringSoil((enum kGroundType)t->m_groundType));
      strcat(text, Cspace());
      strcat(text, toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));
      strcat(text, Cspace());
      strcat(text, toStringCargo(loc->m_cargo, /*plural=*/false));

      //snprintf(text, 256, "%s %s, %s, %s", 
      //  toStringWetness(getWetness(t->m_wetness)), toStringSoil((enum kGroundType)t->m_groundType), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true), toStringCargo(loc->m_cargo, /*plural=*/false));
    } else if (loc->m_building) {
      strcpy(text, toStringWetness(getWetness(t->m_wetness)));
      strcat(text, space());
      strcat(text, toStringSoil((enum kGroundType)t->m_groundType));
      strcat(text, Cspace());
      strcat(text, toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));

      //snprintf(text, 256, "%s %s, %s", 
      //  toStringWetness(getWetness(t->m_wetness)), toStringSoil((enum kGroundType)t->m_groundType), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));
    } else if (loc->m_cargo) {
      strcpy(text, toStringWetness(getWetness(t->m_wetness)));
      strcat(text, space());
      strcat(text, toStringSoil((enum kGroundType)t->m_groundType));
      strcat(text, Cspace());
      strcat(text, toStringCargo(loc->m_cargo, /*plural=*/false));

      //snprintf(text, 256, "%s %s, %s", 
      //  toStringWetness(getWetness(t->m_wetness)), toStringSoil((enum kGroundType)t->m_groundType), toStringCargo(loc->m_cargo, /*plural=*/false));
    } else {
      strcpy(text, toStringWetness(getWetness(t->m_wetness)));
      strcat(text, space());
      strcat(text, toStringSoil((enum kGroundType)t->m_groundType));
      //snprintf(text, 128, "%s %s", 
      //  toStringWetness(getWetness(t->m_wetness)), toStringSoil((enum kGroundType)t->m_groundType));
    }

  } 

  pd->graphics->drawText(text, 256, kUTF8Encoding, TILE_PIX/2, tY());
  pd->graphics->popContext();
}

void drawUIRight() {
  pd->graphics->pushContext(m_UIBitmapRightRotated);
  pd->graphics->fillRect(0, 0, DEVICE_PIX_Y, TILE_PIX, kColorBlack);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  if (getPlayer()->m_infiniteMoney) {
    pd->graphics->drawBitmap(getSprite16(14, 13, 1), 2*TILE_PIX, 0, kBitmapUnflipped); // Inf
  } else {
    char textM[32] = "";
    snprintf_c(textM, 32, getPlayer()->m_money);
    setRoobert10();
    pd->graphics->drawText(textM, 32, kUTF8Encoding, 2*TILE_PIX, tY());
  }
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(2, 16, 1), TILE_PIX/2, 0, kBitmapUnflipped); // Coin
  enum kGameMode gm = getGameMode();
  if (gm == kPlaceMode || gm == kPlantMode || gm == kBuildMode) {
    int16_t rotMod = (m_selRotation == 0 ? 3 : m_selRotation - 1); // We are drawing this sideways, so need to rotate it by pi/2
    const enum kUICat selectedCat = getUIContentCategory();
    const uint16_t selectedID =  getUIContentID();
    char text[16] = "";
    snprintf(text, 16, "%u", (unsigned) getOwned(selectedCat, selectedID));
    uint16_t spriteID = getUIIcon(selectedCat, selectedID);
    if (selectedCat == kUICatConv || selectedCat == kUICatExtractor || (selectedCat == kUICatUtility && selectedID == kBuffferBox)) {
      spriteID += rotMod;
    }
    if (selectedCat == kUICatExtractor && EDesc[selectedID].invert) { // No Factory here as we display the end product
      pd->graphics->setDrawMode(kDrawModeInverted);
    }
    pd->graphics->drawBitmap(getSprite16_byidx(spriteID, 1), DEVICE_PIX_Y/2 + TILE_PIX, -1, kBitmapUnflipped);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(text, 16, kUTF8Encoding, DEVICE_PIX_Y/2 + 3*TILE_PIX, tY());
    pd->graphics->setDrawMode(kDrawModeCopy);
  } else { // Compass
    #define PI 3.141592654f
    #define A_OFF (45.0f/2.0f)
    #define FF_DEG 45.0f
    struct Player_t* p = getPlayer();
    float a = (directionToBuy() * 180.0f/PI);
    uint16_t cSprite = SPRITE16_ID(8, 9);
    if ((float) fabs(a) > (180.0f - 0*FF_DEG) - A_OFF) { /*noop*/ }
    else if (a > (180.0f - 1*FF_DEG) - A_OFF) cSprite += 1;
    else if (a > (180.0f - 2*FF_DEG) - A_OFF) cSprite += 2;
    else if (a > (180.0f - 3*FF_DEG) - A_OFF) cSprite += 3;
    else if (a > (180.0f - 4*FF_DEG) - A_OFF) cSprite += 4;
    else if (a > (180.0f - 5*FF_DEG) - A_OFF) cSprite += 5;
    else if (a > (180.0f - 6*FF_DEG) - A_OFF) cSprite += 6;
    else if (a > (180.0f - 7*FF_DEG) - A_OFF) cSprite += 7;
    pd->graphics->drawBitmap(getSprite16_byidx(cSprite, 1), DEVICE_PIX_Y/2 + TILE_PIX, 0, kBitmapUnflipped);

    // Unlock?
    if (checkHasNewToShow(p) == kNewYes && !(gm == kMenuBuy || gm == kMenuSell)) {
      // unlock ping
      if (m_unlockPing) {
        sfx(kSfxSell);
        m_unlockPing = false;
      }
      pd->graphics->drawBitmap(getSprite16(11, 11, 1), DEVICE_PIX_Y/2 + 3*TILE_PIX, 0, kBitmapUnflipped);
    }
  }
  // Buy/sell multiplier
  if (gm == kMenuBuy || gm == kMenuSell) {
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    char text[16] = "";
    snprintf(text, 16, "x%u", (unsigned) m_buySellMultiplier[m_mode]);
    pd->graphics->drawText(text, 16, kUTF8Encoding, DEVICE_PIX_Y/2 + 3*TILE_PIX, tY());
    pd->graphics->setDrawMode(kDrawModeCopy);
  }

  pd->graphics->popContext();

  pd->graphics->pushContext(m_UIBitmapRight);
  pd->graphics->drawRotatedBitmap(m_UIBitmapRightRotated, 0, 0, 90.0f, 0.0f, 0.0f, 1.0f, 1.0f);  
  pd->graphics->popContext();

  //UIDirtyBottom(); // why?
}

void drawUITop(const char* _text, LCDBitmapDrawMode _mode) {
  m_UITopVisible = (_text != NULL);
  if (_text == NULL) {
    return;
  }
  pd->sprite->setDrawMode(m_UISpriteTop, _mode);
  pd->graphics->pushContext(m_UIBitmapTop);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, SCREEN_PIX_X/2 - TILE_PIX + TILE_PIX*4, TILE_PIX, TILE_PIX*2, kColorBlack);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert24();
  int32_t len = pd->graphics->getTextWidth(getRoobert24(), _text, 16, kUTF8Encoding, 0);
  pd->graphics->drawText(_text, 16, kUTF8Encoding, (SCREEN_PIX_X/2 + TILE_PIX*4 - len)/2, tY()*2);
  pd->graphics->popContext();
}

int32_t getUnlockLevel(enum kUICat _c, int32_t _i) {
  switch (_c) {
    case kUICatTool: return 0;
    case kUICatPlant: return PDesc[_i].unlock;
    case kUICatConv: return CDesc[_i].unlock;
    case kUICatExtractor: return EDesc[_i].unlock;
    case kUICatFactory: return FDesc[_i].unlock;
    case kUICatUtility: return UDesc[_i].unlock;
    case kUICatCargo: return 0;
    case kUICatWarp:; return 0;
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return 0;
    case kNUICats: return 0;
  }
  return 0;
}

int32_t getPrice(enum kUICat _c, int32_t _i) {
  switch (_c) {
    case kUICatTool: return 0;
    case kUICatPlant: return PDesc[_i].price;
    case kUICatConv: return CDesc[_i].price;
    case kUICatExtractor: return EDesc[_i].price;
    case kUICatFactory: return FDesc[_i].price;
    case kUICatUtility: return UDesc[_i].price;
    case kUICatCargo: return CargoDesc[_i].price;
    case kUICatWarp: return WDesc[_i].price;
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return 0;
    case kNUICats: return 0;
  }
  return 0;
}

uint16_t getOwned(enum kUICat _c, int32_t _i) {
  struct Player_t* p = getPlayer();
  switch (_c) {
    case kUICatTool: return 1;
    case kUICatPlant: return p->m_carryPlant[_i];
    case kUICatConv: return p->m_carryConveyor[_i];
    case kUICatExtractor: return p->m_carryExtractor[_i];
    case kUICatFactory: return p->m_carryFactory[_i];
    case kUICatUtility: return p->m_carryUtility[_i];
    case kUICatCargo: return p->m_carryCargo[_i];
    case kUICatWarp: return hasWorld(_i);
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return 0;
    case kNUICats: return 0;
  }
  return 0;
}

void modOwned(enum kUICat _c, int32_t _i, bool _add) {
  struct Player_t* p = getPlayer();
  switch (_c) {
    case kUICatTool: return;
    case kUICatPlant: p->m_carryPlant[_i] += (_add ? 1 : -1); break;
    case kUICatConv: p->m_carryConveyor[_i] += (_add ? 1 : -1); break;
    case kUICatExtractor: p->m_carryExtractor[_i] += (_add ? 1 : -1); break;
    case kUICatFactory: p->m_carryFactory[_i] += (_add ? 1 : -1); break;
    case kUICatUtility: p->m_carryUtility[_i] += (_add ? 1 : -1); break;
    case kUICatCargo: p->m_carryCargo[_i] += (_add ? 1 : -1); break;
    case kUICatWarp: break;
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; break;
    case kNUICats: break;
  }
  UIDirtyRight();
}

uint16_t getNSubTypes(enum kUICat _c) {
  switch (_c) {
    case kUICatTool: return kNToolTypes;
    case kUICatPlant: return kNPlantSubTypes;
    case kUICatConv: return kNConvSubTypes;
    case kUICatExtractor: return kNExtractorSubTypes;
    case kUICatFactory: return kNFactorySubTypes;
    case kUICatUtility: return kNUtilitySubTypes;
    case kUICatCargo: return kNCargoType;
    case kUICatWarp: return WORLD_SAVE_SLOTS;
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return kNCargoType;
    case kNUICats: return 0;
  }
  return 0;
}

enum kBuildingType getCatBuildingSubType(enum kUICat _c) {
  switch (_c) {
    case kUICatTool: return kNoBuilding;
    case kUICatPlant: return kPlant;
    case kUICatConv: return kConveyor;
    case kUICatExtractor: return kExtractor;
    case kUICatFactory: return kFactory;
    case kUICatUtility: return kUtility;
    case kUICatCargo: return kNoBuilding;
    case kUICatWarp: return kNoBuilding;
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return kNoBuilding;
    case kNUICats: return kNoBuilding;
  }
  return kNoBuilding;
}

enum kUICat getBuildingTypeCat(enum kBuildingType _b) {
  switch (_b) {
    case kNoBuilding: return kNUICats;
    case kPlant: return kUICatPlant;
    case kConveyor: return kUICatConv;
    case kExtractor: return kUICatExtractor;
    case kFactory: return kUICatFactory;
    case kUtility: return kUICatUtility;
    case kSpecial: return kNUICats;
    case kNBuildingTypes: return kNUICats;
  }
  return kNUICats;
}

uint16_t getUIIcon(enum kUICat _c, uint16_t _i) {
  switch (_c) {
    case kUICatTool: return kToolUIIcon[_i];
    case kUICatPlant: return CargoDesc[ PDesc[_i].out ].UIIcon;
    case kUICatConv: return CDesc[_i].UIIcon;
    case kUICatExtractor: return EDesc[_i].UIIcon;
    case kUICatFactory: return CargoDesc[ FDesc[_i].out ].UIIcon;
    case kUICatUtility: return UDesc[_i].UIIcon;
    case kUICatCargo: return CargoDesc[_i].UIIcon;
    case kUICatWarp: return WDesc[_i].UIIcon;
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return CargoDesc[_i].UIIcon;
    case kNUICats: break;
  }
  return 0;
}

void moveRow(bool _down) {
  const uint8_t maxRowsVisible = (m_mode == kMenuSettings ? MAX_ROWS_VISIBLE_SETTINGSMENU : MAX_ROWS_VISIBLE);
  if (_down) {
    ++m_selRow[m_mode];
    if (m_cursorRowAbs[m_mode] == maxRowsVisible-1) ++m_selRowOffset[m_mode];
    else ++m_cursorRowAbs[m_mode];
    //
    if (m_rowIsTitle[ m_selRow[m_mode] ]) {
      ++m_selRow[m_mode];
      if (m_cursorRowAbs[m_mode] == maxRowsVisible-1) ++m_selRowOffset[m_mode];
      else ++m_cursorRowAbs[m_mode];
    }
  } else { // Up
    --m_selRow[m_mode];
    if (m_cursorRowAbs[m_mode] == 0) --m_selRowOffset[m_mode];
    else --m_cursorRowAbs[m_mode];
    //
    if (m_rowIsTitle[ m_selRow[m_mode] ]) {
      --m_selRow[m_mode];
      if (m_cursorRowAbs[m_mode] == 0) --m_selRowOffset[m_mode];
      else --m_cursorRowAbs[m_mode];
    }
  }
}

void checkSel() {
  while (m_contentSprite[ m_selRow[m_mode] ][ m_selCol[m_mode] ] == NULL || m_rowIsTitle[ m_selRow[m_mode] ]) {
    if (m_rowIsTitle[ m_selRow[m_mode] ] || m_selCol[m_mode] == 0) {
      moveRow(/*down=*/false);
      m_selCol[m_mode] = ROW_WDTH-1;
    } else {
      --m_selCol[m_mode];
    }
  }
  // special, force the top to be a title
  if (m_cursorRowAbs[m_mode] == 0) {
    --m_selRowOffset[m_mode];
    ++m_cursorRowAbs[m_mode];
  }
}

void setPlotCursorToWorld(enum kWorldType _wt) {
  m_selCol[kMenuWarp] = _wt;
}

void moveCursor(uint32_t _button) {
  sfx(kSfxD);
  const uint8_t rowWidth = m_mode == kMenuSettings ? 1 : ROW_WDTH;
  if (kButtonUp    == _button) {
    if (m_selRow[m_mode] == 1) {
      // noop
    } else {
      moveRow(/*down=*/false);
    }
  } else if (kButtonDown  == _button) {
    moveRow(/*down=*/true);
  } else if (kButtonLeft  == _button) {
    if (m_selCol[m_mode] == 0 && m_selRow[m_mode] == 1) {
      // noop
    } else if (m_selCol[m_mode] == 0) {
      moveRow(/*down=*/false);
      m_selCol[m_mode] = rowWidth-1;
    } else {
      --m_selCol[m_mode];
    }
  } else if (kButtonRight == _button) {
    if (m_selCol[m_mode] == rowWidth-1 || m_contentSprite[ m_selRow[m_mode] ][ m_selCol[m_mode] + 1] == NULL) {
      moveRow(/*down=*/true);
      m_selCol[m_mode] = 0;
    } else {
      ++m_selCol[m_mode];
    }
  }
  //checkSel();
  UIDirtyMain();
}

void setUIContentSettingsMenu(int32_t _row, bool _isHeader) {
  m_contentSprite[_row][0] = m_UISpriteSettingsMenuItem[_row];
  m_contentID[_row][0] = _row;
  m_rowIsTitle[_row] = _isHeader;
  m_contentMaxRow = (_row > m_contentMaxRow ? _row : m_contentMaxRow);
}

void setUIContentHeader(int32_t _row, enum kUICat _c) {
  m_contentSprite[_row][0] = m_UISpriteHeaders[_c];
  m_rowIsTitle[_row] = true;
  m_contentMaxRow = (_row > m_contentMaxRow ? _row : m_contentMaxRow);
}

void setUIContentStickySelected(int32_t _row, int32_t _col, uint8_t _selID) {
  m_contentStickySelected[_row][_col] = m_UISpriteStickySelected[_selID];
}

void setUIContentItem(int32_t _row, int32_t _col, enum kUICat _c, uint16_t _i, uint16_t _r) {
  #ifndef ALLOW_ROTATED_MENU_ICONS
  _r = 0;
  #endif
  m_contentSprite[_row][_col] = m_UISpriteItems[_c][_i][_r];
  m_contentCat[_row][_col] = _c;
  m_contentID[_row][_col] = _i;
  m_contentMaxRow = (_row > m_contentMaxRow ? _row : m_contentMaxRow);
}

LCDSprite* getCannotAffordSprite() {
  return m_UISpriteCannotAfford;
}

LCDBitmap* getInfoBitmap() {
  return m_UIBitmapInfo;
}


void drawUIMain() {

  memset(m_contentSprite, 0, MAX_ROWS * ROW_WDTH * sizeof(LCDSprite*));
  memset(m_contentStickySelected, 0, MAX_ROWS * ROW_WDTH * sizeof(LCDSprite*));
  memset(m_rowIsTitle, 0, MAX_ROWS * sizeof(bool));
  m_contentMaxRow = 0;

  const enum kGameMode gm = getGameMode();

  const LCDBitmapDrawMode dm = (gm == kMenuPlayer ? kDrawModeInverted : kDrawModeCopy);

  // POPULATE
  bool empty = false;
  switch (gm) {
    case kMenuBuy:; populateContentBuy(); break;
    case kMenuSettings:; populateContentSettingsMenu(); break;
    case kMenuNew:; empty = true; break;
    case kMenuPlayer:; populateContentInventory(); break;
    case kMenuSell:; empty = populateContentSell(); break;
    case kMenuWarp:; populateContentWarp(); break;
    case kMenuExport:; empty = populateContentExport(); break;
    case kMenuImport:; empty = populateContentImport(); break;
    default: break;
  }

  if (!empty) checkSel();

  // DRAW
  // Mark invisible
  if (gm == kMenuSettings) {
    for (int32_t i = 0; i < MAX_ROWS; ++i) {
      pd->sprite->setVisible(m_UISpriteSettingsMenuItem[i], 0);
      pd->sprite->setDrawMode(m_UISpriteSettingsMenuItem[i], kDrawModeCopy);
    }
  } else {
    for (enum kUICat c = 0; c < kNUICats; ++c) {
      pd->sprite->setVisible(m_UISpriteHeaders[c], 0);
      for (int32_t i = 0; i < MAX_PER_CAT; ++i) {
        #ifdef ALLOW_ROTATED_MENU_ICONS
        for (int32_t r = 0; r < 4; ++r) {
          if (m_UISpriteItems[c][i][r] != NULL) {
            pd->sprite->setVisible(m_UISpriteItems[c][i][r], 0);
          }
        }
        #else
        if (m_UISpriteItems[c][i][0] != NULL) {
          pd->sprite->setVisible(m_UISpriteItems[c][i][0], 0);
        }
        #endif
      }
    }
    for (int32_t i = 0; i < 4; ++i) {
      pd->sprite->setVisible(m_UISpriteStickySelected[i], 0);
    }
  }


  #define SETTINGSMENUSTARTY (TILE_PIX*3)
  #define UISTARTX (TILE_PIX*3)
  #define UISTARTY (TILE_PIX*5)

  pd->sprite->setDrawMode(m_UISpriteFull, dm);
  pd->sprite->setDrawMode(m_UISpriteInfo, dm);
  pd->sprite->setDrawMode(m_UISpriteScrollBarShortOuter, dm);
  pd->sprite->setDrawMode(m_UISpriteScrollBarInner, dm);

  if (gm == kMenuNew) {
    return renderNewUI();
  }

  if (empty) {
    pd->sprite->setVisible(m_UISpriteSelected, 0);
    pd->sprite->setVisible(m_UISpriteCursor, 0);
    pd->sprite->setVisible(m_UISpriteIngredients, 0);
    pd->sprite->setVisible(m_UISpriteInfo, 0);
    pd->sprite->setVisible(m_UISpriteCannotAfford, 1);
    pd->sprite->setVisible(m_contentSprite[0][0], 1); // Still have the title
    pd->sprite->setDrawMode(m_contentSprite[0][0], dm);
    pd->sprite->moveTo(m_contentSprite[0][0], SCREEN_PIX_X/2 - TILE_PIX, UISTARTY);
    return;
  }
  pd->sprite->setVisible(m_UISpriteInfo, 1);

  // Render
  const uint8_t ROWS_TO_RENDER = (m_mode == kMenuSettings ? MAX_ROWS_VISIBLE_SETTINGSMENU : MAX_ROWS_VISIBLE);

  // Scroll bar
  #define SCROLL_TOP_SETTINGS (TILE_PIX*3)
  #define SCROLL_TOP (TILE_PIX*5)
  #define SCROLL_BOT (TILE_PIX*11)
  const int16_t scrollTop = (m_mode == kMenuSettings ? SCROLL_TOP_SETTINGS : SCROLL_TOP);
  float frac = 0;
  if (m_contentMaxRow-1) {
    frac = (m_selRow[m_mode] - 1) / (float)(m_contentMaxRow-1); // -1 as cannot select the top row
  }
  pd->sprite->moveTo(m_UISpriteScrollBarInner, TILE_PIX*21, scrollTop + (SCROLL_BOT-scrollTop)*frac);

  for (int32_t r = 0; r < ROWS_TO_RENDER; ++r) {
    int32_t rID = r + m_selRowOffset[m_mode];
    if (rID >= MAX_ROWS) break;
    if (m_contentSprite[rID][0] == NULL) break; // Not populated
    if (m_mode == kMenuSettings) {
      pd->sprite->setVisible(m_contentSprite[rID][0], 1);
      pd->sprite->moveTo(m_contentSprite[rID][0], SCREEN_PIX_X/2 - TILE_PIX, SETTINGSMENUSTARTY + r*TILE_PIX);
    } else if (m_rowIsTitle[rID]) {
      pd->sprite->setVisible(m_contentSprite[rID][0], 1);
      pd->sprite->setDrawMode(m_contentSprite[rID][0], dm);
      pd->sprite->moveTo(m_contentSprite[rID][0], SCREEN_PIX_X/2 - TILE_PIX, UISTARTY + r*2*TILE_PIX);
    } else {
      for (int32_t c = 0; c < ROW_WDTH; ++c) {
        if (m_contentSprite[rID][c] == NULL) break; // Not populated
        pd->sprite->setVisible(m_contentSprite[rID][c], 1);
        //pd->sprite->setDrawMode(m_contentSprite[rID][c], dm);
        pd->sprite->moveTo(m_contentSprite[rID][c], UISTARTX + c*2*TILE_PIX, UISTARTY + r*2*TILE_PIX);
        if (m_contentStickySelected[rID][c] != NULL) {
          pd->sprite->setVisible(m_contentStickySelected[rID][c], 1);
          pd->sprite->moveTo(m_contentStickySelected[rID][c], UISTARTX + c*2*TILE_PIX, UISTARTY + r*2*TILE_PIX);
        }
      }
    }
  }

  // CURSOR
  const bool visible = (getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  if (m_mode == kMenuSettings) {
    pd->sprite->setVisible(m_UISpriteSettingsMenuCursor, visible);
    pd->sprite->setDrawMode(m_UISpriteSettingsMenuCursor, dm);
    pd->sprite->moveTo(m_UISpriteSettingsMenuCursor, UISTARTX + TILE_PIX*8, SETTINGSMENUSTARTY + m_cursorRowAbs[m_mode]*TILE_PIX);
  } else {
    pd->sprite->setVisible(m_UISpriteCursor, visible);
    pd->sprite->setDrawMode(m_UISpriteCursor, dm);
    pd->sprite->moveTo(m_UISpriteCursor, UISTARTX + m_selCol[m_mode]*2*TILE_PIX, UISTARTY + m_cursorRowAbs[m_mode]*2*TILE_PIX);
  }

  // SELECTED
  if (m_mode != kMenuSettings) {
    LCDSprite* selectedSprite = m_contentSprite[ m_selRow[m_mode] ][ m_selCol[m_mode] ];
    pd->sprite->setVisible(m_UISpriteSelected, 1);
    pd->sprite->setImage(m_UISpriteSelected, pd->sprite->getImage(selectedSprite), kBitmapUnflipped);
  }

  // INGREDIENTS
  if (m_contentCat[ m_selRow[m_mode] ][ m_selCol[m_mode] ] == kUICatFactory) {
    int32_t x = UISTARTX + (m_selCol[m_mode] < ROW_WDTH/2 + 1 ? TILE_PIX*14 : TILE_PIX*4);
    pd->sprite->setImage(m_UISpriteIngredients, m_UIBitmapIngredients[getUIContentID()], kBitmapUnflipped);
    pd->sprite->moveTo(m_UISpriteIngredients, x, TILE_PIX*8);
    pd->sprite->setVisible(m_UISpriteIngredients, 1);
    //pd->sprite->setDrawMode(m_UISpriteIngredients, dm);
  } else {
   pd->sprite->setVisible(m_UISpriteIngredients, 0);
  }

  // CUSTOM TOP AREA
  switch (gm) {
    case kMenuBuy:; populateInfoBuy(visible); break;
    case kMenuPlayer:; populateInfoInventory(); break;
    case kMenuSell:; populateInfoSell(); break;
    case kMenuWarp:; populateInfoWarp(visible); break;
    case kMenuExport:; populateInfoExport(); break;
    case kMenuImport:; populateInfoImport(); break;
    default: break;
  }

}

void renderNewUI() {
  const enum kBuildingType newBuildingType = getNewBuildingType();
  const enum kUICat newCat = getBuildingTypeCat(newBuildingType);
  const uint32_t newID = getNewID();
  pd->sprite->setImage(m_UISpriteNewBanner, m_UIBitmapNewBanner[newCat], kBitmapUnflipped);

  pd->graphics->clearBitmap(m_UIBitmapNewItem, kColorClear);
  pd->graphics->pushContext(m_UIBitmapNewItem);
  pd->graphics->drawScaledBitmap(m_UIBitmapItems[newCat][newID][0], 0, 0, 2, 2);
  pd->graphics->popContext();

  pd->graphics->clearBitmap(m_UIBitmapNewText, kColorClear);
  pd->graphics->pushContext(m_UIBitmapNewText);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  const char* t0 = toStringBuilding(newBuildingType, (union kSubType) {.raw = newID}, false);
  int16_t len0 = strlen(t0);
  int32_t width0 = pd->graphics->getTextWidth(getRoobert10(), t0, len0, kUTF8Encoding, 0);

  pd->graphics->setDrawMode(kDrawModeFillWhite);

  pd->graphics->drawText(t0, len0, kUTF8Encoding, TILE_PIX*10 - width0/2 + 1, tY());
  pd->graphics->drawText(t0, len0, kUTF8Encoding, TILE_PIX*10 - width0/2 - 1, tY());
  pd->graphics->drawText(t0, len0, kUTF8Encoding, TILE_PIX*10 - width0/2, 1 +tY());
  pd->graphics->drawText(t0, len0, kUTF8Encoding, TILE_PIX*10 - width0/2, -1 +tY());
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  pd->graphics->drawText(t0, len0, kUTF8Encoding, TILE_PIX*10 - width0/2, tY());

  const char* t1 = getNewText();
  int16_t len1 = strlen(t1);
  int32_t width1 = pd->graphics->getTextWidth(getRoobert10(), t1, len1, kUTF8Encoding, 0);

  pd->graphics->setDrawMode(kDrawModeFillWhite);

  pd->graphics->drawText(t1, len1, kUTF8Encoding, TILE_PIX*10 - width1/2 + 1, TILE_PIX +tY());
  pd->graphics->drawText(t1, len1, kUTF8Encoding, TILE_PIX*10 - width1/2 - 1, TILE_PIX +tY());
  pd->graphics->drawText(t1, len1, kUTF8Encoding, TILE_PIX*10 - width1/2, TILE_PIX + 1 +tY());
  pd->graphics->drawText(t1, len1, kUTF8Encoding, TILE_PIX*10 - width1/2, TILE_PIX- 1 +tY());

  pd->graphics->setDrawMode(kDrawModeFillBlack);

  pd->graphics->drawText(t1, len1, kUTF8Encoding, TILE_PIX*10 - width1/2, TILE_PIX +tY());

  pd->graphics->setDrawMode(kDrawModeCopy);

  pd->graphics->popContext();
}

void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;
  if (m_mode == kTitles) return;

  if (m_mode == kMenuCredits) {
    m_creditsCounter = -CREDITS_DELAY;
    return;
  }

  if (_mode == kPlaceMode) {
    drawUITop(tr(kTRPlaceMode), kDrawModeCopy);
  } else if (_mode == kPlantMode) {
    drawUITop(tr(kTRPlantMode), kDrawModeCopy);
  } else if (_mode == kBuildMode) {
    drawUITop(tr(kTRBuildMode), kDrawModeCopy);
  } else if (_mode == kPickMode) {
    drawUITop(tr(kTRPickupMode), kDrawModeCopy);
  } else if (_mode == kMenuPlayer) {
    drawUITop(tr(kTRInventoryMode), kDrawModeCopy);
  } else if (_mode == kInspectMode) {
    drawUITop(tr(kTRInspectMode), kDrawModeCopy);
  } else if (_mode == kDestroyMode) {
    drawUITop(tr(kTRDestroyMode), kDrawModeCopy);
  } else if (_mode >= kMenuBuy) {
    setBuySellMultiplier(1);
  } else drawUITop(NULL, kDrawModeCopy);

  if (m_mode >= kMenuBuy) UIDirtyMain();
  UIDirtyRight(); // Just safer to always do this here
  updateBlueprint(/*beep*/ false);
  updateRenderList();
}

void resetUI() {
  for (int32_t i = 0; i < kNGameModes; ++i) {
    m_selCol[i] = 0;
    m_selRow[i] = 1;
    m_selRowOffset[i] = 0;
    m_cursorRowAbs[i] = 1;
    m_buySellMultiplier[i] = 1;
  }
  m_selRotation = 0;
  updateBlueprint(/*beep*/ false);
  drawUITop(NULL, kDrawModeCopy);
  m_UITitleOffset = UI_TITLE_OFFSET;
  m_UITopOffset = 0;
  #ifdef DEMO
  m_UITitleSelected = 1;
  #else
  m_UITitleSelected = 0;
  #endif
  if (m_UISpriteTop) {
    pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2, TILE_PIX - TOP_TITLE_OFFSET + 1 );
  }
}

void roundedRect(uint16_t _o, uint16_t _w, uint16_t _h, uint16_t _r, LCDColor _c) {
  _w -= 2*_o;
  _h -= 2*_o;
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->fillRect(_o + _r, _o + _r, _o + _w - 2*_r, _o + _h - 2*_r, _c);
  pd->graphics->drawLine(_o + _r/2, _o + _r/2, _o + _w - _r/2, _o + _r/2, _r, _c);
  pd->graphics->drawLine(_o + _r/2, _o + _h - _r/2, _o + _w - _r/2, _o + _h - _r/2, _r, _c);
  pd->graphics->drawLine(_o + _r/2, _o + _r/2, _o + _r/2, _o + _h - _r/2, _r, _c);
  pd->graphics->drawLine(_o + _w - _r/2, _o + _r/2, _o + _w - _r/2, _o + _h - _r/2, _r, _c);
}

const char* newBannerText(enum kUICat _c) {
  switch (_c) {
    case kUICatPlant: return tr(kTRNewCrop);
    case kUICatConv: return tr(kTRNewConveyor);
    case kUICatExtractor: return tr(kTRNewExtractor);
    case kUICatFactory: return tr(kTRNewFactory);
    case kUICatUtility: return tr(kTRNewUtility);
    default: break;
  }
  return "?";
}

const char* toStringHeader(enum kUICat _c, bool _plural) {
  switch (_c) {
    case kUICatTool: return _plural ? tr(kTRUICatToolPlural) : tr(kTRUICatTool);
    case kUICatPlant: return _plural ? tr(kTRUICatPlantPlural) : tr(kTRUICatPlant);
    case kUICatConv: return _plural ? tr(kTRUICatConvPlural) : tr(kTRUICatConv);
    case kUICatExtractor: return _plural ? tr(kTRUICatExtractorPlural) : tr(kTRUICatExtractor);
    case kUICatFactory: return _plural ? tr(kTRUICatFactoryPlural) : tr(kTRUICatFactory);
    case kUICatUtility: return _plural ? tr(kTRUICatUtilityPlural) : tr(kTRUICatUtility);
    case kUICatCargo: return _plural ? tr(kTRUICatCargoPlural) : tr(kTRUICatCargo);
    case kUICatWarp: return _plural ? tr(kTRUICatWarpPlural) : tr(kTRUICatWarp);
    case kUICatImportN: return _plural ? tr(kTRUICatImportNPlural) : tr(kTRUICatImportN); 
    case kUICatImportE: return _plural ? tr(kTRUICatImportEPlural) : tr(kTRUICatImportE);
    case kUICatImportS: return _plural ? tr(kTRUICatImportSPlural) : tr(kTRUICatImportS);
    case kUICatImportW: return _plural ? tr(kTRUICatImportWPlural) : tr(kTRUICatImportW);
    default: break;
  }
  return "?";
}

void updateLangUI() {
  if (!m_UIBitmapSave) return;

  for (enum kUICat c = 0; c < kNUICats; ++c) {
    pd->graphics->pushContext(m_UIBitmapHeaders[c]);
    roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    setRoobert24();
    pd->graphics->drawText(toStringHeader(c, /*plural*/ true), 32, kUTF8Encoding, TILE_PIX, tY()*2);
    pd->graphics->popContext();
  }

  const char* trText = NULL;

  trText = tr(kTRSaving);
  pd->graphics->pushContext(m_UIBitmapSave);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, DEVICE_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert24();
  int32_t tlen = pd->graphics->getTextWidth(getRoobert24(), trText, strlen(trText), kUTF8Encoding, 0);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X/2 - tlen)/2, tY()*2);
  pd->graphics->popContext();

  trText = tr(kTRGenerating);
  pd->graphics->pushContext(m_UIBitmapGen);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, DEVICE_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert24();
  tlen = pd->graphics->getTextWidth(getRoobert24(), trText, strlen(trText), kUTF8Encoding, 0);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X/2 - tlen)/2, tY()*2);
  pd->graphics->popContext();

  trText = tr(kTRLoading);
  pd->graphics->pushContext(m_UIBitmapLoad);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, DEVICE_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert24();
  tlen = pd->graphics->getTextWidth(getRoobert24(), trText, strlen(trText), kUTF8Encoding, 0);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X/2 - tlen)/2, tY()*2);
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);

  trText = tr(kTRFactory);
  pd->graphics->clearBitmap(m_UIBitmapTitleFac, kColorClear);
  pd->graphics->pushContext(m_UIBitmapTitleFac);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert24();
  tlen = pd->graphics->getTextWidth(getRoobert24(), trText, strlen(trText), kUTF8Encoding, 0) - TILE_PIX*4;
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 + 2, +2 +tY()*2);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 + 2, -2 +tY()*2);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 - 2, +2 +tY()*2);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 - 2, -2 +tY()*2);  
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2, tY()*2);
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);

  trText = tr(kTRFarming);
  pd->graphics->clearBitmap(m_UIBitmapTitleFarm, kColorClear);
  pd->graphics->pushContext(m_UIBitmapTitleFarm);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert24();
  tlen = pd->graphics->getTextWidth(getRoobert24(), trText, strlen(trText), kUTF8Encoding, 0) + TILE_PIX*4;
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 + 2, +2 +tY()*2);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 + 2, -2 +tY()*2);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 - 2, +2 +tY()*2);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2 - 2, -2 +tY()*2);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2, tY()*2);
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);

  trText = tr(kTRFactoryFarming);
  pd->graphics->clearBitmap(m_UIBitmapTitleFacFarm, kColorClear);
  pd->graphics->pushContext(m_UIBitmapTitleFacFarm);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert10();
  tlen = pd->graphics->getTextWidth(getRoobert10(), trText, strlen(trText), kUTF8Encoding, 0);
  int16_t st = DEVICE_PIX_X/2 - tlen/2;
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(st, TILE_PIX/2, st + tlen, TILE_PIX/2, TILE_PIX, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, (DEVICE_PIX_X - tlen)/2, tY());
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);

  setRoobert24();
  for (int32_t i = 0; i < kNUICats; ++i) {
    if (i == kUICatTool || i > kUICatUtility) continue;
    pd->graphics->pushContext(m_UIBitmapNewBanner[i]);
    roundedRect(1, TILE_PIX*20, TILE_PIX*2, TILE_PIX, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    const char* t = newBannerText(i);
    int16_t len = strlen(t);
    int32_t width = pd->graphics->getTextWidth(getRoobert24(), t, len, kUTF8Encoding, 0);
    pd->graphics->drawText(t, len, kUTF8Encoding, TILE_PIX*10 - width/2, tY()*2);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->popContext();
  }

  for (int32_t i = 0; i < MAX_ROWS; ++i) {
    redrawSettingsMenuLine(m_UIBitmapSettingsMenuItem[i], i);
  }

  const bool isEng = (getLanguage() == kEN);
  pd->sprite->setVisible(m_UISpriteTitleFac, !isEng);
  pd->sprite->setVisible(m_UISpriteTitleFarm, !isEng);
  pd->sprite->setVisible(m_UISpriteTitleFacFarm, !isEng);

  setRoobert10();
  for (int32_t i = 0; i < 3; ++i) {
    pd->graphics->pushContext(m_UIBitmapTitleNew[i]);
    roundedRect(0, TILE_PIX*7, TILE_PIX*1, TILE_PIX/2, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    char buttonText[64];
    trText = tr(kTRNewGame);
    snprintf(buttonText, 64, trText, i+1);
    int16_t len = strlen(buttonText);
    int32_t width = pd->graphics->getTextWidth(getRoobert10(), buttonText, len, kUTF8Encoding, 0);
    pd->graphics->drawText(buttonText, len, kUTF8Encoding, (7*TILE_PIX)/2 - width/2, tY());
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->popContext();

    pd->graphics->pushContext(m_UIBitmapTitleCont[i]);
    roundedRect(0, TILE_PIX*7, TILE_PIX*1, TILE_PIX/2, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    #ifdef DEMO
    strcpy(buttonText, tr(kTRLoadDemo));
    #else
    trText = tr(kTRContinue);
    snprintf(buttonText, 64, trText, i+1);
    #endif
    len = strlen(buttonText);
    width = pd->graphics->getTextWidth(getRoobert10(), buttonText, len, kUTF8Encoding, 0);
    pd->graphics->drawText(buttonText, len, kUTF8Encoding, (7*TILE_PIX)/2 - width/2, tY());
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->popContext();
  }

  setRoobert10();
  pd->graphics->clearBitmap(m_UIBitmapTitleLang, kColorBlack);
  pd->graphics->pushContext(m_UIBitmapTitleLang);
  pd->graphics->fillRect(1, 1, TILE_PIX*2 - 2, TILE_PIX - 2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  trText = tr(kTRLang);
  tlen = pd->graphics->getTextWidth(getRoobert10(), trText, strlen(trText), kUTF8Encoding, 0);
  pd->graphics->drawText(trText, strlen(trText), kUTF8Encoding, TILE_PIX - tlen/2, tY() + 1);
  pd->graphics->popContext();

  // Populate Ingredients

  #define ING_X_START 6
  #define ING_Y_START 6
  #define ING_ROW_MAX (INGREDIENTS_WIDTH - (ING_X_START*2))
  uint8_t textSize = pd->graphics->getFontHeight(getRoobert10());
  if (getLanguage() != kEN) textSize = 12; // Broken in noto?
  for (enum kFactorySubType fac = 0; fac < kNFactorySubTypes; ++fac) {
    pd->graphics->clearBitmap(m_UIBitmapIngredients[fac], kColorWhite);
    pd->graphics->pushContext(m_UIBitmapIngredients[fac]);
    pd->graphics->drawRect(0, 0, INGREDIENTS_WIDTH, INGREDIENTS_HEIGHT, kColorBlack);
    pd->graphics->drawRect(3, 3, INGREDIENTS_WIDTH-6, INGREDIENTS_HEIGHT-6, kColorBlack);
    pd->graphics->drawRect(4, 4, INGREDIENTS_WIDTH-8, INGREDIENTS_HEIGHT-8, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillBlack);
    setRoobert10();
    int16_t cur_x = ING_X_START;
    int16_t cur_y = ING_Y_START;
    uint16_t w = 0;
    while (true) {
      bool isFlavourText;
      const char* str = toStringIngredients(fac, w++, &isFlavourText);
      const bool isSpace = strcmp(str, " ") == 0;
      const bool isComma = strcmp(str, ",") == 0; 
      if (strcmp(str, "FIN") == 0) {
        break;
      } else if (strcmp(str, "LB") == 0) {
        cur_y += textSize;
        cur_x = ING_X_START;
      } else {
        isFlavourText ? setCooperHewitt12() : setRoobert10();
        int16_t len = strlen(str);
        int32_t width = pd->graphics->getTextWidth(isFlavourText ? getCooperHewitt12() : getRoobert10(), str, len, kUTF8Encoding, 0);
        if (cur_x + width > ING_ROW_MAX && !isComma && cur_y != ING_Y_START) { // Never split on comma
          cur_y += textSize;
          cur_x = ING_X_START;
          if (isSpace) {
            continue; // We skip a space after a new line
          }
        }
        pd->graphics->drawText(str, len, kUTF8Encoding, cur_x, cur_y +tY());
        cur_x += width;
      }
    }
    pd->graphics->popContext();
  }

}

void initiUI() {

  for (enum kUICat c = 0; c < kNUICats; ++c) {
    if (getNSubTypes(c) >= MAX_PER_CAT) {
      pd->system->error("NOT ENOUGH STORAGE FOR UI CAT %i", c);
    }
  }

  m_UISpriteTop = pd->sprite->newSprite();
  m_UISpriteSave = pd->sprite->newSprite();
  m_UISpriteSaveLoadProgress = pd->sprite->newSprite();
  m_UISpriteLoad = pd->sprite->newSprite();
  m_UISpriteGen = pd->sprite->newSprite();
  m_UISpriteBottom = pd->sprite->newSprite();
  m_UISpriteRight = pd->sprite->newSprite();

  m_UISpriteBacking = pd->sprite->newSprite();

  m_UIBitmapTop = pd->graphics->newBitmap(SCREEN_PIX_X/2 + TILE_PIX*4, TILE_PIX*2, kColorClear);
  m_UIBitmapSave = pd->graphics->newBitmap(DEVICE_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapSaveLoadProgress = pd->graphics->newBitmap(TILE_PIX*6, TILE_PIX*1, kColorClear);
  m_UIBitmapGen = pd->graphics->newBitmap(DEVICE_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapLoad = pd->graphics->newBitmap(DEVICE_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapBottom = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX, kColorBlack);
  m_UIBitmapRight = pd->graphics->newBitmap(TILE_PIX, DEVICE_PIX_Y, kColorBlack);
  m_UIBitmapRightRotated = pd->graphics->newBitmap(DEVICE_PIX_Y, TILE_PIX, kColorBlack);

  PDRect boundTopA = {.x = 0, .y = 0, .width = SCREEN_PIX_X/2 + TILE_PIX*4, .height = TILE_PIX*2};
  PDRect boundTopB = {.x = 0, .y = 0, .width = DEVICE_PIX_X/2, .height = TILE_PIX*2};
  PDRect boundSpriteSave = {.x = 0, .y = 0, .width = TILE_PIX*6, .height = TILE_PIX};

  PDRect boundSpriteBacking = {.x = 0, .y = 0, .width = SCREEN_PIX_X, .height = SCREEN_PIX_Y};

  pd->sprite->setBounds(m_UISpriteBacking, boundSpriteBacking);
  pd->sprite->setImage(m_UISpriteBacking, getMenuBacking(), kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteBacking, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteBacking, Z_INDEX_UI_BACK);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteBacking, 1);
  pd->sprite->setVisible(m_UISpriteBacking, 1);

  pd->sprite->setBounds(m_UISpriteTop, boundTopA);
  pd->sprite->setImage(m_UISpriteTop, m_UIBitmapTop, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2 - 32, TILE_PIX);
  pd->sprite->setZIndex(m_UISpriteTop, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTop, 1);
  pd->sprite->setVisible(m_UISpriteTop, 1);

  pd->sprite->setBounds(m_UISpriteSave, boundTopB);
  pd->sprite->setImage(m_UISpriteSave, m_UIBitmapSave, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteSave, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteSave, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSave, 1);
  pd->sprite->setVisible(m_UISpriteSave, 1);

  pd->sprite->setBounds(m_UISpriteSaveLoadProgress, boundSpriteSave);
  pd->sprite->setImage(m_UISpriteSaveLoadProgress, m_UIBitmapSaveLoadProgress, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteSaveLoadProgress, DEVICE_PIX_X/2, DEVICE_PIX_Y/2 + 2*TILE_PIX);
  pd->sprite->setZIndex(m_UISpriteSaveLoadProgress, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSaveLoadProgress, 1);
  pd->sprite->setVisible(m_UISpriteSaveLoadProgress, 1);

  pd->sprite->setBounds(m_UISpriteGen, boundTopB);
  pd->sprite->setImage(m_UISpriteGen, m_UIBitmapGen, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteGen, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteGen, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteGen, 1);
  pd->sprite->setVisible(m_UISpriteGen, 1);

  pd->sprite->setBounds(m_UISpriteLoad, boundTopB);
  pd->sprite->setImage(m_UISpriteLoad, m_UIBitmapLoad, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteLoad, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteLoad, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteLoad, 1);
  pd->sprite->setVisible(m_UISpriteLoad, 1);

  PDRect boundBottom = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = TILE_PIX};
  pd->sprite->setBounds(m_UISpriteBottom, boundBottom);
  pd->sprite->setImage(m_UISpriteBottom, m_UIBitmapRight, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteBottom, SCREEN_PIX_X + TILE_PIX/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteBottom, Z_INDEX_UI_BOTTOM);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteBottom, 1);

  m_UIBitmapDev = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX, kColorClear);
  m_UISpriteDev = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteDev, boundBottom);
  pd->sprite->setImage(m_UISpriteDev, m_UIBitmapDev, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteDev, Z_INDEX_UI_BOTTOM);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteDev, 1);

  PDRect boundRight = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = TILE_PIX};
  pd->sprite->setBounds(m_UISpriteRight, boundRight);
  pd->sprite->setImage(m_UISpriteRight, m_UIBitmapBottom, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteRight, DEVICE_PIX_X/2, SCREEN_PIX_Y + TILE_PIX/2);
  pd->sprite->setZIndex(m_UISpriteRight, Z_INDEX_UI_RIGHT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteRight, 1);

  // Setup menu screens

  m_UIBitmapFull = pd->graphics->newBitmap(TILE_PIX*22, TILE_PIX*12, kColorClear);
  pd->graphics->pushContext(m_UIBitmapFull);
  roundedRect(0, TILE_PIX*22, TILE_PIX*12, TILE_PIX, kColorBlack);
  roundedRect(2, TILE_PIX*22, TILE_PIX*12, TILE_PIX, kColorWhite);
  roundedRect(5, TILE_PIX*22, TILE_PIX*12, TILE_PIX, kColorBlack);
  roundedRect(11, TILE_PIX*22, TILE_PIX*12, TILE_PIX, kColorWhite);
  pd->graphics->popContext();

  PDRect fBound = {.x = 0, .y = 0, .width = TILE_PIX*22, .height = TILE_PIX*12};
  PDRect cBound = {.x = 0, .y = 0, .width = TILE_PIX*10, .height = TILE_PIX*2};
  PDRect iBound = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*2};
  PDRect infoBound = {.x = 0, .y = 0, .width = TILE_PIX*18, .height = TILE_PIX*2};
  PDRect bannerBound = {.x = 0, .y = 0, .width = TILE_PIX*20, .height = TILE_PIX*2};
  PDRect splashBound = {.x = 0, .y = 0, .width = TILE_PIX*8, .height = TILE_PIX*8};
  PDRect deviceBound = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = DEVICE_PIX_Y};
  PDRect buttonBound = {.x = 0, .y = 0, .width = TILE_PIX*7, .height = TILE_PIX};
  PDRect itemBound = {.x = 0, .y = 0, .width = TILE_PIX*4, .height = TILE_PIX*4};
  PDRect ingBound = {.x = 0, .y = 0, .width = INGREDIENTS_WIDTH, .height = INGREDIENTS_HEIGHT};
  PDRect tutBound = {.x = 0, .y = 0, .width = TUTORIAL_WIDTH, .height = TUTORIAL_HEIGHT};
  PDRect stickyBound = {.x = 0, .y = 0, .width = 38, .height = 38};
  PDRect settingsMenuBound = {.x = 0, .y = 0, .width = TILE_PIX*18, .height = TILE_PIX};
  PDRect scrollShortBound = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*8};
  PDRect scrollLongBound = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*10};

  // Titles

  m_UISpriteSplash = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteSplash, deviceBound);
  pd->sprite->setImage(m_UISpriteSplash, getSpriteSplash(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteSplash, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSplash, 1);  
  pd->sprite->moveTo(m_UISpriteSplash, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);

  #ifdef DEMO
  PDRect demoBound = {.x = 0, .y = 0, .width = 167, .height = 108};
  m_UISpriteDemo = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteDemo, deviceBound);
  pd->sprite->setImage(m_UISpriteDemo, getDemoSplash(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteDemo, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteDemo, 1);  
  pd->sprite->moveTo(m_UISpriteDemo, DEVICE_PIX_X - 167/2 - 8, 108/2 + 8);
  #endif

  m_UIBitmapTitleVersion = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*1, kColorWhite);
  pd->graphics->pushContext(m_UIBitmapTitleVersion);
  setRoobert10();
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), VERSION, 5, kUTF8Encoding, 0);
  pd->graphics->drawText(VERSION, 5, kUTF8Encoding, TILE_PIX - width/2, tY());
  pd->graphics->popContext();
  m_UISpriteTitleVersion = pd->sprite->newSprite();
  PDRect vBound = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*1};
  pd->sprite->setBounds(m_UISpriteTitleVersion, buttonBound);
  pd->sprite->setImage(m_UISpriteTitleVersion, m_UIBitmapTitleVersion, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleVersion, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleVersion, 1);
  pd->sprite->moveTo(m_UISpriteTitleVersion, 6*TILE_PIX, TILE_PIX/2);

  m_UISpriteTitleSelected = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTitleSelected, buttonBound);
  pd->sprite->setImage(m_UISpriteTitleSelected, getTitleSelectedBitmap(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleSelected, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleSelected, 1);  
  pd->sprite->moveTo(m_UISpriteTitleSelected, (7*TILE_PIX)/2 + TILE_PIX, DEVICE_PIX_Y + TILE_PIX/2);

  setRoobert10();
  for (int32_t i = 0; i < 3; ++i) {
    m_UIBitmapTitleNew[i] = pd->graphics->newBitmap(TILE_PIX*7, TILE_PIX*1, kColorClear);
    m_UISpriteTitleNew[i] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteTitleNew[i], buttonBound);
    pd->sprite->setImage(m_UISpriteTitleNew[i], m_UIBitmapTitleNew[i], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteTitleNew[i], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleNew[i], 1);  
    pd->sprite->moveTo(m_UISpriteTitleNew[i], i*TILE_PIX*7 + (7*TILE_PIX)/2 + (i+1)*TILE_PIX, DEVICE_PIX_Y + TILE_PIX/2);
    
    m_UIBitmapTitleCont[i] = pd->graphics->newBitmap(TILE_PIX*7, TILE_PIX*1, kColorClear);
    m_UISpriteTitleCont[i] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteTitleCont[i], buttonBound);
    pd->sprite->setImage(m_UISpriteTitleCont[i], m_UIBitmapTitleCont[i], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteTitleCont[i], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleCont[i], 1);  
    pd->sprite->moveTo(m_UISpriteTitleCont[i], i*TILE_PIX*7 + (7*TILE_PIX)/2 + (i+1)*TILE_PIX, DEVICE_PIX_Y - TILE_PIX/2);
  }

  PDRect boundFF = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = TILE_PIX*3};
  PDRect boundFF2 = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = TILE_PIX*2};

  m_UIBitmapTitleFac = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX*3, kColorClear);
  m_UISpriteTitleFac = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTitleFac, boundFF);
  pd->sprite->setImage(m_UISpriteTitleFac, m_UIBitmapTitleFac, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleFac, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleFac, 1);  
  pd->sprite->moveTo(m_UISpriteTitleFac, DEVICE_PIX_X/2, TILE_PIX*3);

  m_UIBitmapTitleFarm = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX*4, kColorClear);
  m_UISpriteTitleFarm = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTitleFarm, boundFF);
  pd->sprite->setImage(m_UISpriteTitleFarm, m_UIBitmapTitleFarm, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleFarm, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleFarm, 1);  
  pd->sprite->moveTo(m_UISpriteTitleFarm, DEVICE_PIX_X/2 - TILE_PIX*2, TILE_PIX*10);

  m_UIBitmapTitleFacFarm = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX*2, kColorClear);
  m_UISpriteTitleFacFarm = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTitleFacFarm, boundFF2);
  pd->sprite->setImage(m_UISpriteTitleFacFarm, m_UIBitmapTitleFacFarm, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleFacFarm, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleFacFarm, 1);  
  pd->sprite->moveTo(m_UISpriteTitleFacFarm, DEVICE_PIX_X/2, TILE_PIX*13 + TILE_PIX/2);

  PDRect boundL = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX};
  m_UIBitmapTitleLang = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX, kColorBlack);
  m_UISpriteTitleLang = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTitleLang, boundL);
  pd->sprite->setImage(m_UISpriteTitleLang, m_UIBitmapTitleLang, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleLang, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleLang, 1);  
  pd->sprite->moveTo(m_UISpriteTitleLang, TILE_PIX*3 + TILE_PIX/2, TILE_PIX*8);
  pd->sprite->setVisible(m_UISpriteTitleLang, false);

  PDRect boundLA = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*3};
  m_UISpriteTitleLangArrow = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTitleLangArrow, boundLA);
  pd->sprite->setImage(m_UISpriteTitleLangArrow, getLangArrowSprite(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteTitleLangArrow, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTitleLangArrow, 1);  
  pd->sprite->moveTo(m_UISpriteTitleLangArrow, TILE_PIX*3 + TILE_PIX/2, TILE_PIX*8);
  pd->sprite->setVisible(m_UISpriteTitleLangArrow, false);

  // Main Menu

  for (int32_t i = 0; i < MAX_ROWS; ++i) {
    m_UIBitmapSettingsMenuItem[i] = pd->graphics->newBitmap(TILE_PIX*18, TILE_PIX*1, kColorClear);
    m_UISpriteSettingsMenuItem[i] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteSettingsMenuItem[i], settingsMenuBound);
    pd->sprite->setImage(m_UISpriteSettingsMenuItem[i], m_UIBitmapSettingsMenuItem[i], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteSettingsMenuItem[i], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteSettingsMenuItem[i], 1);
  }

  m_UISpriteSettingsMenuCursor = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteSettingsMenuCursor, settingsMenuBound);
  pd->sprite->setImage(m_UISpriteSettingsMenuCursor, getSettingsMenuSelectedBitmap(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteSettingsMenuCursor, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSettingsMenuCursor, 1);

  // New stuff

  for (int32_t i = 0; i < kNUICats; ++i) {
    if (i == kUICatTool || i > kUICatUtility) continue;
    m_UIBitmapNewBanner[i] = pd->graphics->newBitmap(TILE_PIX*20, TILE_PIX*2, kColorClear);
  }

  m_UISpriteNewBanner = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteNewBanner, bannerBound);
  pd->sprite->setZIndex(m_UISpriteNewBanner, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteNewBanner, 1);  
  pd->sprite->moveTo(m_UISpriteNewBanner, SCREEN_PIX_X/2, TILE_PIX*3);

  m_UISpriteNewSplash = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteNewSplash, splashBound);
  pd->sprite->setImage(m_UISpriteNewSplash, getSpriteNew(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteNewSplash, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteNewSplash, 1);  
  pd->sprite->moveTo(m_UISpriteNewSplash, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);

  m_UIBitmapNewItem = pd->graphics->newBitmap(TILE_PIX*4, TILE_PIX*4, kColorClear);
  m_UISpriteNewItem = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteNewItem, itemBound);
  pd->sprite->setImage(m_UISpriteNewItem, m_UIBitmapNewItem, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteNewItem, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteNewItem, 1);  
  pd->sprite->moveTo(m_UISpriteNewItem, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);

  m_UIBitmapNewText = pd->graphics->newBitmap(TILE_PIX*20, TILE_PIX*2, kColorClear);
  m_UISpriteNewText = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteNewText, infoBound);
  pd->sprite->setImage(m_UISpriteNewText, m_UIBitmapNewText, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteNewText, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteNewText, 1);  
  pd->sprite->moveTo(m_UISpriteNewText, SCREEN_PIX_X/2, TILE_PIX*11);

  // Menu stuff

  m_UIBitmapScrollBarInner= pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorClear);
  pd->graphics->pushContext(m_UIBitmapScrollBarInner);
  roundedRect(4, TILE_PIX*2, TILE_PIX*2, TILE_PIX, kColorBlack);
  roundedRect(6, TILE_PIX*2, TILE_PIX*2, TILE_PIX, kColorWhite);
  pd->graphics->drawLine(10, 12, TILE_PIX*2 - 10, 12, 2, kColorBlack);
  pd->graphics->drawLine(10, 16, TILE_PIX*2 - 10, 16, 2, kColorBlack);
  pd->graphics->drawLine(10, 20, TILE_PIX*2 - 10, 20, 2, kColorBlack);
  pd->graphics->popContext();
  m_UISpriteScrollBarInner = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteScrollBarInner, iBound);
  pd->sprite->setImage(m_UISpriteScrollBarInner, m_UIBitmapScrollBarInner, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteScrollBarInner, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteScrollBarInner, 1);  
  pd->sprite->moveTo(m_UISpriteScrollBarInner, TILE_PIX*21, TILE_PIX*5);

  m_UIBitmapScrollBarShortOuter = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*8, kColorClear);
  pd->graphics->pushContext(m_UIBitmapScrollBarShortOuter);
  roundedRect(0, TILE_PIX*2, TILE_PIX*8, TILE_PIX, kColorBlack);
  roundedRect(2, TILE_PIX*2, TILE_PIX*8, TILE_PIX, kColorWhite);
  pd->graphics->popContext();
  m_UISpriteScrollBarShortOuter = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteScrollBarShortOuter, scrollShortBound);
  pd->sprite->setImage(m_UISpriteScrollBarShortOuter, m_UIBitmapScrollBarShortOuter, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteScrollBarShortOuter, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteScrollBarShortOuter, 1);  
  pd->sprite->moveTo(m_UISpriteScrollBarShortOuter, TILE_PIX*21, TILE_PIX*8);

  m_UIBitmapScrollBarLongOuter = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*10, kColorClear);
  pd->graphics->pushContext(m_UIBitmapScrollBarLongOuter);
  roundedRect(0, TILE_PIX*2, TILE_PIX*10, TILE_PIX, kColorBlack);
  roundedRect(2, TILE_PIX*2, TILE_PIX*10, TILE_PIX, kColorWhite);
  pd->graphics->popContext();
  m_UISpriteScrollBarLongOuter = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteScrollBarLongOuter, scrollLongBound);
  pd->sprite->setImage(m_UISpriteScrollBarLongOuter, m_UIBitmapScrollBarLongOuter, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteScrollBarLongOuter, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteScrollBarLongOuter, 1);  
  pd->sprite->moveTo(m_UISpriteScrollBarLongOuter, TILE_PIX*21, TILE_PIX*7);

  m_UISpriteFull = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteFull, fBound);
  pd->sprite->setImage(m_UISpriteFull, m_UIBitmapFull, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteFull, Z_INDEX_UI_B);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteFull, 1);  
  pd->sprite->moveTo(m_UISpriteFull, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);

  m_UISpriteCursor = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteCursor, iBound);
  pd->sprite->setImage(m_UISpriteCursor, getSprite16(0, 16, 2), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteCursor, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteCursor, 1);

  m_UISpriteSelected = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteSelected, iBound);
  pd->sprite->setZIndex(m_UISpriteSelected, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSelected, 1);
  pd->sprite->moveTo(m_UISpriteSelected, TILE_PIX*3, TILE_PIX*3);

  m_UISpriteCannotAfford = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteCannotAfford, iBound);
  pd->sprite->setImage(m_UISpriteCannotAfford, getSprite16(1, 16, 2), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteCannotAfford, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteCannotAfford, 1);
  pd->sprite->moveTo(m_UISpriteCannotAfford, TILE_PIX*3, TILE_PIX*3);

  m_UIBitmapInfo = pd->graphics->newBitmap(TILE_PIX*18, TILE_PIX*2, kColorClear);
  m_UISpriteInfo = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteInfo, infoBound);
  pd->sprite->setImage(m_UISpriteInfo, m_UIBitmapInfo, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteInfo, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteInfo, 1);
  pd->sprite->moveTo(m_UISpriteInfo, TILE_PIX*(9+4), TILE_PIX*3);

  // Populate Tutorial
  m_UIBitmapTutorialMain = pd->graphics->newBitmap(TUTORIAL_WIDTH, TUTORIAL_HEIGHT, kColorWhite);
  m_UIBitmapTutorialHint = pd->graphics->newBitmap(TUTORIAL_WIDTH, TUTORIAL_HEIGHT, kColorClear);

  m_UISpriteTutorialMain = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTutorialMain, tutBound);
  pd->sprite->setZIndex(m_UISpriteTutorialMain, Z_INDEX_MAX);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTutorialMain, 1);
  pd->sprite->moveTo(m_UISpriteTutorialMain, TUTORIAL_WIDTH/2, TILE_PIX*12);
  pd->sprite->setImage(m_UISpriteTutorialMain, m_UIBitmapTutorialMain, kBitmapUnflipped);

  m_UISpriteTutorialHint = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteTutorialHint, tutBound);
  pd->sprite->setZIndex(m_UISpriteTutorialHint, Z_INDEX_UI_BB);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTutorialHint, 1);
  pd->sprite->moveTo(m_UISpriteTutorialHint, TUTORIAL_WIDTH/2, TILE_PIX*12);
  pd->sprite->setImage(m_UISpriteTutorialHint, m_UIBitmapTutorialHint, kBitmapUnflipped);

  for (uint32_t i = 0; i < 4; ++i) {
    m_UISpriteStickySelected[i] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteStickySelected[i], stickyBound);
    pd->sprite->setZIndex(m_UISpriteStickySelected[i], Z_INDEX_UI_TT);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteStickySelected[i], 1);
    pd->sprite->setImage(m_UISpriteStickySelected[i], getStickySelectedBitmap(), kBitmapUnflipped);
  }

  // Populate main UI

  setRoobert24();

  for (enum kUICat c = 0; c < kNUICats; ++c) {
    m_UIBitmapHeaders[c] = pd->graphics->newBitmap(TILE_PIX*18, TILE_PIX*2, kColorClear);
    m_UISpriteHeaders[c] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteHeaders[c], cBound);
    pd->sprite->setImage(m_UISpriteHeaders[c], m_UIBitmapHeaders[c], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteHeaders[c], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteHeaders[c], 1);

    for (int32_t i = 0; i < getNSubTypes(c); ++i) {

      // Exclude placeholders
      switch (c) {
        case kUICatPlant: if (PDesc[i].sprite == 0) { continue; }; break;
        case kUICatConv: if (CDesc[i].UIIcon == 0) { continue; }; break;
        case kUICatExtractor: if (EDesc[i].UIIcon == 0) { continue; }; break;
        case kUICatFactory: if (FDesc[i].UIIcon == 0) { continue; }; break;
        case kUICatUtility: if (UDesc[i].UIIcon == 0) { continue; }; break;
        case kUICatCargo: case kUICatImportN: case kUICatImportE: case kUICatImportS: case kUICatImportW:
          if (CargoDesc[i].UIIcon == 0) { continue; }; break;
        default: break;
      }

      uint16_t spriteID = getUIIcon(c, i);
      for (int32_t r = 0; r < 4; ++r) {

        m_UIBitmapItems[c][i][r] = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorClear);
        pd->graphics->pushContext(m_UIBitmapItems[c][i][r]);
        if (c == kUICatPlant) { // Crops are seeds in a packet
          pd->graphics->drawBitmap(getSprite16(3, 16, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID, 1), TILE_PIX/2, TILE_PIX/2, kBitmapUnflipped);
        } else if (c == kUICatFactory) {
          pd->graphics->setDrawMode(FDesc[i].invert ? kDrawModeInverted : kDrawModeCopy);
          pd->graphics->drawBitmap(getSprite16_byidx(FDesc[i].UIIcon + r, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->setDrawMode(kDrawModeCopy);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID, 1), TILE_PIX/2, TILE_PIX/2, kBitmapUnflipped);
        } else if (c == kUICatExtractor) {
          pd->graphics->setDrawMode(EDesc[i].invert ? kDrawModeInverted : kDrawModeCopy);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID + r, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->setDrawMode(kDrawModeCopy);
        } else {
          if (c != kUICatConv && c != kUICatWarp
              && !(c == kUICatUtility && i == kBuffferBox)
              && !(c == kUICatUtility && i == kRetirement)
              && !(c == kUICatUtility && i == kStorageBox)) {
            roundedRect(1, TILE_PIX*2, TILE_PIX*2, TILE_PIX/2, kColorBlack);
            roundedRect(3, TILE_PIX*2, TILE_PIX*2, TILE_PIX/2, kColorWhite);
            pd->graphics->setDrawMode(kDrawModeNXOR);
          }

          pd->graphics->drawBitmap(getSprite16_byidx(spriteID + r, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->setDrawMode(kDrawModeCopy);
        }
        pd->graphics->popContext();

        m_UISpriteItems[c][i][r] = pd->sprite->newSprite();
        pd->sprite->setBounds(m_UISpriteItems[c][i][r], iBound);
        pd->sprite->setImage(m_UISpriteItems[c][i][r], m_UIBitmapItems[c][i][r], kBitmapUnflipped);
        pd->sprite->setZIndex(m_UISpriteItems[c][i][r], Z_INDEX_UI_M);
        pd->sprite->setIgnoresDrawOffset(m_UISpriteItems[c][i][r], 1);

        // Tools, Crops, Cargo, Warp, Import don't have any rotation 
        if (c == kUICatTool || c == kUICatPlant || c >= kUICatCargo || (c == kUICatUtility && i != kBuffferBox)) { 
          break;
        }
      }
    }
  }

  // Populate Ingredients

  m_UISpriteIngredients = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteIngredients, ingBound);
  pd->sprite->setZIndex(m_UISpriteIngredients, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteIngredients, 1);

  for (enum kFactorySubType fac = 0; fac < kNFactorySubTypes; ++fac) {
    m_UIBitmapIngredients[fac] = pd->graphics->newBitmap(INGREDIENTS_WIDTH, INGREDIENTS_HEIGHT, kColorWhite);
  }

  updateLangUI();

  UIDirtyBottom();
  UIDirtyRight();

}


const char* toStringTutorial(enum kUITutorialStage _stage, uint16_t _n) {
  switch (_stage) {
    case kTutWelcomeBuySeeds:;
      switch (_n) {
        #ifdef DEMO
        case 0: return tr(kTRTutDemo0);
        case 1: return tr(kTRTutDemo1);
        case 2: return tr(kTRTutDemo2);
        case 3: return tr(kTRTutDemo3);
        case 4: return tr(kTRTutDemo4);

        case 5: return "";
        case 6: return "";
        case 7: return "";
        case 8: return "";
        #else
        case 0: return tr(kTRTut0_0);
        case 1: return tr(kTRTut0_1);
        case 2: return tr(kTRTut0_2);
        case 3: return tr(kTRTut0_3);
        case 4: return tr(kTRTut0_4);

        case 5: return tr(kTRTut0_5);
        case 6: return tr(kTRTut0_6);
        case 7: return tr(kTRTut0_7);
        case 8: return tr(kTRTut0_8);
        #endif
      }
    case kTutSeeObjective:;
      switch (_n) {
        case 0: return tr(kTRTut1_0);
        case 1: return tr(kTRTut1_1);
        case 2: return tr(kTRTut1_2);
        case 3: return tr(kTRTut1_3);
        case 4: return tr(kTRTut1_4);
          
        case 5: return tr(kTRTut1_5);
        case 6: return tr(kTRTut1_6);
        case 7: return tr(kTRTut1_7);
        case 8: return tr(kTRTut1_8);
      }
    case kTutPlantCarrots:;
      switch (_n) {
        case 0: return tr(kTRTut2_0);
        case 1: return tr(kTRTut2_1);
        case 2: return tr(kTRTut2_2);
        case 3: return tr(kTRTut2_3);
        case 4: return tr(kTRTut2_4);
          
        case 5: return tr(kTRTut2_5);
        case 6: return tr(kTRTut2_6);
        case 7: return tr(kTRTut2_7);
        case 8: return tr(kTRTut2_8);
      }
    case kTutGetCarrots:;
      switch (_n) {
        case 0: return tr(kTRTut3_0);
        case 1: return tr(kTRTut3_1);
        case 2: return tr(kTRTut3_2);
        case 3: return tr(kTRTut3_3);
        case 4: return tr(kTRTut3_4);
          
        case 5: return tr(kTRTut3_5);
        case 6: return tr(kTRTut3_6);
        case 7: return tr(kTRTut3_7);
        case 8: return tr(kTRTut3_8);
      }
    case kTutSellCarrots:;
      switch (_n) {
        case 0: return tr(kTRTut4_0);
        case 1: return tr(kTRTut4_1);
        case 2: return tr(kTRTut4_2);
        case 3: return tr(kTRTut4_3);
        case 4: return tr(kTRTut4_4);
          
        case 5: return tr(kTRTut4_5);
        case 6: return tr(kTRTut4_6);
        case 7: return tr(kTRTut4_7);
        case 8: return tr(kTRTut4_8);
      }
    case kTutBuildHarvester:;
      switch (_n) {
        case 0: return tr(kTRTut5_0);
        case 1: return tr(kTRTut5_1);
        case 2: return tr(kTRTut5_2);
        case 3: return tr(kTRTut5_3);
        case 4: return tr(kTRTut5_4);
          
        case 5: return tr(kTRTut5_5);
        case 6: return tr(kTRTut5_6);
        case 7: return tr(kTRTut5_7);
        case 8: return tr(kTRTut5_8);
      }
    case kTutBuildConveyor:;
      switch (_n) {
        case 0: return tr(kTRTut6_0);
        case 1: return tr(kTRTut6_1);
        case 2: return tr(kTRTut6_2);
        case 3: return tr(kTRTut6_3);
        case 4: return tr(kTRTut6_4);
          
        case 5: return tr(kTRTut6_5);
        case 6: return tr(kTRTut6_6);
        case 7: return tr(kTRTut6_7);
        case 8: return tr(kTRTut6_8);
      }
    case kTutBuildQuarry:
      switch (_n) {
        case 0: return tr(kTRTut7_0);
        case 1: return tr(kTRTut7_1);
        case 2: return tr(kTRTut7_2);
        case 3: return tr(kTRTut7_3);
        case 4: return tr(kTRTut7_4);
          
        case 5: return tr(kTRTut7_5);
        case 6: return tr(kTRTut7_6);
        case 7: return tr(kTRTut7_7);
        case 8: return tr(kTRTut7_8);
      }
    case kTutBuildVitamin:
      switch (_n) {
        case 0: return tr(kTRTut8_0);
        case 1: return tr(kTRTut8_1);
        case 2: return tr(kTRTut8_2);
        case 3: return tr(kTRTut8_3);
        case 4: return tr(kTRTut8_4);
          
        case 5: return tr(kTRTut8_5);
        case 6: return tr(kTRTut8_6);
        case 7: return tr(kTRTut8_7);
        case 8: return tr(kTRTut8_8);
      }
    case kTutFinishedOne:
      switch (_n) {
        case 0: return tr(kTRTut9_0);
        case 1: return tr(kTRTut9_1);
        case 2: return tr(kTRTut9_2);
        case 3: return tr(kTRTut9_3);
        case 4: return tr(kTRTut9_4);
          
        case 5: return " ";
        case 6: return " ";
        case 7: return " ";
        case 8: return " ";
      }
      ///
    case kTutNewPlots:
      switch (_n) {
        case 0: return tr(kTRTut10_0);
        case 1: return tr(kTRTut10_1);
        case 2: return tr(kTRTut10_2);
        case 3: return tr(kTRTut10_3);
        case 4: return tr(kTRTut10_4);
          
        case 5: return tr(kTRTut10_5);
        case 6: return tr(kTRTut10_6);
        case 7: return tr(kTRTut10_7);  
        case 8: return tr(kTRTut10_8);
      }
    case kTutExports:
      switch (_n) {
        case 0: return tr(kTRTut11_0);
        case 1: return tr(kTRTut11_1);
        case 2: return tr(kTRTut11_2);
        case 3: return tr(kTRTut11_3);
        case 4: return tr(kTRTut11_4);
          
        case 5: return tr(kTRTut11_5);
        case 6: return tr(kTRTut11_6);
        case 7: return tr(kTRTut11_7);
        case 8: return tr(kTRTut11_8);
      }
    case kTutImports:
      switch (_n) {
        case 0: return tr(kTRTut12_0);
        case 1: return tr(kTRTut12_1);
        case 2: return tr(kTRTut12_2);
        case 3: return tr(kTRTut12_3);
        case 4: return tr(kTRTut12_4);
          
        case 5: return tr(kTRTut12_5);
        case 6: return tr(kTRTut12_6);
        case 7: return tr(kTRTut12_7);
        case 8: return tr(kTRTut12_8);
      }
    case kTutFinishedTwo:
      switch (_n) {
        case 0: return tr(kTRTut13_0);
        case 1: return tr(kTRTut13_1);
        case 2: return tr(kTRTut13_2);
        case 3: return tr(kTRTut13_3);
        case 4: return tr(kTRTut13_4);
          
        case 5: return " ";
        case 6: return " ";
        case 7: return " ";
        case 8: return " ";
      }
    default: return " ";
  }
  return " ";
}
