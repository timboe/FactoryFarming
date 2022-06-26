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

enum kGameMode m_mode;

LCDSprite* m_UISpriteBottom;

LCDSprite* m_UISpriteRight;

LCDSprite* m_UISpriteTop;

LCDBitmap* m_UIBitmapBottom;

LCDBitmap* m_UIBitmapRight;

LCDBitmap* m_UIBitmapTop;

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

#define CATEGORIES 5
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

//

void drawUIBottom(void);

void drawUIRight(void);

void drawUIBuy(void);

void roundedRect(uint16_t _o, uint16_t _w, uint16_t _h, uint16_t _r, LCDColor _c);

//const char* getRotationAsString(void);

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
}

void updateUI(int _fc) {
  
  if ((m_mode == kMenuBuy || m_mode == kMenuPlayer) && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing cursor
    UIDirtyMain();
  } else if (m_mode == kPlacement && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing blueprint 
    pd->sprite->setVisible(getPlayer()->m_blueprint[getZoom()], _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  } else if (m_mode == kWander && _fc % FAR_TICK_FREQUENCY == 0) {
    // Update bottom ticker
    UIDirtyBottom();
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
    if (m_mode == kMenuBuy) drawUIBuy();
  }
}

void updateBlueprint() {
  uint8_t zoom = getZoom();
  struct Player_t* player = getPlayer();
  LCDSprite* bp = player->m_blueprint[zoom];
  LCDSprite* bpRadius = player->m_blueprintRadius[zoom];
  if (getGameMode() == kPlacement) {

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

  if (m_mode == kMenuBuy || m_mode == kMenuPlayer) {
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

/*
const char* getRotationAsString() {
  switch (m_UISelectedID) {
    case kMenuConveyor:; case kMenuTunnel:;
      switch (m_UISelectedRotation) {
        case 0: return "N";
        case 1: return "E";
        case 2: return "S";
        case 3: return "W";
      }
    case kMenuSplitI:;
      switch (m_UISelectedRotation) {
        case 0: return "W, E";
        case 1: return "N, S";
        case 2: return "W, E";
        case 3: return "N, S";
      }
    case kMenuSplitL:; case kMenuFilterL:;
      switch (m_UISelectedRotation) {
        case 0: return "N, E";
        case 1: return "E, S";
        case 2: return "S, W";
        case 3: return "W, N";
      }
    case kMenuSplitT:;
      switch (m_UISelectedRotation) {
        case 0: return "W, N, E";
        case 1: return "N, E, S";
        case 2: return "E, S, W";
        case 3: return "S, W, N";
      }
  }
  return "!";
}
*/

void drawUIBottom() {
  static char text[128];
  pd->graphics->pushContext(m_UIBitmapBottom);
  pd->graphics->fillRect(0, 0, DEVICE_PIX_X, TILE_PIX, kColorBlack);
  setRoobert10();
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  //const enum kGameMode gm = getGameMode();
  //if (gm == kWander) {

  int32_t mode = 0;
  if (pd->system->getElapsedTime() < 3.0f) {
    mode = 0;
  } else if (pd->system->getElapsedTime() < 6.0f) {
    mode = 1;
  } else if (pd->system->getElapsedTime() < 9.0f) {
    mode = 2;
  } else if (pd->system->getElapsedTime() > 12.0f) {
    pd->system->resetElapsedTime();
  }

  if (mode == 0) {
    snprintf(text, 128, "Conveyors:%u", getNByType(kConveyor));
    pd->graphics->drawText(text, 128, kASCIIEncoding, 0, 0);
    snprintf(text, 128, "Cargo:%u", getNCargo());
    pd->graphics->drawText(text, 128, kASCIIEncoding, 7*TILE_PIX, 0);
    snprintf(text, 128, "Sprite List:%u", pd->sprite->getSpriteCount());
    pd->graphics->drawText(text, 128, kASCIIEncoding, 12*TILE_PIX, 0);
  } else if (mode == 1) {
    snprintf(text, 128, "Near Ticks:%u", getNearTickCount());
    pd->graphics->drawText(text, 128, kASCIIEncoding, 0, 0);
    snprintf(text, 128, "Far Ticks:%u", getFarTickCount());
    pd->graphics->drawText(text, 128, kASCIIEncoding, 7*TILE_PIX, 0);
    snprintf(text, 128, "Money:%u", (unsigned) getPlayer()->m_money);
    pd->graphics->drawText(text, 128, kASCIIEncoding, 14*TILE_PIX, 0);
  } else if (mode == 2) {
    struct Location_t* loc = getPlayerLocation();
    struct Tile_t* t = getTile_fromLocation(loc);
    snprintf(text, 128, "%s (%s), B:%s, C:%s", 
      toStringSoil(t->m_tile), toStringWetness(t->m_wetness), toStringBuilding(loc->m_building), toStringCargo(loc->m_cargo));
    pd->graphics->drawText(text, 128, kASCIIEncoding, 0, 0);
  }

/*
  } else if (gm == kMenuSelect || gm == kPlacement) {
    switch (m_UISelectedID) {
      case kMenuConveyor:;  snprintf(text, 128, "Conveyor Belt (%s)", getRotationAsString()); break;
      case kMenuSplitI:;    snprintf(text, 128, "'I' Conveyor Splitter (%s)", getRotationAsString()); break;
      case kMenuSplitL:;    snprintf(text, 128, "'L' Conveyor Splitter (%s)", getRotationAsString()); break;
      case kMenuSplitT:;    snprintf(text, 128, "'T' Conveyor Splitter (%s)", getRotationAsString()); break;
      case kMenuFilterL:;   snprintf(text, 128, "'L' Conveyor Filter (%s)", getRotationAsString()); break;
      case kMenuTunnel:;    snprintf(text, 128, "Conveyor Tunnel (%s)", getRotationAsString()); break;
      case kMenuApple:;     snprintf(text, 128, "Apple tree, makes apples"); break;
      case kMenuCarrot:;    snprintf(text, 128, "Carrot seeds, grows carrots"); break;
      case kMenuWheat:;     snprintf(text, 128, "Wheat seeds, grows wheat"); break;
      case kMenuCheese:;    snprintf(text, 128, "Cheese. Can be put on conveyors."); break;
      case kMenuExtractor:; snprintf(text, 128, "Automatic Harvester"); break;
      case kMenuBin:;       snprintf(text, 128, "Remove conveyors & cargo"); break;
    }
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX, 0);
  }
*/
  pd->graphics->popContext();
}

void drawUIRight() {
  pd->graphics->pushContext(m_UIBitmapRight);
  pd->graphics->fillRect(0, 0, TILE_PIX, DEVICE_PIX_Y, kColorBlack);
  /*
  const enum kGameMode gm = getGameMode();
  for (uint32_t i = 0; i < UI_ITEMS; ++i) {
    const uint16_t y = TILE_PIX*i;
    const uint16_t offset = (i >= kMenuApple && i != kMenuExtractor) ? 0 : getUISelectedRotation();
    pd->graphics->drawBitmap(getSprite16(m_UIIcons[i*2] + offset, m_UIIcons[(i*2)+1], 1), 0, y, kBitmapUnflipped);
    if ((gm == kMenuSelect || gm == kPlacement) && i == getUISelectedID()) {
      LCDBitmapDrawMode m = ((gm == kMenuSelect && getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4) ? kDrawModeInverted : kDrawModeCopy );
      pd->graphics->setDrawMode(m);
      pd->graphics->drawBitmap(getSprite16(0, 2, 1), 0, y, kBitmapUnflipped);
      pd->graphics->setDrawMode(kDrawModeCopy);
    }
  }
  pd->graphics->popContext();
  */
  UIDirtyBottom();
}

void drawUITop(const char* _text) {
  pd->sprite->setVisible(m_UISpriteTop, !(_text == NULL) );
  if (_text == NULL) {
    return;
  }
  pd->graphics->pushContext(m_UIBitmapTop);
  pd->graphics->setLineCapStyle(kLineCapStyleRound);
  pd->graphics->drawLine(TILE_PIX, TILE_PIX, SCREEN_PIX_X/2 - TILE_PIX, TILE_PIX, TILE_PIX*2, kColorBlack);
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  setRoobert24();
  pd->graphics->drawText(_text, 16, kASCIIEncoding, TILE_PIX, 0);
  pd->graphics->popContext();
}

int32_t getUnlockCost(int32_t _c, int32_t _i) {
  switch (_c) {
    case 0: return kPlantUnlock[_i];
    case 1: return kConvUnlock[_i];
    case 2: return kExtractorUnlock[_i];
    case 3: return kFactoryUnlock[_i];
    case 4: return kUtilityUnlock[_i];
  }
  return 0;
}

int32_t getPrice(int32_t _c, int32_t _i) {
  switch (_c) {
    case 0: return kPlantPrice[_i];
    case 1: return kConvPrice[_i];
    case 2: return kExtractorPrice[_i];
    case 3: return kFactoryPrice[_i];
    case 4: return kUtilityPrice[_i];
  }
  return 0;
}

uint16_t getOwned(int32_t _c, int32_t _i) {
  struct Player_t* p = getPlayer();
  switch (_c) {
    case 0: return p->m_carryPlant[_i];
    case 1: return p->m_carryConveyor[_i];
    case 2: return p->m_carryExtractor[_i];
    case 3: return p->m_carryFactory[_i];
    case 4: return p->m_carryUtility[_i];
  }
  return 0;
}

uint16_t getNSubTypes(int32_t _c) {
  switch (_c) {
    case 0: return kNPlantSubTypes;
    case 1: return kNConvSubTypes;
    case 2: return kNExtractorSubTypes;
    case 3: return kNFactorySubTypes;
    case 4: return kNUtilitySubTypes;
  }
  return 0;
}

enum kBuildingType getCatType(int32_t _c) {
  switch (_c) {
    case 0: return kPlant;
    case 1: return kConveyor;
    case 2: return kExtractor;
    case 3: return kFactory;
    case 4: return kUtility;
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
  while (m_contentSprite[m_selRow][m_selCol] == NULL) {
    if (m_rowIsTitle[m_selRow] || m_selCol == 0) {
      moveRow(/*down=*/false);
      m_selCol = ROW_WDTH-1;
    } else {
      --m_selCol;
    }
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
  // special, force the top to be a title
  if (m_cursorRowAbs == 0) {
    --m_selRowOffset;
    m_cursorRowAbs = 1;
  }
  checkSel();
  UIDirtyMain();
}

void drawUIBuy() {

  struct Player_t* p = getPlayer();
  const uint32_t hwm = p->m_moneyHighWaterMark;

  //LCDSprite* content[MAX_ROWS][ROW_WDTH] = {NULL};
  //bool rowIsTitle[MAX_ROWS] = {false};
  memset(m_contentSprite, 0, MAX_ROWS * ROW_WDTH * sizeof(LCDSprite*));
  memset(m_rowIsTitle, 0, MAX_ROWS * sizeof(bool));

  int16_t column = 0, row = 0;

  for (int32_t c = 0; c < CATEGORIES; ++c) {
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
        break;
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

  const uint32_t UIStartY = TILE_PIX*5; // 4 + 1 (centre)
  const uint32_t UIStartX = TILE_PIX*3; // 2 + 1 (centre)

  for (int32_t r = 0; r < 4; ++r) {
    int32_t rID = r + m_selRowOffset;
    if (rID >= MAX_ROWS) break;
    if (m_contentSprite[rID][0] == NULL) break; // Not populated
    if (m_rowIsTitle[rID]) {
      pd->sprite->setVisible(m_contentSprite[rID][0], 1);
      pd->sprite->moveTo(m_contentSprite[rID][0], SCREEN_PIX_X/2 - TILE_PIX, UIStartY + r*2*TILE_PIX);
    } else {
      for (int32_t c = 0; c < ROW_WDTH; ++c) {
        if (m_contentSprite[rID][c] == NULL) break; // Not populated
        pd->sprite->setVisible(m_contentSprite[rID][c], 1);
        pd->sprite->moveTo(m_contentSprite[rID][c], UIStartX + c*2*TILE_PIX, UIStartY + r*2*TILE_PIX);
      }
    }
  }

  // CURSOR
  bool visible = (getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  pd->sprite->setVisible(m_UISpriteCursor, visible);
  pd->sprite->moveTo(m_UISpriteCursor, UIStartX + m_selCol*2*TILE_PIX, UIStartY + m_cursorRowAbs*2*TILE_PIX);

  // SELECTED
  LCDSprite* selectedSprite = m_contentSprite[m_selRow][m_selCol];
  uint16_t selectedCat = m_contentCat[m_selRow][m_selCol];
  enum kBuildingType selectedCatType = getCatType(selectedCat);
  uint16_t selectedID =  m_contentID[m_selRow][m_selCol];
  int32_t selectedPrice = getPrice(selectedCat, selectedID);
  uint16_t selectedOwned = getOwned(selectedCat, selectedID);
  pd->sprite->setImage(m_UISpriteSelected, pd->sprite->getImage(selectedSprite), kBitmapUnflipped);

  // AFFORD
  bool canAfford = (p->m_money >= selectedPrice);
  pd->sprite->setVisible(m_UISpriteCannotAfford, !canAfford && visible);

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
    case kPlant:; snprintf(textA, 128, "%s, Likes: %s %s",
      toStringBuildingByType(selectedCatType, (union kSubType) {.plant = selectedID}),
      toStringWetness(kPlantWetness[selectedID]),
      toStringSoil(kPlantSoil[selectedID])); break;
    case kConveyor:; snprintf(textA, 128, "%s", toStringBuildingByType(selectedCatType, (union kSubType) {.conveyor = selectedID})); break;
    case kExtractor:; snprintf(textA, 128, "%s", toStringBuildingByType(selectedCatType, (union kSubType) {.extractor = selectedID})); break;
    case kFactory:; snprintf(textA, 128, "%s", toStringBuildingByType(selectedCatType, (union kSubType) {.factory = selectedID})); break;
    case kUtility:; snprintf(textA, 128, "%s", toStringBuildingByType(selectedCatType, (union kSubType) {.utility = selectedID})); break;
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

///XXX



}


void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;

  if (_mode == kPlacement) drawUITop("Place Mode");
  else if (_mode == kMenuBuy) drawUITop("The Shop");
  else drawUITop(NULL);

  if (m_mode == kMenuBuy) UIDirtyMain();

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


  PDRect boundTop = {.x = 0, .y = 0, .width = SCREEN_PIX_X/2, .height = TILE_PIX*2};
  pd->sprite->setBounds(m_UISpriteTop, boundTop);
  pd->sprite->setImage(m_UISpriteTop, m_UIBitmapTop, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2, TILE_PIX);
  pd->sprite->setZIndex(m_UISpriteTop, Z_INDEX_MAX);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteTop, 1);
  pd->sprite->setVisible(m_UISpriteTop, 1);

  PDRect boundBottom = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = TILE_PIX};
  pd->sprite->setBounds(m_UISpriteBottom, boundBottom);
  pd->sprite->setImage(m_UISpriteBottom, m_UIBitmapRight, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteBottom, SCREEN_PIX_X + TILE_PIX/2, DEVICE_PIX_Y/2);
  pd->sprite->setZIndex(m_UISpriteBottom, Z_INDEX_UI_BOTTOM);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteBottom, 1);

  PDRect boundRight = {.x = 0, .y = 0, .width = DEVICE_PIX_X, .height = TILE_PIX};
  pd->sprite->setBounds(m_UISpriteRight, boundRight);
  pd->sprite->setImage(m_UISpriteRight, m_UIBitmapBottom, kBitmapUnflipped);
  pd->sprite->moveTo(m_UISpriteRight, DEVICE_PIX_X/2, SCREEN_PIX_Y + TILE_PIX/2);
  pd->sprite->setZIndex(m_UISpriteRight, Z_INDEX_UI_RIGHT);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteRight, 1);

  // Setup select / buy screen

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
    roundedRect(0, TILE_PIX*18, TILE_PIX*2, TILE_PIX, kColorBlack);
    pd->graphics->setDrawMode(kDrawModeFillWhite);
    switch (c) {
      case 0: pd->graphics->drawText("Crops", 5, kASCIIEncoding, TILE_PIX, 0); break;
      case 1: pd->graphics->drawText("Conveyors", 9, kASCIIEncoding, TILE_PIX, 0); break;
      case 2: pd->graphics->drawText("Harvesters", 10, kASCIIEncoding, TILE_PIX, 0); break;
      case 3: pd->graphics->drawText("Factories", 9, kASCIIEncoding, TILE_PIX, 0); break;
      case 4: pd->graphics->drawText("Utility", 7, kASCIIEncoding, TILE_PIX, 0); break;
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
        case 0: spriteID = kPlantUIIcon[i]; break;
        case 1: spriteID = kConvUIIcon[i]; break;
        case 2: spriteID = kExtractorUIIcon[i]; break;
        case 3: spriteID = kFactoryUIIcon[i]; break;
        case 4: spriteID = kUtilityUIIcon[i]; break;
      }
      for (int32_t r = 0; r < 4; ++r) {
        m_UIBitmapItems[c][i][r] = pd->graphics->newBitmap(TILE_PIX*2, TILE_PIX*2, kColorClear);
        pd->graphics->pushContext(m_UIBitmapItems[c][i][r]);
        roundedRect(1, TILE_PIX*2, TILE_PIX*2, TILE_PIX/2, kColorBlack);
        roundedRect(3, TILE_PIX*2, TILE_PIX*2, TILE_PIX/2, kColorWhite);
        pd->graphics->setDrawMode(kDrawModeNXOR);
        pd->graphics->drawBitmap(getSprite16_byidx(spriteID + r, 2), 0, 0, kBitmapUnflipped);
        pd->graphics->setDrawMode(kDrawModeCopy);
        pd->graphics->popContext();

        m_UISpriteItems[c][i][r] = pd->sprite->newSprite();
        pd->sprite->setBounds(m_UISpriteItems[c][i][r], iBound);
        pd->sprite->setImage(m_UISpriteItems[c][i][r], m_UIBitmapItems[c][i][r], kBitmapUnflipped);
        pd->sprite->setZIndex(m_UISpriteItems[c][i][r], Z_INDEX_UI_M);
        pd->sprite->setIgnoresDrawOffset(m_UISpriteItems[c][i][r], 1);

        if (c == 0 || c == 4) { // Crops & Utility don't have any rotation 
          break;
        }
      }
    }
  }

  UIDirtyBottom();
  UIDirtyRight();
}