#include "ui.h"
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

#define CATEGORIES 7
LCDSprite* m_UISpriteHeaders[CATEGORIES]; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 
LCDBitmap* m_UIBitmapHeaders[CATEGORIES]; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 

#define ROW_WDTH 9

#define MAX_ROW_PER_CAT 3

#define MAX_PER_CAT (ROW_WDTH * MAX_ROW_PER_CAT)
LCDSprite* m_UISpriteItems[CATEGORIES][MAX_PER_CAT][4]; // Final 4 is for rotation states
LCDBitmap* m_UIBitmapItems[CATEGORIES][MAX_PER_CAT][4];

#define MAX_ROWS (MAX_ROW_PER_CAT*CATEGORIES + CATEGORIES)

#define MAX_ROWS_VISIBLE 4

uint16_t m_selCol = 0, m_selRow = 1, m_selRotation = 0;
uint16_t m_selRowOffset = 0, m_cursorRowAbs = 1;

LCDSprite* m_contentSprite[MAX_ROWS][ROW_WDTH] = {NULL};
enum kBuildingType m_contentCat[MAX_ROWS][ROW_WDTH];
uint16_t m_contentID[MAX_ROWS][ROW_WDTH];

bool m_rowIsTitle[MAX_ROWS] = {false};

// Checks that the cursor selection is OK
void checkSel(void);

#define TOP_TITLE_OFFSET 34

//

void drawUIBottom(void);

void drawUIRight(void);

void drawUIMain(void);

void roundedRect(uint16_t _o, uint16_t _w, uint16_t _h, uint16_t _r, LCDColor _c);

//const char* getRotationAsString(void);

uint16_t getOwned(int32_t _c, int32_t _i);

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

//uint16_t getUISelectedID() {
//  return m_UISelectedID;
//}

uint16_t getCursorRotation() {
  return m_selRotation;
}

void rotateCursor(bool _increment) {
  if (_increment) {
    m_selRotation = (m_selRotation == kDirN-1 ? 0 : m_selRotation + 1);
  } else {
    m_selRotation = (m_selRotation == 0 ? kDirN-1 : m_selRotation - 1);
  }
  if (m_mode == kMenuPlayer) UIDirtyMain();
  UIDirtyRight();
  updateBlueprint();
}

void updateUI(int _fc) {
  
  if ((m_mode == kMenuBuy || m_mode == kMenuPlayer) && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing cursor
    UIDirtyMain();
  } else if (m_mode == kPlacement && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing blueprint 
    pd->sprite->setVisible(getPlayer()->m_blueprint[getZoom()], _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  }
  if (_fc % FAR_TICK_FREQUENCY == 0) {
    // Update bottom ticker
    UIDirtyBottom();
  }

  if (m_mode == kWander) {
    if (!m_UITopVisible) {
      if      (distanceFromBuy()  < ACTIVATE_DISTANCE) drawUITop("The Shop");
      else if (distanceFromSell() < ACTIVATE_DISTANCE) drawUITop("Sell Box");
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

  const uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  const uint16_t selectedID =  m_contentID[m_selRow][m_selCol];


  if (gm == kPick) {
    setPlayerLookingAtOffset(0);
    pd->sprite->setImage(bp, getSprite16_byidx(0, zoom), kBitmapUnflipped); 
    pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped);
  } else if (gm == kPlacement) { // Of conveyors, cargo or utility
    setPlayerLookingAtOffset(0);
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
    switch (selectedCat) {
      case 2: pd->sprite->setImage(bp, getSprite16_byidx(kConvUIIcon[selectedID] + m_selRotation, zoom), kBitmapUnflipped); break;
      case 5: pd->sprite->setImage(bp, getSprite16_byidx(kUtilityUIIcon[selectedID] + m_selRotation, zoom), kBitmapUnflipped); break;
      case 6: pd->sprite->setImage(bp, getSprite16_byidx(kCargoUIIcon[selectedID], zoom), kBitmapUnflipped); break;
    }
  } else if (gm == kPlantMode) { // Of crops
    setPlayerLookingAtOffset(0);
    pd->sprite->setImage(bp, getSprite16_byidx(kPlantUIIcon[selectedID], zoom), kBitmapUnflipped); 
    pd->sprite->setImage(bpRadius, getSprite16_byidx(0, zoom), kBitmapUnflipped);
  } else if (gm == kBuild) { // Of factories and harvesters
    setPlayerLookingAtOffset(2);
    pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap7x7[zoom], kBitmapUnflipped);
    switch (selectedCat) {
      case 3: pd->sprite->setImage(bp, getSprite48_byidx(kExtractorSprite[selectedID] + m_selRotation, zoom), kBitmapUnflipped); break;
      case 4: pd->sprite->setImage(bp, getSprite48_byidx(kFactorySprite[selectedID] + m_selRotation,   zoom), kBitmapUnflipped); break;
    }
  
/*

    //pd->sprite->setVisible(getPlayer()->m_blueprint[zoom], true);
    switch (m_UISelectedID) {
      case kMenuConveyor:;  pd->sprite->setImage(bp, getSprite16(m_UISelectedRotation+0, 3, zoom),    kBitmapUnflipped); setPlayerLookingAtOffset(0); break;
      case kMenuSplitI:;    pd->sprite->setImage(bp, getSprite16(m_UISelectedRotation+0, 4, zoom),    kBitmapUnflipped); setPlayerLookingAtOffset(0); break;
      case kMenuSplitL:;    pd->sprite->setImage(bp, getSprite16(m_UISelectedRotation+4, 3, zoom),    kBitmapUnflipped); setPlayerLookingAtOffset(0); break;
      case kMenuSplitT:;    pd->sprite->setImage(bp, getSprite16(m_UISelectedRotation+4, 4, zoom),    kBitmapUnflipped); setPlayerLookingAtOffset(0); break;     
      case kMenuFilterL:;   pd->sprite->setImage(bp, getSprite16(m_UISelectedRotation+8, 4, zoom),    kBitmapUnflipped); setPlayerLookingAtOffset(0); break;
      case kMenuTunnel:;    pd->sprite->setImage(bp, getSprite16(m_UISelectedRotation+8, 3, zoom),    kBitmapUnflipped); setPlayerLookingAtOffset(0); break;
      case kMenuApple:;     pd->sprite->setImage(bp, getSprite16(m_UIIcons[12], m_UIIcons[13], zoom),   kBitmapUnflipped); setPlayerLookingAtOffset(0); break;     
      case kMenuCarrot:;    pd->sprite->setImage(bp, getSprite16(m_UIIcons[14], m_UIIcons[15], zoom), kBitmapUnflipped); setPlayerLookingAtOffset(0); break;     
      case kMenuWheat:;     pd->sprite->setImage(bp, getSprite16(m_UIIcons[16], m_UIIcons[17], zoom), kBitmapUnflipped); setPlayerLookingAtOffset(0); break;     
      case kMenuCheese:;    pd->sprite->setImage(bp, getSprite16(m_UIIcons[18], m_UIIcons[19], zoom), kBitmapUnflipped); setPlayerLookingAtOffset(0); break;     
      case kMenuExtractor:; pd->sprite->setImage(bp, getSprite48(m_UISelectedRotation, 0 , zoom),     kBitmapUnflipped); setPlayerLookingAtOffset(2);
                            pd->sprite->setImage(bpRadius, player->m_blueprintRadiusBitmap7x7[zoom], kBitmapUnflipped); break;     
      case kMenuBin:;       pd->sprite->setImage(bp, getSprite16(m_UIIcons[22], m_UIIcons[23], zoom), kBitmapUnflipped); setPlayerLookingAtOffset(1); break;     
    }
*/ 

  } else { // Clear blueprint
    //pd->sprite->setVisible(getPlayer()->m_blueprint[zoom], false);
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
    for (int32_t c = 0; c < CATEGORIES; ++c) {
      pd->sprite->addSprite(m_UISpriteHeaders[c]);
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
      toStringWetness(t->m_wetness), toStringSoil(t->m_tile), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true), toStringCargo(loc->m_cargo));
  } else if (loc->m_building) {
    snprintf(text, 128, "%s %s, %s", 
      toStringWetness(t->m_wetness), toStringSoil(t->m_tile), toStringBuilding(loc->m_building->m_type, loc->m_building->m_subType, true));
  } else if (loc->m_cargo) {
    snprintf(text, 128, "%s %s, %s", 
      toStringWetness(t->m_wetness), toStringSoil(t->m_tile), toStringCargo(loc->m_cargo));
  } else {
    snprintf(text, 128, "%s %s", 
      toStringWetness(t->m_wetness), toStringSoil(t->m_tile));
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
  if (gm == kPlacement || gm == kPlantMode || gm == kBuild) {
    int16_t rotMod = (m_selRotation == 0 ? 3 : m_selRotation - 1); // We are drawing this sideways, so need to rotate it by pi/2
    const uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
    const uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
    snprintf(text, 32, "%u", (unsigned) getOwned(selectedCat, selectedID));
    switch (selectedCat) {
      case 0: pd->graphics->drawBitmap(getSprite16_byidx(kToolUIIcon[selectedID], 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
      case 1: pd->graphics->drawBitmap(getSprite16_byidx(kPlantUIIcon[selectedID], 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
      case 2: pd->graphics->drawBitmap(getSprite16_byidx(kConvUIIcon[selectedID] + rotMod, 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
      case 3: pd->graphics->drawBitmap(getSprite16_byidx(kExtractorUIIcon[selectedID] + rotMod, 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
      case 4: pd->graphics->drawBitmap(getSprite16_byidx(kFactoryUIIcon[selectedID] + rotMod, 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
      case 5: pd->graphics->drawBitmap(getSprite16_byidx(kUtilityUIIcon[selectedID] + rotMod, 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
      case 6: pd->graphics->drawBitmap(getSprite16_byidx(kCargoUIIcon[selectedID], 1), DEVICE_PIX_Y/2, 0, kBitmapUnflipped); break;
    }
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

int32_t getUnlockCost(int32_t _c, int32_t _i) {
  switch (_c) {
    case 0: return 0;
    case 1: return kPlantUnlock[_i];
    case 2: return kConvUnlock[_i];
    case 3: return kExtractorUnlock[_i];
    case 4: return kFactoryUnlock[_i];
    case 5: return kUtilityUnlock[_i];
    case 6: return 0;
  }
  return 0;
}

int32_t getPrice(int32_t _c, int32_t _i) {
  switch (_c) {
    case 0: return 0;
    case 1: return kPlantPrice[_i];
    case 2: return kConvPrice[_i];
    case 3: return kExtractorPrice[_i];
    case 4: return kFactoryPrice[_i];
    case 5: return kUtilityPrice[_i];
    case 6: return kCargoValue[_i];
  }
  return 0;
}

uint16_t getOwned(int32_t _c, int32_t _i) {
  struct Player_t* p = getPlayer();
  switch (_c) {
    case 0: return 1;
    case 1: return p->m_carryPlant[_i];
    case 2: return p->m_carryConveyor[_i];
    case 3: return p->m_carryExtractor[_i];
    case 4: return p->m_carryFactory[_i];
    case 5: return p->m_carryUtility[_i];
    case 6: return p->m_carryCargo[_i];
  }
  return 0;
}

void modOwned(int32_t _c, int32_t _i, bool _add) {
  struct Player_t* p = getPlayer();
  switch (_c) {
    case 0: return;
    case 1: p->m_carryPlant[_i] += (_add ? 1 : -1); break;
    case 2: p->m_carryConveyor[_i] += (_add ? 1 : -1); break;
    case 3: p->m_carryExtractor[_i] += (_add ? 1 : -1); break;
    case 4: p->m_carryFactory[_i] += (_add ? 1 : -1); break;
    case 5: p->m_carryUtility[_i] += (_add ? 1 : -1); break;
    case 6: p->m_carryCargo[_i] += (_add ? 1 : -1); break;
  }
  UIDirtyRight();
}

uint16_t getNSubTypes(int32_t _c) {
  switch (_c) {
    case 0: return kNToolTypes;
    case 1: return kNPlantSubTypes;
    case 2: return kNConvSubTypes;
    case 3: return kNExtractorSubTypes;
    case 4: return kNFactorySubTypes;
    case 5: return kNUtilitySubTypes;
    case 6: return kNCargoType;
  }
  return 0;
}

enum kBuildingType getCatType(int32_t _c) {
  switch (_c) {
    case 0: return kNoBuilding;
    case 1: return kPlant;
    case 2: return kConveyor;
    case 3: return kExtractor;
    case 4: return kFactory;
    case 5: return kUtility;
    case 6: return kNoBuilding;
  }
  return kNoBuilding;
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
    m_cursorRowAbs = 1;
  }
}

void moveCursor(uint32_t _button) {
  if        (kButtonUp    == _button) {
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

void doPurchace() {
  const uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  const uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
  const int32_t selectedPrice = getPrice(selectedCat, selectedID);
  if (modMoney(-selectedPrice)) {
    modOwned(selectedCat, selectedID, /*add=*/ true);
    UIDirtyMain();
  }
}

void doSale() {

}

void doPlayerMenuClick() {
  const uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  const uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
  switch (selectedCat) {
    case 0: switch(selectedID) {
      case kToolPickup:; return setGameMode(kPick);
      case kToolInspect:; return setGameMode(kInspect);
      case kToolDestroy:; return setGameMode(kDestroy);
      case kNToolTypes:; break;
    } break;
    case 1: return setGameMode(kPlantMode);
    case 2: return setGameMode(kPlacement); 
    case 3: return setGameMode(kBuild);
    case 4: return setGameMode(kBuild);
    case 5: return setGameMode(kPlacement);
    case 6: return setGameMode(kPlacement); 
  }
}

void doPlace() {
  const uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  const uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
  if (getOwned(selectedCat, selectedID) == 0) {
    return;
  }
  bool placed = false;
  switch (selectedCat) {
    case 1: placed = newBuilding(getPlayerLookingAtLocation(), SN, kPlant, (union kSubType) {.plant = selectedID} ); break;
    case 2: placed = newBuilding(getPlayerLookingAtLocation(), m_selRotation, kConveyor, (union kSubType) {.conveyor = selectedID} ); break;
    case 3: placed = newBuilding(getPlayerLookingAtLocation(), m_selRotation, kExtractor, (union kSubType) {.extractor = selectedID} ); break;
    case 4: placed = newBuilding(getPlayerLookingAtLocation(), m_selRotation, kFactory, (union kSubType) {.factory = selectedID} ); break;
    case 5: placed = newBuilding(getPlayerLookingAtLocation(), m_selRotation, kUtility, (union kSubType) {.utility = selectedID} ); break;
    case 6: placed = newCargo(getPlayerLookingAtLocation(), selectedID, true); break;
  }
  if (placed) {
    modOwned(selectedCat, selectedID, /*add=*/ false);
  }
}



void doPick() {
  bool update = false;
  struct Location_t* ploc = getPlayerLocation();
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Location_t* loc = getLocation(ploc->m_x + x, ploc->m_y + y);
      if (loc->m_cargo) {
        modOwned(6, loc->m_cargo->m_type, /*add=*/ true); // 6 is cargo TODO enum this
        clearLocation(loc, /*cargo=*/ true, /*building=*/ false);
        update = true;
      }
    }
  }
  if (update) updateRenderList();
}

void doDestroy() {


}
    /*
    switch (getUISelectedID()) {
      case kMenuConveyor:; newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kBelt}   ); break;
      case kMenuSplitI:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kSplitI} ); break;
      case kMenuSplitL:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kSplitL} ); break;
      case kMenuSplitT:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kSplitT} ); break;
      case kMenuFilterL:;  newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kFilterL} ); break;
      case kMenuTunnel:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kTunnelIn} ); break;
      case kMenuApple:;    newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kPlant, (union kSubType) {.plant = kAppleTree} ); break;
      case kMenuCarrot:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kPlant, (union kSubType) {.plant = kCarrotPlant} ); break;
      case kMenuWheat:;    newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kPlant, (union kSubType) {.plant = kWheatPlant} ); break;
      case kMenuCheese:;   newCargo(getPlayerLookingAtLocation(), kCheese, true); break;
      case kMenuExtractor:;newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kExtractor, (union kSubType) {.extractor = kCropHarvester} ); break;
      case kMenuBin:;      clearLocation(getPlayerLookingAtLocation(), true, true); break; 
    }
    */

void populateContentPlayer(void) {
  struct Player_t* p = getPlayer();
  int16_t column = 0, row = 0;
  for (int32_t c = 0; c < CATEGORIES; ++c) {
    bool owned = false;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      if (getOwned(c, i)) {
        owned = true;
        break;
      }
    }
    if (!owned) { // Don't own anything in this category
      continue;
    }
    m_contentSprite[row][0] = m_UISpriteHeaders[c];
    m_rowIsTitle[row] = true;
    ++row;
    column = 0;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      if (!getOwned(c, i)) {
        continue;
      }
      int16_t rot = 0;
      if (c >= 2 && c <= 5) rot = m_selRotation;
      m_contentSprite[row][column] = m_UISpriteItems[c][i][rot];
      m_contentCat[row][column] = c;
      m_contentID[row][column] = i; 
      if (++column == ROW_WDTH) {
        ++row;
        column = 0;
      }
    }
    ++row;
  }
}


void populateContentBuy(void) {
  struct Player_t* p = getPlayer();
  const uint32_t hwm = p->m_moneyHighWaterMark;
  int16_t column = 0, row = 0;
  // Miss the first (Tools). Miss the last (Cargo)
  for (int32_t c = 1; c < CATEGORIES-1; ++c) {
    int32_t firstEntryCost = getUnlockCost(c, 0);
    if (hwm < firstEntryCost) { // If can unlock 1st entry
      continue;
    }
    m_contentSprite[row][0] = m_UISpriteHeaders[c];
    m_rowIsTitle[row] = true;
    ++row;
    column = 0;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      if (hwm < getUnlockCost(c, i)) {
        continue;
      }
      m_contentSprite[row][column] = m_UISpriteItems[c][i][0];
      m_contentCat[row][column] = c;
      m_contentID[row][column] = i; 
      if (++column == ROW_WDTH) {
        ++row;
        column = 0;
      }
    }
    ++row;
  }
}

void populateInfoPlayer(void) {
  uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  enum kBuildingType selectedCatType = getCatType(selectedCat);
  uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
  int32_t selectedPrice = getPrice(selectedCat, selectedID);
  uint16_t selectedOwned = getOwned(selectedCat, selectedID);

  // AFFORD (N/A)
  pd->sprite->setVisible(m_UISpriteCannotAfford, 0);

  // INFO
  static char textA[128] = "";
  static char textB[128] = "";
  static char textC[128] = "";
  setRoobert10();
  pd->graphics->clearBitmap(m_UIBitmapInfo, kColorClear);
  pd->graphics->pushContext(m_UIBitmapInfo);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  switch (selectedCat) {
    case 0:  snprintf(textA, 128, "%s", toStringTool(selectedID)); snprintf(textB, 128, "%s", toStringToolInfo(selectedID)); break;
    case 1:; snprintf(textA, 128, "Plant %s, Likes: %s %s",
      toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
      toStringWetness(kPlantWetness[selectedID]),
      toStringSoil(kPlantSoil[selectedID])); break;
    case 2:; snprintf(textA, 128, "Place %s (%s)", toStringBuilding(selectedCatType, (union kSubType) {.conveyor = selectedID}, false), getRotationAsString()); break;
    case 3:; snprintf(textA, 128, "Build %s", toStringBuilding(selectedCatType, (union kSubType) {.extractor = selectedID}, false)); break;
    case 4:; snprintf(textA, 128, "Build %s", toStringBuilding(selectedCatType, (union kSubType) {.factory = selectedID}, false)); break;
    case 5:; snprintf(textA, 128, "Place %s", toStringBuilding(selectedCatType, (union kSubType) {.utility = selectedID}, false)); break;
    case 6:; snprintf(textA, 128, "Place %s", toStringCargoByType(selectedID)); break;
  }
  if (selectedCat != 0) snprintf(textB, 128, "Inventory: %i", selectedOwned);
  if (selectedCat == 6) snprintf(textC, 128, "Value:      %i", (int)selectedPrice);
  else snprintf(textC, 128, " ");
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, 9*TILE_PIX, TILE_PIX - 2);
  pd->graphics->setDrawMode(kDrawModeCopy);
  if (selectedCat == 6) pd->graphics->drawBitmap(getSprite16(2, 2, 1), 11*TILE_PIX + TILE_PIX/2, TILE_PIX - 2, kBitmapUnflipped);
  pd->graphics->popContext();
  pd->sprite->setVisible(m_UISpriteInfo, 1);
}

void populateInfoBuy(bool _visible) {
  struct Player_t* p = getPlayer();
  uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  enum kBuildingType selectedCatType = getCatType(selectedCat);
  uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
  int32_t selectedPrice = getPrice(selectedCat, selectedID);
  uint16_t selectedOwned = getOwned(selectedCat, selectedID);

  // AFFORD
  bool canAfford = (p->m_money >= selectedPrice);
  pd->sprite->setVisible(m_UISpriteCannotAfford, !canAfford && _visible);

  // INFO
  static char textA[128] = "";
  static char textB[128] = "";
  static char textC[128] = "";
  setRoobert10();
  pd->graphics->clearBitmap(m_UIBitmapInfo, kColorClear);
  pd->graphics->pushContext(m_UIBitmapInfo);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  switch (selectedCatType) {
    case kPlant:; snprintf(textA, 128, "Buy %s, Likes: %s %s",
      toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
      toStringWetness(kPlantWetness[selectedID]),
      toStringSoil(kPlantSoil[selectedID])); break;
    case kConveyor:; snprintf(textA, 128, "Buy %s", toStringBuilding(selectedCatType, (union kSubType) {.conveyor = selectedID}, false)); break;
    case kExtractor:; snprintf(textA, 128, "Buy %s", toStringBuilding(selectedCatType, (union kSubType) {.extractor = selectedID}, false)); break;
    case kFactory:; snprintf(textA, 128, "Buy %s", toStringBuilding(selectedCatType, (union kSubType) {.factory = selectedID}, false)); break;
    case kUtility:; snprintf(textA, 128, "Buy %s", toStringBuilding(selectedCatType, (union kSubType) {.utility = selectedID}, false)); break;
    case kSpecial:; case kNoBuilding:; case kNBuildingTypes:; break;
  }
  snprintf(textB, 128, "Inventory: %i", selectedOwned);
  snprintf(textC, 128, "Price:     %i", (int)selectedPrice);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, 9*TILE_PIX, TILE_PIX - 2);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(2, 2, 1), 11*TILE_PIX + TILE_PIX/4, TILE_PIX - 2, kBitmapUnflipped);
  pd->graphics->popContext();
  pd->sprite->setVisible(m_UISpriteInfo, 1);
} 


void drawUIMain() {

  memset(m_contentSprite, 0, MAX_ROWS * ROW_WDTH * sizeof(LCDSprite*));
  memset(m_rowIsTitle, 0, MAX_ROWS * sizeof(bool));

  // POPULATE
  switch (getGameMode()) {
    case kMenuBuy:; populateContentBuy(); break;
    case kMenuPlayer:; populateContentPlayer(); break;
    default: break;
  }

  checkSel();

  // DRAW
  // Mark invisible
  for (int32_t c = 0; c < CATEGORIES; ++c) {
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
    case kMenuPlayer:; populateInfoPlayer(); break;
    default: break;
  }

///XXX



}


void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;

  if (_mode == kPlacement) {
    drawUITop("Place Mode");
  } else if (_mode == kPlantMode) {
    drawUITop("Plant Mode");
  } else if (_mode == kBuild) {
    drawUITop("Build Mode");
  } else if (_mode == kPick) {
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
  setGameMode(kWander);
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

  for (int32_t c = 0; c < CATEGORIES; ++c) {
    m_UIBitmapHeaders[c] = pd->graphics->newBitmap(TILE_PIX*18, TILE_PIX*2, kColorClear);
    pd->graphics->pushContext(m_UIBitmapHeaders[c]);
    roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    switch (c) {
      case 0: pd->graphics->drawText("Tools", 5, kASCIIEncoding, TILE_PIX, 0); break;
      case 1: pd->graphics->drawText("Crops", 5, kASCIIEncoding, TILE_PIX, 0); break;
      case 2: pd->graphics->drawText("Conveyors", 9, kASCIIEncoding, TILE_PIX, 0); break;
      case 3: pd->graphics->drawText("Harvesters", 10, kASCIIEncoding, TILE_PIX, 0); break;
      case 4: pd->graphics->drawText("Factories", 9, kASCIIEncoding, TILE_PIX, 0); break;
      case 5: pd->graphics->drawText("Utility", 7, kASCIIEncoding, TILE_PIX, 0); break;
      case 6: pd->graphics->drawText("Cargo", 5, kASCIIEncoding, TILE_PIX, 0); break;
    }
    pd->graphics->popContext();

    m_UISpriteHeaders[c] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_UISpriteHeaders[c], cBound);
    pd->sprite->setImage(m_UISpriteHeaders[c], m_UIBitmapHeaders[c], kBitmapUnflipped);
    pd->sprite->setZIndex(m_UISpriteHeaders[c], Z_INDEX_UI_M);
    pd->sprite->setIgnoresDrawOffset(m_UISpriteHeaders[c], 1);

    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      uint16_t spriteID = 0;
      switch (c) {
        case 0: spriteID = kToolUIIcon[i]; break;
        case 1: spriteID = kPlantUIIcon[i]; break;
        case 2: spriteID = kConvUIIcon[i]; break;
        case 3: spriteID = kExtractorUIIcon[i]; break;
        case 4: spriteID = kFactoryUIIcon[i]; break;
        case 5: spriteID = kUtilityUIIcon[i]; break;
        case 6: spriteID = kCargoUIIcon[i]; break;
      }
      for (int32_t r = 0; r < 4; ++r) {
        m_UIBitmapItems[c][i][r] = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorClear);
        pd->graphics->pushContext(m_UIBitmapItems[c][i][r]);
        roundedRect(1, TILE_PIX*2, TILE_PIX*2, TILE_PIX/2, kColorBlack);
        roundedRect(3, TILE_PIX*2, TILE_PIX*2, TILE_PIX/2, kColorWhite);
        if (c == 1) { // Crops are seeds in a packet
          pd->graphics->drawBitmap(getSprite16(3, 2, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID + r, 1), TILE_PIX/2, TILE_PIX/2, kBitmapUnflipped);

        } else {
          pd->graphics->setDrawMode(kDrawModeNXOR);
          pd->graphics->drawBitmap(getSprite16_byidx(spriteID + r, 2), 0, 0, kBitmapUnflipped);
          pd->graphics->setDrawMode(kDrawModeCopy);
        }
        pd->graphics->popContext();

        m_UISpriteItems[c][i][r] = pd->sprite->newSprite();
        pd->sprite->setBounds(m_UISpriteItems[c][i][r], iBound);
        pd->sprite->setImage(m_UISpriteItems[c][i][r], m_UIBitmapItems[c][i][r], kBitmapUnflipped);
        pd->sprite->setZIndex(m_UISpriteItems[c][i][r], Z_INDEX_UI_M);
        pd->sprite->setIgnoresDrawOffset(m_UISpriteItems[c][i][r], 1);

        if (c == 0 || c == 1 || c == 6) { // Tools, Crops & Cargo don't have any rotation 
          break;
        }
      }
    }
  }

  UIDirtyBottom();
  UIDirtyRight();
}