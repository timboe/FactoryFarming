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

#ifdef DEV
LCDBitmap* m_UIBitmapDev;
LCDSprite* m_UISpriteDev;
#endif

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
  } else if (m_mode == kPlaceMode && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing blueprint 
    pd->sprite->setVisible(getPlayer()->m_blueprint[getZoom()], _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  }
  if (_fc % FAR_TICK_FREQUENCY == 0) {
    // Update bottom ticker
    UIDirtyBottom();
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
        case kCropHarvesterLarge: pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap9x9[zoom], kBitmapUnflipped); break;
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
  pd->sprite->addSprite(m_UISpriteBottom);
  pd->sprite->addSprite(m_UISpriteTop);
  pd->sprite->addSprite(m_UISpriteRight);
  #ifdef DEV
  pd->sprite->addSprite(m_UISpriteDev);
  #endif

  if (m_mode >= kMenuBuy) {
    pd->sprite->addSprite(m_UISpriteFull);
    pd->sprite->addSprite(m_UISpriteCursor);
    pd->sprite->addSprite(m_UISpriteSelected);
    pd->sprite->addSprite(m_UISpriteCannotAfford);
    pd->sprite->addSprite(m_UISpriteInfo);
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

  #ifdef DEV
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
  #endif

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
  pd->graphics->drawBitmap(getSprite16(2, 2, 1), TILE_PIX/2, 0, kBitmapUnflipped);
  enum kGameMode gm = getGameMode();
  if (gm == kPlaceMode || gm == kPlantMode || gm == kBuildMode) {
    int16_t rotMod = (m_selRotation == 0 ? 3 : m_selRotation - 1); // We are drawing this sideways, so need to rotate it by pi/2
    const enum kUICat selectedCat = getUIContentCategory();
    const uint16_t selectedID =  getUIContentID();
    snprintf(text, 32, "%u", (unsigned) getOwned(selectedCat, selectedID));
    uint16_t spriteID = getUIIcon(selectedCat, selectedID);
    if (selectedCat >= kUICatConv && selectedCat <= kUICatUtility) spriteID += rotMod;
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
  checkSel();
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
  switch (getGameMode()) {
    case kMenuBuy:; populateContentBuy(); break;
    case kMenuPlayer:; populateContentMainmenu(); break;
    case kMenuSell:; populateContentSell(); break;
    default: break;
  }

  checkSel();

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

  // Render
  #define UISTARTX (TILE_PIX*3)
  #define UISTARTY (TILE_PIX*5)
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
  pd->sprite->setImage(m_UISpriteSelected, pd->sprite->getImage(selectedSprite), kBitmapUnflipped);

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

  #ifdef DEV
  m_UIBitmapDev = pd->graphics->newBitmap(DEVICE_PIX_X, TILE_PIX, kColorClear);
  m_UISpriteDev = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteDev, boundBottom);
  pd->sprite->setImage(m_UISpriteDev, m_UIBitmapDev, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteDev, Z_INDEX_UI_BOTTOM);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteDev, 1);
  #endif

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

  m_UISpriteFull = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteFull, fBound);
  pd->sprite->setImage(m_UISpriteFull, m_UIBitmapFull, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteFull, Z_INDEX_UI_B);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteFull, 1);  
  pd->sprite->moveTo(m_UISpriteFull, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);

  m_UISpriteCursor = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteCursor, iBound);
  pd->sprite->setImage(m_UISpriteCursor, getSprite16(0, 2, 2), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteCursor, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteCursor, 1);

  m_UISpriteSelected = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteSelected, iBound);
  pd->sprite->setZIndex(m_UISpriteSelected, Z_INDEX_UI_M);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteSelected, 1);
  pd->sprite->moveTo(m_UISpriteSelected, TILE_PIX*3, TILE_PIX*3);

  m_UISpriteCannotAfford = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteCannotAfford, iBound);
  pd->sprite->setImage(m_UISpriteCannotAfford, getSprite16(1, 2, 2), kBitmapUnflipped);
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
          pd->graphics->drawBitmap(getSprite16(3, 2, 2), 0, 0, kBitmapUnflipped);
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

  UIDirtyBottom();
  UIDirtyRight();
}