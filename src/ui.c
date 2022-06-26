#include "ui.h"
#include "sprite.h"
#include "player.h"
#include "input.h"
#include "cargo.h"
#include "building.h"
#include "location.h"
#include "generate.h"
#include "cargo.h"

uint16_t m_UIIcons[] = {0,3,  0,4,  4,3,  4,4,  8,4,  8,3,  8,7,  10,7,  11,7,  9,7,  4,2,  1,2};

enum kGameMode m_mode;

LCDSprite* m_UISpriteBottom;

LCDSprite* m_UISpriteRight;

LCDSprite* m_UISpriteTop;

LCDBitmap* m_UIBitmapBottom;

LCDBitmap* m_UIBitmapRight;

LCDBitmap* m_UIBitmapTop;

bool m_UIDirtyBottom = false;

bool m_UIDirtyRight = false;

int16_t m_UISelectedID = 0;

int16_t m_UISelectedRotation = 0;

//

// Buy / select screen

LCDSprite* m_UISpriteCursor;

LCDSprite* m_UISpriteFull; // Main window backing
LCDBitmap* m_UIBitmapFull;

#define CATEGORIES 5
LCDSprite* m_UISpriteHeaders[CATEGORIES]; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 
LCDBitmap* m_UIBitmapHeaders[CATEGORIES]; // Category headers: Crops. Conveyors. Utility. Harvesters. Factories. 

#define ROW_WDTH 10

#define MAX_ROW_PER_CAT 3

#define MAX_PER_CAT (ROW_WDTH * MAX_ROW_PER_CAT)
LCDSprite* m_UISpriteItems[CATEGORIES][MAX_PER_CAT][4]; // Final 4 is for rotation states
LCDBitmap* m_UIBitmapItems[CATEGORIES][MAX_PER_CAT][4];

#define MAX_ROWS (MAX_ROW_PER_CAT*CATEGORIES + CATEGORIES)

uint16_t m_selCol = 0, m_selRow = 1;

LCDSprite* m_content[MAX_ROWS][ROW_WDTH] = {NULL};
bool m_rowIsTitle[MAX_ROWS] = {false};



//

void drawUIBottom(void);

void drawUIRight(void);

void drawUIBuy(void);


const char* getRotationAsString(void);

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

uint16_t getUISelectedID() {
  return m_UISelectedID;
}

uint16_t getUISelectedRotation() {
  return m_UISelectedRotation;
}

void modUISelectedID(bool _increment) {
  if (_increment) {
    m_UISelectedID = (m_UISelectedID == UI_ITEMS-1 ? 0 : m_UISelectedID + 1);
  } else {
    m_UISelectedID = (m_UISelectedID == 0 ? UI_ITEMS-1 : m_UISelectedID - 1);
  }
  UIDirtyRight();
}

void modUISelectedRotation(bool _increment) {
  if (_increment) {
    m_UISelectedRotation = (m_UISelectedRotation == kDirN-1 ? 0 : m_UISelectedRotation + 1);
  } else {
    m_UISelectedRotation = (m_UISelectedRotation == 0 ? kDirN-1 : m_UISelectedRotation - 1);
  }
  UIDirtyRight();
}

void updateUI(int _fc) {
  
  if (m_mode == kMenuSelect && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing cursor
    UIDirtyRight();
  } else if (m_mode == kMenuOptionSelected && _fc % (TICK_FREQUENCY/4) == 0) {
    // Flashing blueprint 
    pd->sprite->setVisible(getPlayer()->m_blueprint[getZoom()], _fc % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4);
  } else if (m_mode ==kWander && _fc % FAR_TICK_FREQUENCY == 0) {
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
}

void updateBlueprint() {
  uint8_t zoom = getZoom();
  struct Player_t* player = getPlayer();
  LCDSprite* bp = player->m_blueprint[zoom];
  LCDSprite* bpRadius = player->m_blueprintRadius[zoom];
  if (getGameMode() == kMenuOptionSelected) {
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

  pd->sprite->addSprite(m_UISpriteFull);
  pd->sprite->addSprite(m_UISpriteCursor);
  for (int32_t c = 0; c < CATEGORIES; ++c) {
    pd->sprite->addSprite(m_UISpriteHeaders[c]);
    for (int32_t i = 0; i < MAX_PER_CAT; ++i) {
      for (int32_t r = 0; r < 4; ++r) {
        if (m_UISpriteItems[c][i][r]) {
          pd->sprite->addSprite(m_UISpriteItems[c][i][r]);
        }
      }
    }
  }
}

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

void drawUIBottom() {
  static char text[128];
  pd->graphics->pushContext(m_UIBitmapBottom);
  pd->graphics->fillRect(0, 0, DEVICE_PIX_X, TILE_PIX, kColorBlack);
  setRoobert10();
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  const enum kGameMode gm = getGameMode();
  if (gm == kWander) {

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
        toStringSoil(t), toStringWetness(t), toStringBuilding(loc->m_building), toStringCargo(loc->m_cargo));
      pd->graphics->drawText(text, 128, kASCIIEncoding, 0, 0);
    }

  } else if (gm == kMenuSelect || gm == kMenuOptionSelected) {
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
  pd->graphics->popContext();
}

void drawUIRight() {
  pd->graphics->pushContext(m_UIBitmapRight);
  pd->graphics->fillRect(0, 0, TILE_PIX, DEVICE_PIX_Y, kColorBlack);
  const enum kGameMode gm = getGameMode();
  for (uint32_t i = 0; i < UI_ITEMS; ++i) {
    const uint16_t y = TILE_PIX*i;
    const uint16_t offset = (i >= kMenuApple && i != kMenuExtractor) ? 0 : getUISelectedRotation();
    pd->graphics->drawBitmap(getSprite16(m_UIIcons[i*2] + offset, m_UIIcons[(i*2)+1], 1), 0, y, kBitmapUnflipped);
    if ((gm == kMenuSelect || gm == kMenuOptionSelected) && i == getUISelectedID()) {
      LCDBitmapDrawMode m = ((gm == kMenuSelect && getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4) ? kDrawModeInverted : kDrawModeCopy );
      pd->graphics->setDrawMode(m);
      pd->graphics->drawBitmap(getSprite16(0, 2, 1), 0, y, kBitmapUnflipped);
      pd->graphics->setDrawMode(kDrawModeCopy);
    }
  }
  pd->graphics->popContext();
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

uint16_t getSubTypes(int32_t _c) {
  switch (_c) {
    case 0: return kNPlantSubTypes;
    case 1: return kNConvSubTypes;
    case 2: return kNExtractorSubTypes;
    case 3: return kNFactorySubTypes;
    case 4: return kNUtilitySubTypes;
  }
  return 0;
}

// Checks that the selected
void checkSel() {
  while (m_content[m_selRow][m_selCol] == NULL) {
    if (m_rowIsTitle[m_selRow] || m_selCol == 0) {
      --m_selRow;
      m_selCol = ROW_WDTH-1;
    } else {
      --m_selCol;
    }
  }
}

void drawUIBuy() {

  const uint32_t hwm = getPlayer()->m_moneyHighWaterMark;

  //LCDSprite* content[MAX_ROWS][ROW_WDTH] = {NULL};
  //bool rowIsTitle[MAX_ROWS] = {false};
  memset(m_content, 0, MAX_ROWS * ROW_WDTH * sizeof(LCDSprite*));
  memset(m_rowIsTitle, 0, MAX_ROWS * sizeof(bool));

  int16_t column = 0, row = 0;

  for (int32_t c = 0; c < CATEGORIES; ++c) {
    int32_t firstEntryCost = getUnlockCost(c, 0);
    if (hwm < firstEntryCost) { // If can unlock 1st entry
      continue;
    }
    m_content[row][0] = m_UISpriteHeaders[c];
    m_rowIsTitle[row] = true;
    ++row;
    column = 0;
    for (int32_t i = 0; i < getSubTypes(c); ++i) {
      if (hwm < getUnlockCost(c, i)) {
        break;
      }
      m_content[row][column] = m_UISpriteItems[c][i][0];
      if (++column == ROW_WDTH) {
        ++row;
        column = 0;
      }
    }
    ++row;
  }


  // DRAW
  uint16_t rowOffset = 0;

  const uint32_t UIStartY = TILE_PIX*5; // 4 + 1 (centre)
  const uint32_t UIStartX = TILE_PIX*3; // 2 + 1 (centre)

  for (int32_t r = 0; r < 4; ++r) {
    int32_t rID = r + rowOffset;
    if (rID >= MAX_ROWS) break;
    if (m_content[rID][0] == NULL) break; // Not populated
    if (m_rowIsTitle[rID]) {
      pd->sprite->setVisible(m_content[rID][0], 1);
      pd->sprite->moveTo(m_content[rID][0], SCREEN_PIX_X/2, UIStartY + r*2*TILE_PIX);
    } else {
      for (int32_t c = 0; c < ROW_WDTH; ++c) {
        if (m_content[rID][c] == NULL) break; // Not populated
        pd->sprite->setVisible(m_content[rID][c], 1);
        pd->sprite->moveTo(m_content[rID][c], UIStartX + c*2*TILE_PIX, UIStartY + r*2*TILE_PIX);
      }
    }
  }
  

  // CURSOR
  checkSel();
  pd->sprite->setVisible(m_UISpriteCursor, 1);
  pd->sprite->moveTo(m_UISpriteCursor, UIStartX + m_selCol*2*TILE_PIX, UIStartY + m_selRow*2*TILE_PIX);



///XXX



}


void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;
  if (_mode == kMenuSelect) drawUITop("Menu Mode");
  else if (_mode == kMenuOptionSelected) drawUITop("Place Mode");
  else drawUITop(NULL);

  drawUIBuy();
}

void resetUI() {
  m_UISelectedID = 0;
  m_UISelectedRotation = 0;
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
  pd->sprite->moveTo(m_UISpriteTop, SCREEN_PIX_X/2, TILE_PIX*2);
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

  m_UISpriteFull = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteFull, fBound);
  pd->sprite->setImage(m_UISpriteFull, m_UIBitmapFull, kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteFull, Z_INDEX_UI_B);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteFull, 1);  
  pd->sprite->moveTo(m_UISpriteFull, SCREEN_PIX_X/2, SCREEN_PIX_Y/2);

  m_UISpriteCursor = pd->sprite->newSprite();
  pd->sprite->setBounds(m_UISpriteCursor, fBound);
  pd->sprite->setImage(m_UISpriteCursor, getSprite16(0, 2, 2), kBitmapUnflipped);
  pd->sprite->setZIndex(m_UISpriteCursor, Z_INDEX_UI_T);
  pd->sprite->setIgnoresDrawOffset(m_UISpriteCursor, 1);  

  setRoobert24();

  for (int32_t c = 0; c < CATEGORIES; ++c) {
    m_UIBitmapHeaders[c] = pd->graphics->newBitmap(TILE_PIX*20, TILE_PIX*2, kColorClear);
    pd->graphics->pushContext(m_UIBitmapHeaders[c]);
    roundedRect(0, TILE_PIX*20, TILE_PIX*2, TILE_PIX, kColorBlack);
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

    for (int32_t i = 0; i < getSubTypes(c); ++i) {
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