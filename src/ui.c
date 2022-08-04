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
#include "io.h"
#include "buildings/special.h"
#include "buildings/factory.h"
#include "buildings/conveyor.h"
#include "buildings/extractor.h"
#include "buildings/plant.h"
#include "buildings/utility.h"

enum kGameMode m_mode;

LCDSprite* m_UISpriteBottom;
LCDBitmap* m_UIBitmapBottom;

LCDSprite* m_UISpriteRight;
LCDBitmap* m_UIBitmapRight;
LCDBitmap* m_UIBitmapRightRotated;

LCDSprite* m_UISpriteSave;
LCDSprite* m_UISpriteLoad;
LCDSprite* m_UISpriteGen;

LCDBitmap* m_UIBitmapSave;
LCDBitmap* m_UIBitmapLoad;
LCDBitmap* m_UIBitmapGen;

// Title Screen
LCDSprite* m_UISpriteSplash;
LCDSprite* m_UISpriteTitleSelected;
int16_t m_UITitleOffset = 0;
int16_t m_UITitleSelected = 0;
LCDSprite* m_UISpriteTitleNew[3];
LCDSprite* m_UISpriteTitleCont[3];
LCDBitmap* m_UIBitmapTitleNew[3];
LCDBitmap* m_UIBitmapTitleCont[3];

LCDSprite* m_UISpriteTop;
LCDBitmap* m_UIBitmapTop;
int16_t m_UITopOffset = 0;
bool m_UITopVisible = false;

LCDBitmap* m_UIBitmapDev;
LCDSprite* m_UISpriteDev;

bool m_UIDirtyBottom = false;

bool m_UIDirtyRight = false;

bool m_UIDirtyMain = false;

// Menu screens

// "New"
LCDSprite* m_UISpriteNewBanner;
LCDBitmap* m_UIBitmapNewBanner[kNUICats] = {NULL};

LCDSprite* m_UISpriteNewSplash;
LCDBitmap* m_UIBitmapNewSplash;

LCDSprite* m_UISpriteNewItem;
LCDBitmap* m_UIBitmapNewItem;

LCDSprite* m_UISpriteNewText;
LCDBitmap* m_UIBitmapNewText;

// Main Menu

LCDSprite* m_UISpriteMainMenuItem[MAX_ROWS];
LCDBitmap* m_UIBitmapMainMenuItem[MAX_ROWS];

LCDSprite* m_UISpriteMainMenuCursor;

// Other Menus

LCDSprite* m_UISpriteCursor;

LCDSprite* m_UISpriteSelected;
LCDSprite* m_UISpriteCannotAfford;
LCDSprite* m_UISpriteInfo;
LCDBitmap* m_UIBitmapInfo;

LCDSprite* m_UISpriteScrollBarShortOuter;
LCDBitmap* m_UIBitmapScrollBarShortOuter;

LCDSprite* m_UISpriteScrollBarLongOuter;
LCDBitmap* m_UIBitmapScrollBarLongOuter;

LCDSprite* m_UISpriteFull; // Main window backing
LCDBitmap* m_UIBitmapFull;

#define TUTORIAL_WIDTH DEVICE_PIX_X
#define TUTORIAL_HEIGHT (TILE_PIX*6)

#define INGREDIENTS_WIDTH (TILE_PIX*10)
#define INGREDIENTS_HEIGHT (TILE_PIX*6)
LCDSprite* m_UISpriteIngredients; // Main window backing
LCDBitmap* m_UIBitmapIngredients[kNFactorySubTypes];

LCDSprite* m_UISpriteIngredients; // Main window backing
LCDBitmap* m_UIBitmapIngredients[kNFactorySubTypes];

LCDSprite* m_UISpriteTutorialMain; 
LCDBitmap* m_UIBitmapTutorialMain;

LCDSprite* m_UISpriteTutorialHint; 
LCDBitmap* m_UIBitmapTutorialHint;

LCDSprite* m_UISpriteHeaders[kNUICats]; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 
LCDBitmap* m_UIBitmapHeaders[kNUICats]; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 

LCDSprite* m_UISpriteItems[kNUICats][MAX_PER_CAT][4]; // Final 4 is for rotation states
LCDBitmap* m_UIBitmapItems[kNUICats][MAX_PER_CAT][4];

LCDSprite* m_UISpriteStickySelected[4];

uint16_t m_selCol[kNGameModes] = {0}, m_selRow[kNGameModes] = {1}, m_selRotation = 0;
uint16_t m_selRowOffset[kNGameModes] = {0}, m_cursorRowAbs[kNGameModes] = {1};

LCDSprite* m_contentSprite[MAX_ROWS][ROW_WDTH] = {NULL};
LCDSprite* m_contentStickySelected[MAX_ROWS][ROW_WDTH] = {NULL};
enum kUICat m_contentCat[MAX_ROWS][ROW_WDTH];
uint16_t m_contentID[MAX_ROWS][ROW_WDTH];

bool m_rowIsTitle[MAX_ROWS] = {false};

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

LCDSprite* getSaveSprite() { return m_UISpriteSave; } 

LCDSprite* getLoadSprite() { return m_UISpriteLoad; }

LCDSprite* getGenSprite() { return m_UISpriteGen; }

uint16_t getCursorRotation() {
  return m_selRotation;
}

LCDBitmap* getMainmenuUIBitmap(uint32_t _i) {
  return m_UIBitmapMainMenuItem[_i];
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

//LCDSprite* getUIHeader(enum kUICat _c) {
//  return m_UISpriteHeaders[_c];/
//}

//LCDSprite* getUIItem(enum kUICat _c, uint16_t _i, uint16_t _r) {
//  return m_UISpriteItems[_c][_i][_r];
//}

void setCursorRotation(int8_t _value) {
  m_selRotation = _value;
  UIDirtyRight();
  updateBlueprint();
}

void rotateCursor(bool _increment) {
  if (_increment) {
    m_selRotation = (m_selRotation == kDirN-1 ? 0 : m_selRotation + 1);
  } else {
    m_selRotation = (m_selRotation == 0 ? kDirN-1 : m_selRotation - 1);
  }
  if (m_mode >= kMenuPlayer) UIDirtyMain();
  UIDirtyRight();
  updateBlueprint();
}

void modTitleCursor(bool _increment) {
  if (_increment) {
    m_UITitleSelected = (m_UITitleSelected == N_SAVES-1 ? 0 : m_UITitleSelected + 1);
  } else {
    m_UITitleSelected = (m_UITitleSelected == 0 ? N_SAVES-1 : m_UITitleSelected - 1);
  }  
}

uint16_t getTitleCursorSelected() {
  return m_UITitleSelected;
}

void updateUITitles(int _fc) {
  pd->sprite->setVisible(m_UISpriteTitleSelected, _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  pd->sprite->moveTo(m_UISpriteTitleSelected, 
    m_UITitleSelected*TILE_PIX*7 + (7*TILE_PIX)/2 + (m_UITitleSelected+1)*TILE_PIX, 
    DEVICE_PIX_Y + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset)*2);
  if (m_UITitleOffset) {
    --m_UITitleOffset;
    pd->sprite->moveTo(m_UISpriteSplash,
      DEVICE_PIX_X/2,
      DEVICE_PIX_Y/2 - (UI_TITLE_OFFSET - m_UITitleOffset) );
    for (int32_t i = 0; i < 3; ++i) {
      pd->sprite->moveTo(m_UISpriteTitleNew[i], 
        i*TILE_PIX*7 + (7*TILE_PIX)/2 + (i+1)*TILE_PIX, 
        DEVICE_PIX_Y + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset)*2);
      pd->sprite->moveTo(m_UISpriteTitleCont[i], 
        i*TILE_PIX*7 + (7*TILE_PIX)/2 + (i+1)*TILE_PIX, 
        DEVICE_PIX_Y + TILE_PIX/2 - (UI_TITLE_OFFSET - m_UITitleOffset)*2);
    }
  }
}

void updateUI(int _fc) {
  
  const bool flash = _fc % (TICK_FREQUENCY/4) == 0;
  if ((m_mode >= kMenuBuy) && flash) {
    // Flashing cursor
    UIDirtyMain();
  } else if ((m_mode == kPlaceMode || m_mode == kBuildMode || m_mode == kPlantMode) && flash) {
    // Flashing blueprint 
    updateBlueprint();
    pd->sprite->setVisible(getPlayer()->m_blueprint[getZoom()], _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  }
  if (_fc % FAR_TICK_FREQUENCY == 0) {
    // Update bottom ticker
    UIDirtyBottom();

    // Tutorial
    if (getTutorialStage() == kTutBuildConveyor && m_mode == kWanderMode && getTutorialProgress()) {
      nextTutorialStage();
    }
    // Tutorial
    if (getTutorialStage() == kTutBuildVitamin && m_mode == kWanderMode && getTutorialProgress()) {
      nextTutorialStage();
    }
  }

  if (m_mode == kWanderMode) {
    if (!m_UITopVisible) {
      if (_fc % (TICK_FREQUENCY/2) == 0) {
        const bool ic = isCamouflaged();
        if      (distanceFromBuy()  < ACTIVATE_DISTANCE) drawUITop("The Shop");
        else if (distanceFromSell() < ACTIVATE_DISTANCE) drawUITop("Sales");
        else if (!ic && distanceFromWarp() < ACTIVATE_DISTANCE) drawUITop("Plots");
        else if (!ic && distanceFromOut() < ACTIVATE_DISTANCE) drawUITop("Exports");
        else if (!ic && distanceFromIn() < ACTIVATE_DISTANCE) drawUITop("Imports");
      }
    } else {
      if (distanceFromBuy() >= ACTIVATE_DISTANCE && 
          distanceFromSell() >= ACTIVATE_DISTANCE &&
          distanceFromWarp() >= ACTIVATE_DISTANCE &&
          distanceFromOut() >= ACTIVATE_DISTANCE &&
          distanceFromIn() >= ACTIVATE_DISTANCE) drawUITop(NULL);
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

void updateBlueprint() {
  uint8_t zoom = getZoom();
  struct Player_t* player = getPlayer();
  enum kGameMode gm = getGameMode();
  LCDSprite* bp = player->m_blueprint[zoom];
  LCDSprite* bpRadius = player->m_blueprintRadius[zoom];

  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  uint16_t selectedRot = getCursorRotation();
  struct Location_t* pl = getPlayerLocation();

  pd->sprite->setDrawMode(bp, kDrawModeCopy);

  if (gm == kPickMode || gm == kDestroyMode) {
    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
    switch (getRadius()) {
      case 1: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap1x1[zoom], kBitmapUnflipped); break;
      case 3: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped); break;
      case 5: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap5x5[zoom], kBitmapUnflipped); break;
    }
  } else if (gm == kPlaceMode) { // Of conveyors, cargo or utility
    if (selectedCat == kUICatUtility && selectedID == kConveyorGrease) {
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
                           pd->sprite->setImage(bp, getSprite16_byidx( UDesc[selectedID].UIIcon + selectedRot, zoom), kBitmapUnflipped); break;
      case kUICatCargo:; canPlace = canBePlacedCargo(pl);
                         pd->sprite->setImage(bp, getSprite16_byidx( CargoDesc[selectedID].UIIcon, zoom), kBitmapUnflipped); break;
      default: break;
    }
    if (!canPlace) pd->sprite->setImage(bp, getSprite16(1, 16, zoom), kBitmapUnflipped);
  } else if (gm == kPlantMode) { // Of crops
    bool canPlace;
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    if (canBePlacedPlant(pl)) {
      const struct Tile_t* t = getTile_fromLocation( pl );
      const int8_t gb = getGroundBonus( PDesc[selectedID].soil, getGroundType( t->m_tile ) );
      const int8_t wb = getWaterBonus( PDesc[selectedID].wetness, getWetness( t->m_wetness ) );
      pd->sprite->setImage(bp, getSprite16_byidx( getPlantSmilieSprite(gb+wb), zoom), kBitmapUnflipped);
    } else {
      pd->sprite->setImage(bp, getSprite16(1, 16, zoom), kBitmapUnflipped);
    }
  } else if (gm == kBuildMode) { // Of factories and harvesters
    if (selectedCat == kUICatExtractor) {
      switch (selectedID) {
        case kCropHarvesterLarge: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap9x9[zoom], kBitmapUnflipped);
                                  pd->sprite->setDrawMode(bp, kDrawModeInverted); break;
        case kCropHarvesterSmall: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap7x7[zoom], kBitmapUnflipped); break;
        default: pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
      }
    } else {
      pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
    }
    bool canPlace = true;
    switch (selectedCat) {
      case kUICatExtractor:; canPlace = canBePlacedExtractor(pl, (union kSubType) {.extractor = selectedID});
                             pd->sprite->setImage(bp, getSprite48_byidx( EDesc[selectedID].sprite + selectedRot, zoom), kBitmapUnflipped); break;
      case kUICatFactory:; canPlace = canBePlacedFactory(pl);
                           pd->sprite->setImage(bp, getSprite48_byidx( FDesc[selectedID].sprite + selectedRot, zoom), kBitmapUnflipped); break;
      default: break;
    }
    if (!canPlace) pd->sprite->setImage(bp, getSprite48(1, 2, zoom), kBitmapUnflipped);
  } else { // Clear blueprint
    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
  }
}

void addUIToSpriteList() {
  struct Player_t* p = getPlayer();

  if (m_mode == kTitles) {
    pd->sprite->addSprite(m_UISpriteSplash);
    pd->sprite->addSprite(m_UISpriteTitleSelected);
    for (int32_t i = 0; i < 3; ++i) {
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
    if (m_mode == kMenuNew) {
      pd->sprite->addSprite(m_UISpriteNewBanner);
      pd->sprite->addSprite(m_UISpriteNewSplash);
      pd->sprite->addSprite(m_UISpriteNewItem);
      pd->sprite->addSprite(m_UISpriteNewText);
    } else if (m_mode == kMenuMain) {
      for (int32_t i = 0; i < MAX_ROWS; ++i) {
        pd->sprite->addSprite(m_UISpriteMainMenuItem[i]);
      }
      pd->sprite->addSprite(m_UISpriteMainMenuCursor);
      pd->sprite->addSprite(m_UISpriteScrollBarLongOuter);
    } else {    
      pd->sprite->addSprite(m_UISpriteCursor);
      pd->sprite->addSprite(m_UISpriteSelected);
      pd->sprite->addSprite(m_UISpriteCannotAfford);
      pd->sprite->addSprite(m_UISpriteInfo);
      pd->sprite->addSprite(m_UISpriteIngredients);
      pd->sprite->addSprite(m_UISpriteScrollBarShortOuter);
      for (int32_t i = 0; i < 4; ++i) {
        pd->sprite->addSprite(m_UISpriteStickySelected[i]);
      }
      for (enum kUICat c = 0; c < kNUICats; ++c) {
        if (m_UISpriteHeaders[c] != NULL) {
          pd->sprite->addSprite(m_UISpriteHeaders[c]);
        }
        for (int32_t i = 0; i < MAX_PER_CAT; ++i) {
          for (int32_t r = 0; r < 4; ++r) {
            if (m_UISpriteItems[c][i][r] != NULL) {
              pd->sprite->addSprite(m_UISpriteItems[c][i][r]);
            }
          }
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
  snprintf(text, 128, "--- Tutorial Stage %i/%i ---", tutN, ofN);
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), text, 128, kASCIIEncoding, 0);
  pd->graphics->drawText(text, 128, kASCIIEncoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  for (int32_t l = 0; l < 5; ++l) {
    const char* txt = toStringTutorial(_stage, l);
    width = pd->graphics->getTextWidth(getRoobert10(), txt, strlen(txt), kUTF8Encoding, 0);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  }
  pd->graphics->popContext();

  pd->graphics->pushContext(m_UIBitmapTutorialHint);
  y = 1;
  setRoobert10();
  for (int32_t l = 5; l < 9; ++l) {
    const char* txt = toStringTutorial(_stage, l);
    width = pd->graphics->getTextWidth(getRoobert10(), txt, strlen(txt), kUTF8Encoding, 0);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2 + 1, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - TUT_Y_SHFT + 1);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2 - 1, TUT_Y_SPACING*(y) - TUT_Y_SHFT);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - TUT_Y_SHFT - 1);
    pd->graphics->setDrawMode(kDrawModeFillBlack);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - TUT_Y_SHFT);
  }
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);
}

bool checkReturnDismissTutorialMsg() {
  if (pd->sprite->isVisible(m_UISpriteTutorialMain)) {
    pd->sprite->setVisible(m_UISpriteTutorialMain, 0);
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
      case 0: return "N";
      case 1: return "E";
      case 2: return "S";
      case 3: return "W";
    }
  }

  switch (_selectedID) {
    case kBelt:; case kTunnelIn:; case kTunnelOut:;
      switch (_rotation) {
        case 0: return "N";
        case 1: return "E";
        case 2: return "S";
        case 3: return "W";
      }
    case kSplitI:; case kFilterI:;
      switch (_rotation) {
        case 0: return "W, E";
        case 1: return "N, S";
        case 2: return "W, E";
        case 3: return "N, S";
      }
    case kSplitL:; case kFilterL:;
      switch (_rotation) {
        case 0: return "N, E";
        case 1: return "E, S";
        case 2: return "S, W";
        case 3: return "W, N";
      }
    case kSplitT:;
      switch (_rotation) {
        case 0: return "W, N, E";
        case 1: return "N, E, S";
        case 2: return "E, S, W";
        case 3: return "S, W, N";
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
  static char text[128];
  uint8_t y = 0;
  pd->graphics->pushContext(m_UIBitmapTutorialMain);
  renderTutorialInspectRect(false);

  // Draw grid
  for (int32_t b = 0; b < 4; ++b) {
    pd->graphics->drawRect(TILE_PIX, TUT_Y_SPACING*(3+b) - TUT_Y_SHFT, SCREEN_PIX_X-2*TILE_PIX, TUT_Y_SPACING + 1, kColorBlack);
  }

  struct Location_t* loc = getPlayerLocation();
  struct Tile_t* t = getTile_fromLocation(loc);
  if (loc->m_cargo) {
    snprintf(text, 128, "(%i, %i)  %s %s, %s",
      loc->m_x, loc->m_y, toStringWetness(getWetness(t->m_wetness)), toStringSoil(getGroundType(t->m_tile)), toStringCargoByType(loc->m_cargo->m_type));
  } else {
    snprintf(text, 128, "(%i, %i)  %s %s",
      loc->m_x, loc->m_y, toStringWetness(getWetness(t->m_wetness)), toStringSoil(getGroundType(t->m_tile)));
  }
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert10();
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
  
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

  static char text[128];
  setRoobert10();

  if (getPlayer()->m_enableDebug) {
    snprintf(text, 128, "SL:%u, NT:%u, FT:%u, B:%u, C:%u", 
      pd->sprite->getSpriteCount(), getNearTickCount(), getFarTickCount(), getNBuildings(), getNCargo() );
    pd->graphics->pushContext(m_UIBitmapDev);
    pd->graphics->fillRect(0, 0, DEVICE_PIX_X, TILE_PIX, kColorClear);
    pd->graphics->setDrawMode(kDrawModeFillBlack);
    pd->graphics->drawText(text, 128, kASCIIEncoding, 2*TILE_PIX + 1, 0);
    pd->graphics->drawText(text, 128, kASCIIEncoding, 2*TILE_PIX, +1);
    pd->graphics->drawText(text, 128, kASCIIEncoding, 2*TILE_PIX - 1, 0);
    pd->graphics->drawText(text, 128, kASCIIEncoding, 2*TILE_PIX, -1);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(text, 128, kASCIIEncoding, 2*TILE_PIX, 0);
    pd->graphics->popContext();
  }

  pd->graphics->pushContext(m_UIBitmapBottom);
  pd->graphics->fillRect(0, 0, DEVICE_PIX_X, TILE_PIX, kColorBlack);
  pd->graphics->setDrawMode(kDrawModeFillWhite);

  struct Location_t* loc = getPlayerLocation();
  struct Tile_t* t = getTile_fromLocation(loc);

  if (loc->m_building && loc->m_cargo) {
    snprintf(text, 128, "%s %s, %s, %s", 
      toStringWetness(getWetness(t->m_wetness)), toStringSoil(getGroundType(t->m_tile)), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true), toStringCargo(loc->m_cargo));
  } else if (loc->m_building) {
    snprintf(text, 128, "%s %s, %s", 
      toStringWetness(getWetness(t->m_wetness)), toStringSoil(getGroundType(t->m_tile)), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));
  } else if (loc->m_cargo) {
    snprintf(text, 128, "%s %s, %s", 
      toStringWetness(getWetness(t->m_wetness)), toStringSoil(getGroundType(t->m_tile)), toStringCargo(loc->m_cargo));
  } else {
    snprintf(text, 128, "%s %s", 
      toStringWetness(getWetness(t->m_wetness)), toStringSoil(getGroundType(t->m_tile)));
  }

  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX/2, 0);
  pd->graphics->popContext();
}

void drawUIRight() {
  pd->graphics->pushContext(m_UIBitmapRightRotated);
  pd->graphics->fillRect(0, 0, DEVICE_PIX_Y, TILE_PIX, kColorBlack);
  static char text[32] = "";
  snprintf(text, 32, "%u", (unsigned) getPlayer()->m_money);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert10();
  pd->graphics->drawText(text, 32, kASCIIEncoding, 2*TILE_PIX, 0);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(2, 16, 1), TILE_PIX/2, 0, kBitmapUnflipped); // Coin
  enum kGameMode gm = getGameMode();
  if (gm == kPlaceMode || gm == kPlantMode || gm == kBuildMode) {
    int16_t rotMod = (m_selRotation == 0 ? 3 : m_selRotation - 1); // We are drawing this sideways, so need to rotate it by pi/2
    const enum kUICat selectedCat = getUIContentCategory();
    const uint16_t selectedID =  getUIContentID();
    snprintf(text, 32, "%u", (unsigned) getOwned(selectedCat, selectedID));
    uint16_t spriteID = getUIIcon(selectedCat, selectedID);
    if ((selectedCat >= kUICatConv && selectedCat < kUICatUtility) || (selectedCat == kUICatUtility && selectedID == kBuffferBox)) {
      spriteID += rotMod;
    }
    pd->graphics->drawBitmap(getSprite16_byidx(spriteID, 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(text, 32, kASCIIEncoding, DEVICE_PIX_Y/2 + 2*TILE_PIX, 0);
    pd->graphics->setDrawMode(kDrawModeCopy);
  }
  pd->graphics->popContext();

  pd->graphics->pushContext(m_UIBitmapRight);
  pd->graphics->drawRotatedBitmap(m_UIBitmapRightRotated, 0, 0, 90.0f, 0.0f, 0.0f, 1.0f, 1.0f);  
  pd->graphics->popContext();

  UIDirtyBottom(); // why?
}

void drawUITop(const char* _text) {
  m_UITopVisible = (_text != NULL);
  if (_text == NULL) {
    return;
  }
  pd->graphics->pushContext(m_UIBitmapTop);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, SCREEN_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorBlack);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert24();
  int32_t len = pd->graphics->getTextWidth(getRoobert24(), _text, 16, kASCIIEncoding, 0);
  pd->graphics->drawText(_text, 16, kASCIIEncoding, (SCREEN_PIX_X/2 - len)/2, 0);
  pd->graphics->popContext();
}

int32_t getUnlockCost(enum kUICat _c, int32_t _i) {
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
    case kUICatWarp: return kWarpPrice[_i];
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

uint16_t getUIIcon(enum kUICat _c, uint16_t _i) {
  switch (_c) {
    case kUICatTool: return kToolUIIcon[_i];
    case kUICatPlant: return CargoDesc[ PDesc[_i].out ].UIIcon;
    case kUICatConv: return CDesc[_i].UIIcon;
    case kUICatExtractor: return EDesc[_i].UIIcon;
    case kUICatFactory: return CargoDesc[ FDesc[_i].out ].UIIcon;
    case kUICatUtility: return UDesc[_i].UIIcon;
    case kUICatCargo: return CargoDesc[_i].UIIcon;
    case kUICatWarp: return kWarpUIIcon[_i];
    case kUICatImportN:; case kUICatImportE:; case kUICatImportS:; case kUICatImportW:; return CargoDesc[_i].UIIcon;
    case kNUICats: break;
  }
  return 0;
}

void moveRow(bool _down) {
  const uint8_t maxRowsVisible = (m_mode == kMenuMain ? MAX_ROWS_VISIBLE_MAINMENU : MAX_ROWS_VISIBLE);
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
  const uint8_t rowWidth = m_mode == kMenuMain ? 1 : ROW_WDTH;
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

void setUIContentMainMenu(int32_t _row, bool _isHeader) {
  m_contentSprite[_row][0] = m_UISpriteMainMenuItem[_row];
  m_contentID[_row][0] = _row;
  m_rowIsTitle[_row] = _isHeader;
}

void setUIContentHeader(int32_t _row, enum kUICat _c) {
  m_contentSprite[_row][0] = m_UISpriteHeaders[_c];
  m_rowIsTitle[_row] = true;
}

void setUIContentStickySelected(int32_t _row, int32_t _col, uint8_t _selID) {
  m_contentStickySelected[_row][_col] = m_UISpriteStickySelected[_selID];
}

void setUIContentItem(int32_t _row, int32_t _col, enum kUICat _c, uint16_t _i, uint16_t _r) {
  m_contentSprite[_row][_col] = m_UISpriteItems[_c][_i][_r];
  m_contentCat[_row][_col] = _c;
  m_contentID[_row][_col] = _i;
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

  const enum kGameMode gm = getGameMode();

  // POPULATE
  bool empty = false;
  switch (gm) {
    case kMenuBuy:; populateContentBuy(); break;
    case kMenuMain:; populateContentMainmenu(); break;
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
  if (gm == kMenuMain) {
    for (int32_t i = 0; i < MAX_ROWS; ++i) {
      pd->sprite->setVisible(m_UISpriteMainMenuItem[i], 0);
    }
  } else {
    for (enum kUICat c = 0; c < kNUICats; ++c) {
      pd->sprite->setVisible(m_UISpriteHeaders[c], 0);
      for (int32_t i = 0; i < MAX_PER_CAT; ++i) {
        for (int32_t r = 0; r < 4; ++r) {
          if (m_UISpriteItems[c][i][r] != NULL) {
            pd->sprite->setVisible(m_UISpriteItems[c][i][r], 0);
          }
        }
      }
    }
    for (int32_t i = 0; i < 4; ++i) {
      pd->sprite->setVisible(m_UISpriteStickySelected[i], 0);
    }
  }


  #define MAINMENUSTARTY (TILE_PIX*3)
  #define UISTARTX (TILE_PIX*3)
  #define UISTARTY (TILE_PIX*5)

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
    pd->sprite->moveTo(m_contentSprite[0][0], SCREEN_PIX_X/2 - TILE_PIX, UISTARTY);
    return;
  }
  pd->sprite->setVisible(m_UISpriteInfo, 1);

  // Render
  const uint8_t ROWS_TO_RENDER = (m_mode == kMenuMain ? MAX_ROWS_VISIBLE_MAINMENU : MAX_ROWS_VISIBLE);

  //pd->system->logToConsole("SelRow:%i  SelCol:%i SelRowOffset:%i CursorRowAbs:%i", m_selRow[m_mode], m_selCol[m_mode], m_selRowOffset[m_mode], m_cursorRowAbs[m_mode]);

  for (int32_t r = 0; r < ROWS_TO_RENDER; ++r) {
    int32_t rID = r + m_selRowOffset[m_mode];
    if (rID >= MAX_ROWS) break;
    if (m_contentSprite[rID][0] == NULL) break; // Not populated
    if (m_mode == kMenuMain) {
      pd->sprite->setVisible(m_contentSprite[rID][0], 1);
      pd->sprite->moveTo(m_contentSprite[rID][0], SCREEN_PIX_X/2 - TILE_PIX, MAINMENUSTARTY + r*TILE_PIX);
    } else if (m_rowIsTitle[rID]) {
      pd->sprite->setVisible(m_contentSprite[rID][0], 1);
      pd->sprite->moveTo(m_contentSprite[rID][0], SCREEN_PIX_X/2 - TILE_PIX, UISTARTY + r*2*TILE_PIX);
    } else {
      for (int32_t c = 0; c < ROW_WDTH; ++c) {
        if (m_contentSprite[rID][c] == NULL) break; // Not populated
        pd->sprite->setVisible(m_contentSprite[rID][c], 1);
        pd->sprite->moveTo(m_contentSprite[rID][c], UISTARTX + c*2*TILE_PIX, UISTARTY + r*2*TILE_PIX);
        if (m_contentStickySelected[rID][c] != NULL) {

  pd->system->logToConsole("rid %i c %i sprite %i", rID, c, (int)m_contentStickySelected[rID][c]);

          // TODO this started to crash?
          //pd->sprite->setVisible(m_contentStickySelected[rID][c], 1);
          //pd->sprite->moveTo(m_contentStickySelected[rID][c], UISTARTX + c*2*TILE_PIX, UISTARTY + r*2*TILE_PIX);
        }
      }
    }
  }

  // CURSOR
  const bool visible = (getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  if (m_mode == kMenuMain) {
    pd->sprite->setVisible(m_UISpriteMainMenuCursor, visible);
    pd->sprite->moveTo(m_UISpriteMainMenuCursor, UISTARTX + TILE_PIX*8, MAINMENUSTARTY + m_cursorRowAbs[m_mode]*TILE_PIX);
  } else {
    pd->sprite->setVisible(m_UISpriteCursor, visible);
    pd->sprite->moveTo(m_UISpriteCursor, UISTARTX + m_selCol[m_mode]*2*TILE_PIX, UISTARTY + m_cursorRowAbs[m_mode]*2*TILE_PIX);
  }

  // SELECTED
  if (m_mode != kMenuMain) {
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
  const enum kUICat newCat = getNewCategory();
  const uint32_t newID = getNewID();
  pd->sprite->setImage(m_UISpriteNewBanner, m_UIBitmapNewBanner[newCat], kBitmapUnflipped);

  pd->graphics->clearBitmap(m_UIBitmapNewItem, kColorClear);
  pd->graphics->pushContext(m_UIBitmapNewItem);
  pd->graphics->drawScaledBitmap(m_UIBitmapItems[newCat][newID][0], 0, 0, 2, 2);
  pd->graphics->popContext();

  pd->graphics->clearBitmap(m_UIBitmapNewText, kColorClear);
  pd->graphics->pushContext(m_UIBitmapNewText);
  pd->graphics->setDrawMode(kDrawModeFillBlack);

  const char* t0 = toStringBuilding(getCatBuildingSubType(newCat), (union kSubType) {.raw = newID}, false);
  int16_t len0 = strlen(t0);
  int32_t width0 = pd->graphics->getTextWidth(getRoobert10(), t0, len0, kASCIIEncoding, 0);

  const char* t1 = getNewText();
  int16_t len1 = strlen(t1);
  int32_t width1 = pd->graphics->getTextWidth(getRoobert10(), t1, len1, kASCIIEncoding, 0);

  pd->graphics->setDrawMode(kDrawModeFillWhite);
  //
  pd->graphics->drawText(t0, len0, kASCIIEncoding, TILE_PIX*10 - width0/2 + 1, 0);
  pd->graphics->drawText(t1, len1, kASCIIEncoding, TILE_PIX*10 - width1/2 + 1, TILE_PIX);
  //
  pd->graphics->drawText(t0, len0, kASCIIEncoding, TILE_PIX*10 - width0/2 - 1, 0);
  pd->graphics->drawText(t1, len1, kASCIIEncoding, TILE_PIX*10 - width1/2 - 1, TILE_PIX);
  //
  pd->graphics->drawText(t0, len0, kASCIIEncoding, TILE_PIX*10 - width0/2, 1);
  pd->graphics->drawText(t1, len1, kASCIIEncoding, TILE_PIX*10 - width1/2, TILE_PIX + 1);
  //
  pd->graphics->drawText(t0, len0, kASCIIEncoding, TILE_PIX*10 - width0/2, -1);
  pd->graphics->drawText(t1, len1, kASCIIEncoding, TILE_PIX*10 - width1/2, TILE_PIX- 1);
  //
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  //
  pd->graphics->drawText(t0, len0, kASCIIEncoding, TILE_PIX*10 - width0/2, 0);
  pd->graphics->drawText(t1, len1, kASCIIEncoding, TILE_PIX*10 - width1/2, TILE_PIX);
  //
  pd->graphics->setDrawMode(kDrawModeCopy);

  pd->graphics->popContext();
}

void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;
  if (m_mode == kTitles) return;

  if (_mode == kPlaceMode) {
    drawUITop("Place Mode");
  } else if (_mode == kPlantMode) {
    drawUITop("Plant Mode");
  } else if (_mode == kBuildMode) {
    drawUITop("Build Mode");
  } else if (_mode == kPickMode) {
    drawUITop("Pick Mode");
  } else if (_mode == kMenuPlayer) {
    drawUITop("Inventory");
  } else if (_mode == kInspectMode) {
    drawUITop("Inspect");
  } else if (_mode >= kMenuBuy) {
    /*noop*/ 
  } else drawUITop(NULL);

  if (m_mode >= kMenuBuy) UIDirtyMain();
  UIDirtyRight(); // Just safer to always do this here
  updateBlueprint();
  updateRenderList();
}

void resetUI() {
  for (int32_t i = 0; i < kNGameModes; ++i) {
    m_selCol[i] = 0;
    m_selRow[i] = 1;
    m_selRowOffset[i] = 0;
    m_cursorRowAbs[i] = 1;
  }
  m_selRotation = 0;
  updateBlueprint();
  m_UITitleOffset = UI_TITLE_OFFSET;
  m_UITopOffset = 0;
  m_UITitleSelected = 0;
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
    case kUICatPlant: return "New Crop!";
    case kUICatConv: return "New Conveyor!";
    case kUICatExtractor: return "New Harvester!";
    case kUICatFactory: return "New Factory!";
    case kUICatUtility: return "New Utility!";
    default: break;
  }
  return "?";
}

const char* getWorldLetter(int8_t _i) {
  switch(_i) {
    case 0: return "a";
    case 1: return "b";
  }
  return "c";
}

void initiUI() {
  m_UISpriteTop = pd->sprite->newSprite();
  m_UISpriteSave = pd->sprite->newSprite();
  m_UISpriteLoad = pd->sprite->newSprite();
  m_UISpriteGen = pd->sprite->newSprite();
  m_UISpriteBottom = pd->sprite->newSprite();
  m_UISpriteRight = pd->sprite->newSprite();

  m_UIBitmapTop = pd->graphics->newBitmap(SCREEN_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapSave = pd->graphics->newBitmap(DEVICE_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapGen = pd->graphics->newBitmap(DEVICE_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapLoad = pd->graphics->newBitmap(DEVICE_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapBottom = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX, kColorBlack);
  m_UIBitmapRight = pd->graphics->newBitmap(TILE_PIX, DEVICE_PIX_Y, kColorBlack);
  m_UIBitmapRightRotated = pd->graphics->newBitmap(DEVICE_PIX_Y, TILE_PIX, kColorBlack);

  PDRect boundTop = {.x = 0, .y = 0, .width = SCREEN_PIX_X/2, .height = TILE_PIX*2};
  pd->sprite->setBounds(m_UISpriteTop, boundTop);
  pd->sprite->setImage(m_UISpriteTop, m_UIBitmapTop, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2 - 32, TILE_PIX);
  pd->sprite->setZIndex(m_UISpriteTop, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTop, 1);
  pd->sprite->setVisible(m_UISpriteTop, 1);

  pd->sprite->setBounds(m_UISpriteSave, boundTop);
  pd->sprite->setImage(m_UISpriteSave, m_UIBitmapSave, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteSave, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteSave, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSave, 1);
  pd->sprite->setVisible(m_UISpriteSave, 1);

  pd->graphics->pushContext(m_UIBitmapSave);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, DEVICE_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert24();
  int32_t tlen = pd->graphics->getTextWidth(getRoobert24(), "SAVING", 16, kASCIIEncoding, 0);
  pd->graphics->drawText("SAVING", 16, kASCIIEncoding, (DEVICE_PIX_X/2 - tlen)/2, 0);
  pd->graphics->popContext();

  pd->sprite->setBounds(m_UISpriteGen, boundTop);
  pd->sprite->setImage(m_UISpriteGen, m_UIBitmapGen, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteGen, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteGen, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteGen, 1);
  pd->sprite->setVisible(m_UISpriteGen, 1);

  pd->graphics->pushContext(m_UIBitmapGen);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, DEVICE_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert24();
  tlen = pd->graphics->getTextWidth(getRoobert24(), "GENERATING", 16, kASCIIEncoding, 0);
  pd->graphics->drawText("GENERATING", 16, kASCIIEncoding, (DEVICE_PIX_X/2 - tlen)/2, 0);
  pd->graphics->popContext();

  pd->sprite->setBounds(m_UISpriteLoad, boundTop);
  pd->sprite->setImage(m_UISpriteLoad, m_UIBitmapLoad, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteLoad, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteLoad, Z_INDEX_UI_TT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteLoad, 1);
  pd->sprite->setVisible(m_UISpriteLoad, 1);

  pd->graphics->pushContext(m_UIBitmapLoad);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, DEVICE_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert24();
  tlen = pd->graphics->getTextWidth(getRoobert24(), "LOADING", 16, kASCIIEncoding, 0);
  pd->graphics->drawText("LOADING", 16, kASCIIEncoding, (DEVICE_PIX_X/2 - tlen)/2, 0);
  pd->graphics->popContext();
  pd->graphics->setDrawMode(kDrawModeCopy);

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
  PDRect mainMenuBound = {.x = 0, .y = 0, .width = TILE_PIX*18, .height = TILE_PIX};
  PDRect scrollShortBound = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*8};
  PDRect scrollLongBound = {.x = 0, .y = 0, .width = TILE_PIX*2, .height = TILE_PIX*10};

  // Titles

  m_UISpriteSplash = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteSplash, deviceBound);
  pd->sprite->setImage(m_UISpriteSplash, getSpriteSplash(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteSplash, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSplash, 1);  
  pd->sprite->moveTo(m_UISpriteSplash, DEVICE_PIX_X/2, DEVICE_PIX_Y/2);

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

    pd->graphics->pushContext(m_UIBitmapTitleNew[i]);
    roundedRect(0, TILE_PIX*7, TILE_PIX*1, TILE_PIX/2, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    char text[32];
    snprintf(text, 32, "%s: New Game", getWorldLetter(i));
    int16_t len = strlen(text);
    int32_t width = pd->graphics->getTextWidth(getRoobert10(), text, len, kASCIIEncoding, 0);
    pd->graphics->drawText(text, len, kASCIIEncoding, (7*TILE_PIX)/2 - width/2, 0);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->popContext();

    pd->graphics->pushContext(m_UIBitmapTitleCont[i]);
    roundedRect(0, TILE_PIX*7, TILE_PIX*1, TILE_PIX/2, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    snprintf(text, 32, "%s: Continue", getWorldLetter(i));
    len = strlen(text);
    width = pd->graphics->getTextWidth(getRoobert10(), text, len, kASCIIEncoding, 0);
    pd->graphics->drawText(text, len, kASCIIEncoding, (7*TILE_PIX)/2 - width/2, 0);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->popContext();
  }

  // Main Menu

  for (int32_t i = 0; i < MAX_ROWS; ++i) {
    m_UIBitmapMainMenuItem[i] = pd->graphics->newBitmap(TILE_PIX*18, TILE_PIX*1, kColorClear);
    m_UISpriteMainMenuItem[i] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteMainMenuItem[i], mainMenuBound);
    pd->sprite->setImage(m_UISpriteMainMenuItem[i], m_UIBitmapMainMenuItem[i], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteMainMenuItem[i], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteMainMenuItem[i], 1);
    redrawMainmenuLine(m_UIBitmapMainMenuItem[i], i);
  }

  m_UISpriteMainMenuCursor = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteMainMenuCursor, mainMenuBound);
  pd->sprite->setImage(m_UISpriteMainMenuCursor, getMainmenuSelectedBitmap(), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteMainMenuCursor, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteMainMenuCursor, 1);

  // New stuff

  setRoobert24();
  for (int32_t i = 0; i < kNUICats; ++i) {
    if (i == kUICatTool || i > kUICatUtility) continue;
    m_UIBitmapNewBanner[i] = pd->graphics->newBitmap(TILE_PIX*20, TILE_PIX*2, kColorClear);
    pd->graphics->pushContext(m_UIBitmapNewBanner[i]);
    roundedRect(1, TILE_PIX*20, TILE_PIX*2, TILE_PIX, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    const char* t = newBannerText(i);
    int16_t len = strlen(t);
    int32_t width = pd->graphics->getTextWidth(getRoobert24(), t, len, kASCIIEncoding, 0);
    pd->graphics->drawText(t, len, kASCIIEncoding, TILE_PIX*10 - width/2, 0);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->popContext();
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



  m_UIBitmapScrollBarShortOuter = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*8, kColorBlack);
  m_UISpriteScrollBarShortOuter = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteScrollBarShortOuter, scrollShortBound);
  pd->sprite->setImage(m_UISpriteScrollBarShortOuter, m_UIBitmapScrollBarShortOuter, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteScrollBarShortOuter, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteScrollBarShortOuter, 1);  
  pd->sprite->moveTo(m_UISpriteScrollBarShortOuter, TILE_PIX*21, TILE_PIX*8);

  m_UIBitmapScrollBarLongOuter = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*10, kColorBlack);
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
  pd->sprite->setZIndex(m_UISpriteInfo, Z_INDEX_UI_M);
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
    pd->graphics->pushContext(m_UIBitmapHeaders[c]);
    roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    switch (c) {
      case kUICatTool: pd->graphics->drawText("Tools", 5, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatPlant: pd->graphics->drawText("Crops", 5, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatConv: pd->graphics->drawText("Conveyors", 9, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatExtractor: pd->graphics->drawText("Harvesters", 10, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatFactory: pd->graphics->drawText("Factories", 9, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatUtility: pd->graphics->drawText("Utility", 7, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatCargo: pd->graphics->drawText("Cargo", 5, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatWarp: pd->graphics->drawText("Plot Locations", 14, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatImportN:; pd->graphics->drawText("Import (North)", 14, kASCIIEncoding, TILE_PIX, 0); break; 
      case kUICatImportE:; pd->graphics->drawText("Import (East)", 13, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatImportS:; pd->graphics->drawText("Import (South)", 14, kASCIIEncoding, TILE_PIX, 0); break;
      case kUICatImportW:; pd->graphics->drawText("Import (West)", 13, kASCIIEncoding, TILE_PIX, 0); break;
      case kNUICats: break;
    }
    pd->graphics->popContext();

    m_UISpriteHeaders[c] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteHeaders[c], cBound);
    pd->sprite->setImage(m_UISpriteHeaders[c], m_UIBitmapHeaders[c], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteHeaders[c], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteHeaders[c], 1);

    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      uint16_t spriteID = getUIIcon(c, i);
      for (int32_t r = 0; r < 4; ++r) {
        m_UIBitmapItems[c][i][r] = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorClear);
        pd->graphics->pushContext(m_UIBitmapItems[c][i][r]);
        if (c == kUICatPlant) { // Crops are seeds in a packet
          pd->graphics->drawBitmap(getSprite16(3, 16, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID, 1), TILE_PIX/2, TILE_PIX/2, kBitmapUnflipped);
        } else if (c == kUICatFactory) {
          pd->graphics->drawBitmap(getSprite16(12 + r, 10, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID, 1), TILE_PIX/2, TILE_PIX/2, kBitmapUnflipped);
        } else {
          if (c != kUICatConv && c != kUICatExtractor && c != kUICatWarp && c != kUICatUtility) {
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
  pd->sprite->setZIndex(m_UISpriteIngredients, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteIngredients, 1);

  #define ING_X_START 6
  #define ING_Y_START 20
  #define ING_ROW_SIZE 12
  #define ING_ROW_MAX (INGREDIENTS_WIDTH - (ING_X_START*2))
  uint8_t textSize = pd->graphics->getFontHeight(getRoobert10());
  for (enum kFactorySubType fac = 0; fac < kNFactorySubTypes; ++fac) {
    m_UIBitmapIngredients[fac] = pd->graphics->newBitmap(INGREDIENTS_WIDTH, INGREDIENTS_HEIGHT, kColorWhite);
    pd->graphics->pushContext(m_UIBitmapIngredients[fac]);
    pd->graphics->drawRect(0, 0, INGREDIENTS_WIDTH, INGREDIENTS_HEIGHT, kColorBlack);
    pd->graphics->drawRect(3, 3, INGREDIENTS_WIDTH-6, INGREDIENTS_HEIGHT-6, kColorBlack);
    pd->graphics->drawRect(4, 4, INGREDIENTS_WIDTH-8, INGREDIENTS_HEIGHT-8, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillBlack);
    setRoobert10();
    pd->graphics->drawText("Ingredients:", 12, kASCIIEncoding, 6, 6);
    int16_t cur_x = ING_X_START;
    int16_t cur_y = ING_Y_START;
    for (int32_t w = 0; w < getNIngreindientsText(fac); ++w) {
      bool isFlavourText;
      const char* str = toStringIngredients(fac, w, &isFlavourText);
      isFlavourText ? setCooperHewitt12() : setRoobert10();
      int16_t len = strlen(str);
      int32_t width = pd->graphics->getTextWidth(isFlavourText ? getCooperHewitt12() : getRoobert10(), str, len, kASCIIEncoding, 0);
      if (cur_x + width > ING_ROW_MAX) {
        cur_y += textSize;
        cur_x = ING_X_START;
      }
      pd->graphics->drawText(str, len, kASCIIEncoding, cur_x, cur_y);
      cur_x += width;
    }
    pd->graphics->popContext();
  }

  UIDirtyBottom();
  UIDirtyRight();
}


const char* toStringTutorial(enum kUITutorialStage _stage, uint16_t _n) {
  switch (_stage) {
    case kTutWelcomeBuySeeds:;
      switch (_n) {
        case 0: return "-- The Initial Seed Purchase --";
        case 1: return "Welcome to Factory Farming! There's money to be";
        case 2: return "made, and it won't make itself! So let's get started by";
        case 3: return "vising The Shop with Ⓐ and buying some Carrot";
        case 4: return "Seeds. Use Ⓑ to exit any mode or menu.";

        case 5: return "Move with the D-Pad, ✛. Zoom in & out with 🎣.";
        case 6: return "Go to The Shop and press Ⓐ.";
        case 7: return "Buy 10 Carrot Seeds from The Shop with Ⓐ.";
        case 8: return "Press Ⓑ to exit The Shop.";
      }
    case kTutPlantCarrots:;
      switch (_n) {
        case 0: return "-- The First Crops --";
        case 1: return "Good, carrots will grow well in the Silty Soil near to the";
        case 2: return "shop. Pay attention to the Soil & Water each plant likes.";
        case 3: return "Find a good nearby spot, open your inventory";
        case 4: return "with Ⓐ, select the seeds, and plant 10 Carrots.";
          
        case 5: return "Find a nearby area of Silty Soil, preferably Moist. ";
        case 6: return "Press Ⓐ to enter your inventory, select the Carrot";
        case 7: return "Seeds and press Ⓐ again. Plant 10 Carrot Seeds on";
        case 8: return "empty patches of Silty Soil, then press Ⓑ.";
      }
    case kTutGetCarrots:;
      switch (_n) {
        case 0: return "-- The First Harvest --";
        case 1: return "Great, crops normally take some time to yield produce,";
        case 2: return "but let's speed up these carrot plants. Press Ⓐ and";
        case 3: return "enter 'Pickup Mode' and harvest 10 grown carrots.";
        case 4: return "You can pickup any cargo in the world, or in buildings.";
          
        case 5: return "Go to where you planted your Carrot Seeds. ";
        case 6: return "Press Ⓐ to enter your inventory, select 'Pickup";
        case 7: return "Mode' and press Ⓐ again. Press or hold Ⓐ when";
        case 8: return "moving, pickup 10 grown Carrots.";
      }
    case kTutSellCarrots:;
      switch (_n) {
        case 0: return "-- The First Sale --";
        case 1: return "Nice, you know what we can do with Cargo like those";
        case 2: return "fresh Carrots? Sell them for a profit! Visit Sales";
        case 3: return "(next to The Shop) and press Ⓐ to access the";
        case 4: return "sales menu. Sell 10 carrots to continue.";
          
        case 5: return "Go to Sales (next to The Shop). ";
        case 6: return "Press Ⓐ, choose the harvested Carrots";
        case 7: return "Press or hold Ⓐ to sell at least 10 Carrots.";
        case 8: return "Press Ⓑ to close the Sales window.";
      }
    case kTutBuildHarvester:;
      switch (_n) {
        case 0: return "-- The First Automated Harvest --";
        case 1: return "Sweet, but that was a lot of manual labor... Let's";
        case 2: return "use an Automatic Harvester instead! Buy one from";
        case 3: return "The Shop and build it where some Carrot Plants are";
        case 4: return "within its harvest catchment area.";
          
        case 5: return "Go to the The Shop, buy an Automatic Harvester.";
        case 6: return "Press Ⓐ and choose this from your inventory.";
        case 7: return "Place the harvester such that it located on empty";
        case 8: return "soil, and Carrot Plants are within its harvest range.";
      }
    case kTutBuildConveyor:;
      switch (_n) {
        case 0: return "-- The First Cargo Line --";
        case 1: return "Good, We're harvesting Carrots. But we need to get them";
        case 2: return "to Sales. We need Conveyor Belts! Buy around";
        case 3: return "50 from The Shop and lay the path to move & sell";
        case 4: return "the carrots. Rotate belt pieces with 🎣.";
          
        case 5: return "Go to the The Shop, buy around 50 an Conveyor Belts.";
        case 6: return "Press Ⓐ and choose these from your inventory.";
        case 7: return "Make a chain of belts from the Automatic Harvester to";
        case 8: return "Sales. Rotate the Belts with 🎣.";
      }
    case kTutBuildQuarry:
      switch (_n) {
        case 0: return "-- Mining The Earth --";
        case 1: return "OK. But not all exploitable resources are grown.";
        case 2: return "Some are dug up, or pumped out. Let's build a Chalk";
        case 3: return "Quarry next. Buy one from The Shop and build it on";
        case 4: return "Chalky Soil. There should be some to the West.";
          
        case 5: return "Go to the The Shop, buy a Chalk Quarry.";
        case 6: return "Press Ⓐ and choose this from your inventory.";
        case 7: return "Go West from The Shop and look for Chalky Soil.";
        case 8: return "Place the Quarry on at least one Chalky Soil tile.";
      }
    case kTutBuildVitamin:
      switch (_n) {
        case 0: return "-- Manufacturing Value --";
        case 1: return "Nice. We can finally set up a full production line!";
        case 2: return "Buy a Vitamin Factory from The Shop, supply it with";
        case 3: return "both Chalk and Carrots, and transport the Vitamins";
        case 4: return "to Sales for a much bigger profit!";
          
        case 5: return "Go to the The Shop, buy a Vitamin Factory.";
        case 6: return "Place it in a good spot. Use Conveyor Belts to";
        case 7: return "supply it with Carrots and Chalk. Use a more Belts";
        case 8: return "to transport the Vitamins to Sales.";
      }
    case kTutFinishedOne:
      switch (_n) {
        case 0: return "-- Go Forth And Consume --";
        #ifdef DEMO
        case 1: return "Excellent! You now know all the basics of exploiting";
        case 2: return "the world for profit. The demo ends here with the";
        case 3: return "Vitamin Factory. If you buy the whole game you can";
        case 4: return "copy your save file over to continue from here!";
        #else
        case 1: return "Excellent! You now know all the basics of exploiting";
        case 2: return "the world for profit. As your bank account swells,";
        case 3: return "more Crops, Factories, and other items will unlock.";
        case 4: return "Use them well to maximize profit.";
        #endif
          
        case 5: return " ";
        case 6: return " ";
        case 7: return " ";
        case 8: return " ";
      }
      ///
    case kTutNewPlots:
      switch (_n) {
        case 0: return "-- But I Don't Have The Soil For That! --";
        case 1: return "Problem, the Cactus Plant you just unlocked wants";
        case 2: return "Sandy Soil, but there isn't any here. Expansion time!";
        case 3: return "Visit the Plots Manager (next to Sales) and buy a new";
        case 4: return "plot of land to develop.";
          
        case 5: return "The Plots Manager, Exports Manager and Imports Manager";
        case 6: return "are all now unlocked.";
        case 7: return "Visit the Plots Manager to the right of Sales.";
        case 8: return "Buy a new plot of land to expand the factory.";
      }
    case kTutExports:
      switch (_n) {
        case 0: return "-- A New Horizon --";
        case 1: return "Welcome to you new plot of land! More room for your";
        case 2: return "manufacturing empire to grow. Instead of selling Cargo,";
        case 3: return "you can also move it between Plots. Use Conveyor Belts to";
        case 4: return "feed Cargo in to the Exports Manager in any of your Plots.";
          
        case 5: return "Export some Cargo by moving it via Conveyors into the";
        case 6: return "Exports Manager (to the right of the Plot Manager) on any";
        case 7: return "Plot. The export rate of each Cargo type is averaged over";
        case 8: return "the previous two minutes.";
      }
    case kTutImports:
      switch (_n) {
        case 0: return "-- Accessing The Goods --";
        case 1: return "Now that you're exporting Cargo, let's look at imports too.";
        case 2: return "Go to a different Plot and visit the Imports Manager,";
        case 3: return "you can import up to four Cargo per Plot. Try setting";
        case 4: return "up an Import now.";
          
        case 5: return "Import some Cargo in a different Plot via the Imports";
        case 6: return "Manager (to the right of the Exports Manager). N, S, E, W";
        case 7: return "imports can be individually chosen. You cannot export a";
        case 8: return "Cargo type which is being imported to the same Plot.";
      }
    case kTutFinishedTwo:
      switch (_n) {
        case 0: return "-- Imports & Exports & Plots --";
        case 1: return "Wonderful! Continue to buy more Plots in order to";
        case 2: return "access new soil types and to expand the factory.";
        case 3: return "Use the Export and Import Manager to move Cargo";
        case 4: return "between plots, and continue to maximize efficiency!";
          
        case 5: return " ";
        case 6: return " ";
        case 7: return " ";
        case 8: return " ";
      }
    default: return " ";
  }
  return " ";
}
