#include "ui.h"
#include "ui/mainmenu.h"
#include "ui/sell.h"
#include "ui/shop.h"
#include "sprite.h"
#include "player.h"
#include "input.h"
#include "cargo.h"
#include "building.h"
#include "location.h"
#include "generate.h"
#include "cargo.h"
#include "render.h"
#include "buildings/special.h"
#include "buildings/factory.h"

enum kGameMode m_mode;

LCDSprite* m_UISpriteBottom;
LCDBitmap* m_UIBitmapBottom;

LCDSprite* m_UISpriteRight;
LCDBitmap* m_UIBitmapRight;
LCDBitmap* m_UIBitmapRightRotated;

LCDSprite* m_UISpriteTop;
LCDBitmap* m_UIBitmapTop;
int16_t m_UITopOffset = 0;
bool m_UITopVisible = false;

LCDBitmap* m_UIBitmapDev;
LCDSprite* m_UISpriteDev;

bool m_UIDirtyBottom = false;

bool m_UIDirtyRight = false;

bool m_UIDirtyMain = false;

// Buy / select screen

LCDSprite* m_UISpriteCursor;

LCDSprite* m_UISpriteSelected;
LCDSprite* m_UISpriteCannotAfford;
LCDSprite* m_UISpriteInfo;
LCDBitmap* m_UIBitmapInfo;

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

uint16_t m_selCol = 0, m_selRow = 1, m_selRotation = 0;
uint16_t m_selRowOffset = 0, m_cursorRowAbs = 1;

LCDSprite* m_contentSprite[MAX_ROWS][ROW_WDTH] = {NULL};
enum kUICat m_contentCat[MAX_ROWS][ROW_WDTH];
uint16_t m_contentID[MAX_ROWS][ROW_WDTH];

bool m_rowIsTitle[MAX_ROWS] = {false};

// Checks that the cursor selection is OK
void checkSel(void);


//

void drawUIBottom(void);

void drawUIRight(void);

void drawUIMain(void);

/// ///

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

uint16_t getCursorRotation() {
  return m_selRotation;
}

enum kUICat getUIContentCategory() {
  return m_contentCat[m_selRow][m_selCol];
}

uint16_t getUIContentID() {
  return m_contentID[m_selRow][m_selCol];
}

//LCDSprite* getUIHeader(enum kUICat _c) {
//  return m_UISpriteHeaders[_c];/
//}

//LCDSprite* getUIItem(enum kUICat _c, uint16_t _i, uint16_t _r) {
//  return m_UISpriteItems[_c][_i][_r];
//}

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

void updateUI(int _fc) {
  
  if ((m_mode >= kMenuBuy) && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing cursor
    UIDirtyMain();
  } else if ((m_mode == kPlaceMode || m_mode == kBuildMode || m_mode == kPlantMode) && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing blueprint 
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
      if      (distanceFromBuy()  < ACTIVATE_DISTANCE) drawUITop("The Shop");
      else if (distanceFromSell() < ACTIVATE_DISTANCE) drawUITop("Sales");
    } else {
      if (distanceFromBuy() >= ACTIVATE_DISTANCE && distanceFromSell() >= ACTIVATE_DISTANCE) drawUITop(NULL);
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
  if (m_UIDirtyMain) {
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

  pd->sprite->setDrawMode(bp, kDrawModeCopy);

  if (gm == kPickMode || gm == kDestroyMode) {
    setPlayerLookingAtOffset(0);
    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
    pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped);
  } else if (gm == kPlaceMode) { // Of conveyors, cargo or utility
    setPlayerLookingAtOffset(0);
    if (selectedCat == kUICatUtility && selectedID == kConveyorGrease) {
      pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped);
    } else {
      pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    }
    switch (selectedCat) {
      case kUICatConv:;    pd->sprite->setImage(bp, getSprite16_byidx(kConvUIIcon[selectedID] + m_selRotation, zoom), kBitmapUnflipped); break;
      case kUICatUtility:; pd->sprite->setImage(bp, getSprite16_byidx(kUtilityUIIcon[selectedID] + m_selRotation, zoom), kBitmapUnflipped); break;
      case kUICatCargo:;   pd->sprite->setImage(bp, getSprite16_byidx(kCargoUIIcon[selectedID], zoom), kBitmapUnflipped); break;
      case kUICatTool:; case kUICatPlant:; case kUICatExtractor:; case kUICatFactory:; case kNUICats:; break;
    }
  } else if (gm == kPlantMode) { // Of crops
    setPlayerLookingAtOffset(0);
    pd->sprite->setImage(bp, getSprite16_byidx(kPlantUIIcon[selectedID], zoom), kBitmapUnflipped); 
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
  } else if (gm == kBuildMode) { // Of factories and harvesters
    setPlayerLookingAtOffset(2);
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
    switch (selectedCat) {
      case kUICatExtractor:; pd->sprite->setImage(bp, getSprite48_byidx(kExtractorSprite[selectedID] + m_selRotation, zoom), kBitmapUnflipped); break;
      case kUICatFactory:;   pd->sprite->setImage(bp, getSprite48_byidx(kFactorySprite[selectedID] + m_selRotation,   zoom), kBitmapUnflipped); break;
      case kUICatTool:; case kUICatPlant:; case kUICatConv:; case kUICatUtility:; case kUICatCargo:; case kNUICats:; break;
    }
  } else { // Clear blueprint
    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
  }
}

void addUIToSpriteList() {
  struct Player_t* p = getPlayer();
  pd->sprite->addSprite(m_UISpriteBottom);
  pd->sprite->addSprite(m_UISpriteTop);
  pd->sprite->addSprite(m_UISpriteRight);
  if (p->m_enableDebug) {
    pd->sprite->addSprite(m_UISpriteDev);
  }

  if (p->m_enableTutorial < 254) { // 254: finished. 255: disabled
    pd->sprite->addSprite(m_UISpriteTutorialMain);
    pd->sprite->addSprite(m_UISpriteTutorialHint);
  }

  if (m_mode >= kMenuBuy) {
    pd->sprite->addSprite(m_UISpriteFull);
    pd->sprite->addSprite(m_UISpriteCursor);
    pd->sprite->addSprite(m_UISpriteSelected);
    pd->sprite->addSprite(m_UISpriteCannotAfford);
    pd->sprite->addSprite(m_UISpriteInfo);
    pd->sprite->addSprite(m_UISpriteIngredients);
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

void showTutorialMsg(enum kUITutorialStage _stage) {
  pd->sprite->setVisible(m_UISpriteTutorialMain, 1);
  #define TUT_Y_SPACING 13
  #define Y_SHFT 4
  char text[128];
  uint8_t y = 0;
  pd->graphics->clearBitmap(m_UIBitmapTutorialMain, kColorWhite);
  pd->graphics->clearBitmap(m_UIBitmapTutorialHint, kColorClear);
  pd->graphics->pushContext(m_UIBitmapTutorialMain);
  pd->graphics->drawRect(0, 0, TUTORIAL_WIDTH, TUTORIAL_HEIGHT, kColorBlack);
  pd->graphics->drawRect(3, 3, TUTORIAL_WIDTH-6, TUTORIAL_HEIGHT-6, kColorBlack);
  pd->graphics->drawRect(4, 4, TUTORIAL_WIDTH-8, TUTORIAL_HEIGHT-8, kColorBlack);
  pd->graphics->drawBitmap(getSprite16(9, 10, 2), TILE_PIX/2, TILE_PIX/2 - Y_SHFT, kBitmapUnflipped);
  pd->graphics->drawBitmap(getSprite16(9, 10, 2), TUTORIAL_WIDTH - (TILE_PIX/2) - TILE_PIX*2, TILE_PIX/2 - Y_SHFT, kBitmapUnflipped);
  pd->graphics->drawBitmap(getSprite16(10, 10, 1), TUTORIAL_WIDTH - (TILE_PIX/2) - TILE_PIX, TUTORIAL_HEIGHT - (TILE_PIX/2) - TILE_PIX, kBitmapUnflipped);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  setRoobert10();
  snprintf(text, 128, "--- Tutorial Stage %u/%u ---", (unsigned) _stage+1, (unsigned) kTutBreakOne);
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), text, 128, kASCIIEncoding, 0);
  pd->graphics->drawText(text, 128, kASCIIEncoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(++y) - Y_SHFT);
  for (int32_t l = 0; l < 5; ++l) {
    const char* txt = toStringTutorial(_stage, l);
    width = pd->graphics->getTextWidth(getRoobert10(), txt, strlen(txt), kUTF8Encoding, 0);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(++y) - Y_SHFT);
  }
  pd->graphics->popContext();

  pd->graphics->pushContext(m_UIBitmapTutorialHint);
  y = 1;
  setRoobert10();
  for (int32_t l = 5; l < 9; ++l) {
    const char* txt = toStringTutorial(_stage, l);
    width = pd->graphics->getTextWidth(getRoobert10(), txt, strlen(txt), kUTF8Encoding, 0);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2 + 1, TUT_Y_SPACING*(++y) - Y_SHFT);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - Y_SHFT + 1);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2 - 1, TUT_Y_SPACING*(y) - Y_SHFT);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - Y_SHFT - 1);
    pd->graphics->setDrawMode(kDrawModeFillBlack);
    pd->graphics->drawText(txt, strlen(txt), kUTF8Encoding, (TUTORIAL_WIDTH-width)/2, TUT_Y_SPACING*(y) - Y_SHFT);
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

const char* getRotationAsString(void) {
  if (m_contentCat[m_selRow][m_selCol] != kUICatConv) {
    switch (m_selRotation) {
      case 0: return "N";
      case 1: return "E";
      case 2: return "S";
      case 3: return "W";
    }
  }

  switch (m_contentID[m_selRow][m_selCol]) {
    case kBelt:; case kTunnelIn:;
      switch (m_selRotation) {
        case 0: return "N";
        case 1: return "E";
        case 2: return "S";
        case 3: return "W";
      }
    case kSplitI:; case kFilterI:;
      switch (m_selRotation) {
        case 0: return "W, E";
        case 1: return "N, S";
        case 2: return "W, E";
        case 3: return "N, S";
      }
    case kSplitL:; case kFilterL:;
      switch (m_selRotation) {
        case 0: return "N, E";
        case 1: return "E, S";
        case 2: return "S, W";
        case 3: return "W, N";
      }
    case kSplitT:;
      switch (m_selRotation) {
        case 0: return "W, N, E";
        case 1: return "N, E, S";
        case 2: return "E, S, W";
        case 3: return "S, W, N";
      }
  }
  return "!";
}


void drawUIBottom() {
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
    if (selectedCat != kUICatFactory && selectedCat >= kUICatConv && selectedCat <= kUICatUtility) spriteID += rotMod;
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
    case kUICatPlant: return kPlantUnlock[_i];
    case kUICatConv: return kConvUnlock[_i];
    case kUICatExtractor: return kExtractorUnlock[_i];
    case kUICatFactory: return kFactoryUnlock[_i];
    case kUICatUtility: return kUtilityUnlock[_i];
    case kUICatCargo: return 0;
    case kNUICats: return 0;
  }
  return 0;
}

int32_t getPrice(enum kUICat _c, int32_t _i) {
  switch (_c) {
    case kUICatTool: return 0;
    case kUICatPlant: return kPlantPrice[_i];
    case kUICatConv: return kConvPrice[_i];
    case kUICatExtractor: return kExtractorPrice[_i];
    case kUICatFactory: return kFactoryPrice[_i];
    case kUICatUtility: return kUtilityPrice[_i];
    case kUICatCargo: return kCargoValue[_i];
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
    case kNUICats: return kNoBuilding;
  }
  return kNoBuilding;
}

uint16_t getUIIcon(enum kUICat _c, uint16_t _i) {
  switch (_c) {
    case kUICatTool: return kToolUIIcon[_i];
    case kUICatPlant: return kPlantUIIcon[_i];
    case kUICatConv: return kConvUIIcon[_i];
    case kUICatExtractor: return kExtractorUIIcon[_i];
    case kUICatFactory: return kCargoUIIcon[ kFactoryOut[_i] ];
    case kUICatUtility: return kUtilityUIIcon[_i];
    case kUICatCargo: return kCargoUIIcon[_i];
    case kNUICats: break;
  }
  return 0;
}

void moveRow(bool _down) {
  if (_down) {
    ++m_selRow;
    if (m_cursorRowAbs == MAX_ROWS_VISIBLE-1) ++m_selRowOffset;
    else ++m_cursorRowAbs;
    //
    if (m_rowIsTitle[m_selRow]) {
      ++m_selRow;
      if (m_cursorRowAbs == MAX_ROWS_VISIBLE-1) ++m_selRowOffset;
      else ++m_cursorRowAbs;
    }
  } else { // Up
    --m_selRow;
    if (m_cursorRowAbs == 0) --m_selRowOffset;
    else --m_cursorRowAbs;
    //
    if (m_rowIsTitle[m_selRow]) {
      --m_selRow;
      if (m_cursorRowAbs == 0) --m_selRowOffset;
      else --m_cursorRowAbs;
    }
  }
}

void checkSel() {
  while (m_contentSprite[m_selRow][m_selCol] == NULL || m_rowIsTitle[m_selRow]) {
    if (m_rowIsTitle[m_selRow] || m_selCol == 0) {
      moveRow(/*down=*/false);
      m_selCol = ROW_WDTH-1;
    } else {
      --m_selCol;
    }
  }
  // special, force the top to be a title
  if (m_cursorRowAbs == 0) {
    --m_selRowOffset;
    ++m_cursorRowAbs;
  }
}

void moveCursor(uint32_t _button) {
  if (kButtonUp    == _button) {
    if (m_selRow == 1) {
      // noop
    } else {
      moveRow(/*down=*/false);
    }
  } else if (kButtonDown  == _button) {
    moveRow(/*down=*/true);
  } else if (kButtonLeft  == _button) {
    if (m_selCol == 0 && m_selRow == 1) {
      // noop
    } else if (m_selCol == 0) {
      moveRow(/*down=*/false);
      m_selCol = ROW_WDTH-1;
    } else {
      --m_selCol;
    }
  } else if (kButtonRight == _button) {
    if (m_selCol == ROW_WDTH-1 || m_contentSprite[m_selRow][m_selCol + 1] == NULL) {
      moveRow(/*down=*/true);
      m_selCol = 0;
    } else {
      ++m_selCol;
    }
  }
  //checkSel();
  UIDirtyMain();
}


void setUIContentHeader(int32_t _row, enum kUICat _c) {
  m_contentSprite[_row][0] = m_UISpriteHeaders[_c];
  m_rowIsTitle[_row] = true;

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
  memset(m_rowIsTitle, 0, MAX_ROWS * sizeof(bool));

  // POPULATE
  bool empty = false;
  switch (getGameMode()) {
    case kMenuBuy:; populateContentBuy(); break;
    case kMenuPlayer:; populateContentMainmenu(); break;
    case kMenuSell:; empty = populateContentSell(); break;
    default: break;
  }

  if (!empty) checkSel();

  // DRAW
  // Mark invisible
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

  #define UISTARTX (TILE_PIX*3)
  #define UISTARTY (TILE_PIX*5)

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
  for (int32_t r = 0; r < 4; ++r) {
    int32_t rID = r + m_selRowOffset;
    if (rID >= MAX_ROWS) break;
    if (m_contentSprite[rID][0] == NULL) break; // Not populated
    if (m_rowIsTitle[rID]) {
      pd->sprite->setVisible(m_contentSprite[rID][0], 1);
      pd->sprite->moveTo(m_contentSprite[rID][0], SCREEN_PIX_X/2 - TILE_PIX, UISTARTY + r*2*TILE_PIX);
    } else {
      for (int32_t c = 0; c < ROW_WDTH; ++c) {
        if (m_contentSprite[rID][c] == NULL) break; // Not populated
        pd->sprite->setVisible(m_contentSprite[rID][c], 1);
        pd->sprite->moveTo(m_contentSprite[rID][c], UISTARTX + c*2*TILE_PIX, UISTARTY + r*2*TILE_PIX);
      }
    }
  }

  // CURSOR
  const bool visible = (getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  pd->sprite->setVisible(m_UISpriteCursor, visible);
  pd->sprite->moveTo(m_UISpriteCursor, UISTARTX + m_selCol*2*TILE_PIX, UISTARTY + m_cursorRowAbs*2*TILE_PIX);

  // SELECTED
  LCDSprite* selectedSprite = m_contentSprite[m_selRow][m_selCol];
  pd->sprite->setVisible(m_UISpriteSelected, 1);
  pd->sprite->setImage(m_UISpriteSelected, pd->sprite->getImage(selectedSprite), kBitmapUnflipped);

  // INGREDIENTS
  if (m_contentCat[m_selRow][m_selCol] == kUICatFactory) {
    int32_t x = UISTARTX + (m_selCol < ROW_WDTH/2 + 1 ? TILE_PIX*14 : TILE_PIX*4);
    pd->sprite->setImage(m_UISpriteIngredients, m_UIBitmapIngredients[getUIContentID()], kBitmapUnflipped);
    pd->sprite->moveTo(m_UISpriteIngredients, x, TILE_PIX*8);
    pd->sprite->setVisible(m_UISpriteIngredients, 1);
  } else {
   pd->sprite->setVisible(m_UISpriteIngredients, 0);
  }

  // CUSTOM TOP AREA
  switch (getGameMode()) {
    case kMenuBuy:; populateInfoBuy(visible); break;
    case kMenuPlayer:; populateInfoMainmenu(); break;
    case kMenuSell:; populateInfoSell(); break;
    default: break;
  }

}

void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;

  if (_mode == kPlaceMode) {
    drawUITop("Place Mode");
  } else if (_mode == kPlantMode) {
    drawUITop("Plant Mode");
  } else if (_mode == kBuildMode) {
    drawUITop("Build Mode");
  } else if (_mode == kPickMode) {
    drawUITop("Pick Mode");
  } else if (_mode == kMenuPlayer) {
    drawUITop("Main Menu");
  } else if (_mode == kMenuBuy || _mode == kMenuSell) {
    /*noop*/ 
  } else drawUITop(NULL);

  if (m_mode >= kMenuBuy) UIDirtyMain();
  UIDirtyRight(); // Just safer to always do this here
  updateBlueprint();
  updateRenderList();
}

void resetUI() {
  m_selCol = 0;
  m_selRow = 1;
  m_selRotation = 0;
  m_selRowOffset = 0;
  m_cursorRowAbs = 1;
  setGameMode(kWanderMode);
  updateBlueprint();
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

void initiUI() {
  m_UISpriteTop = pd->sprite->newSprite();
  m_UISpriteBottom = pd->sprite->newSprite();
  m_UISpriteRight = pd->sprite->newSprite();

  m_UIBitmapTop = pd->graphics->newBitmap(SCREEN_PIX_X/2, TILE_PIX*2, kColorClear);
  m_UIBitmapBottom = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX, kColorBlack);
  m_UIBitmapRight = pd->graphics->newBitmap(TILE_PIX, DEVICE_PIX_Y, kColorBlack);
  m_UIBitmapRightRotated = pd->graphics->newBitmap(DEVICE_PIX_Y, TILE_PIX, kColorBlack);

  PDRect boundTop = {.x = 0, .y = 0, .width = SCREEN_PIX_X/2, .height = TILE_PIX*2};
  pd->sprite->setBounds(m_UISpriteTop, boundTop);
  pd->sprite->setImage(m_UISpriteTop, m_UIBitmapTop, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2 - 32, TILE_PIX);
  pd->sprite->setZIndex(m_UISpriteTop, Z_INDEX_MAX);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTop, 1);
  pd->sprite->setVisible(m_UISpriteTop, 1);

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

  // Setup main menu screen

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
  PDRect ingBound = {.x = 0, .y = 0, .width = INGREDIENTS_WIDTH, .height = INGREDIENTS_HEIGHT};
  PDRect tutBound = {.x = 0, .y = 0, .width = TUTORIAL_WIDTH, .height = TUTORIAL_HEIGHT};

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
          if (c != kUICatConv && c != kUICatExtractor) {
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

        if (c == kUICatTool || c == kUICatPlant || c == kUICatCargo) { // Tools, Crops & Cargo don't have any rotation 
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

        case 5: return "Move with the D-Pad. Zoom in & out with the Crank.";
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
        case 2: return "fresh Carrots? Sell them for a profit! Visit the Sales Box";
        case 3: return "(next to The Shop) and press Ⓐ to access the";
        case 4: return "sales menu. Sell 10 carrots to continue.";
          
        case 5: return "Go to the Sales Box (next to The Shop). ";
        case 6: return "Press Ⓐ, choose the harvested Carrots";
        case 7: return "Press or hold Ⓐ to sell at least 10 Carrots.";
        case 8: return "Press Ⓑ to close the Sales Box window.";
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
        case 2: return "to the Sales Box. We need Conveyor Belts! Buy around";
        case 3: return "50 from The Shop and lay the path to move & sell";
        case 4: return "the carrots. Rotate belt pieces with The Crank.";
          
        case 5: return "Go to the The Shop, buy around 50 an Conveyor Belts.";
        case 6: return "Press Ⓐ and choose these from your inventory.";
        case 7: return "Make a chain of belts from the Automatic Harvester to";
        case 8: return "the Sell Box. Rotate the Belts with The Crank.";
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
        case 4: return "to the Sales Box for a much bigger profit!";
          
        case 5: return "Go to the The Shop, buy a Vitamin Factory.";
        case 6: return "Place it in a good spot. Use Conveyor Belts to";
        case 7: return "supply it with Carrots and Chalk. Use a more Belts";
        case 8: return "to transport the Vitamins to the Sell Box.";
      }
    case kTutFinishedOne:
      switch (_n) {
        case 0: return "-- Go Forth And Consume --";
        case 1: return "Excellent! You now know all the basics of exploiting";
        //case 2: return "the world for profit. As your bank account swells,";
        //case 3: return "more Crops, Factories, and other items will unlock.";
        //case 4: return "Use them well to maximise profit.";
        case 2: return "the world for profit. This ends the tutorial,";
        case 3: return "the remaining items available in this Alpha build";
        case 4: return "have all been unlocked and you have been given 100k.";
          
        case 5: return " ";
        case 6: return " ";
        case 7: return " ";
        case 8: return " ";
      }
    default: return " ";
  }
  return " ";
}
