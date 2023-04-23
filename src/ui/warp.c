#include "warp.h"
#include "../player.h"
#include "../sprite.h"
#include "../io.h"
#include "../generate.h"
#include "../render.h"
#include "../sound.h"

/// ///

void doWarp() {
  const uint16_t selectedID =  getUIContentID();

  if (selectedID == getSlot()) {
    setGameMode(kWanderMode);
  } else if (getOwned(kUICatWarp, selectedID) == 0) { // Not yet unlocked
    
  #ifdef FAST_PROGRESS
    const int32_t selectedPriceOrAmount = FAST_PROGRESS_SALES;
  #else
    const int32_t selectedPriceOrAmount = getPrice(kUICatWarp, selectedID);
  #endif

    bool doUnlock = false;

    if (WDesc[selectedID].unlock != kNoCargo) {
      doUnlock = (getOwned(kUICatCargo, WDesc[selectedID].unlock) >= selectedPriceOrAmount || getPlayer()->m_infiniteMoney);
      if (doUnlock && !getPlayer()->m_infiniteMoney) {
        for (int32_t dummy = 0; dummy < selectedPriceOrAmount; ++dummy) {
          modOwned(kUICatCargo, WDesc[selectedID].unlock, /*add=*/false);
        }
      }
    } else {
      doUnlock = modMoney(-selectedPriceOrAmount);
    }

    if (doUnlock) {
      setForceSlot(selectedID); // This will be the ID of the world we generate
      setGameMode(kTruckModeNew);
      sfx(kSfxWarp);
    } else {
      sfx(kSfxNo);
    }
  } else { // Unlocked world, switch
    setForceSlot(selectedID); // This will be the ID of the slot we load from
    setGameMode(kTruckModeLoad);
    sfx(kSfxWarp);
  }
}

void populateInfoWarp(bool _visible) {
  const uint16_t selectedID =  getUIContentID();

#ifdef FAST_PROGRESS
  const int32_t selectedPriceOrAmount = FAST_PROGRESS_SALES;
#else
  const int32_t selectedPriceOrAmount = getPrice(kUICatWarp, selectedID);
#endif
  
  const uint16_t selectedOwned = getOwned(kUICatWarp, selectedID);

  bool canAfford = (getPlayer()->m_money >= selectedPriceOrAmount || getPlayer()->m_infiniteMoney);
  // Some slots are not unlocked with money
  if (WDesc[selectedID].unlock != kNoCargo) {
    canAfford = (getOwned(kUICatCargo, WDesc[selectedID].unlock) >= selectedPriceOrAmount);
  } 

  pd->sprite->setVisible(getCannotAffordSprite(), _visible && !canAfford && !selectedOwned);

  // INFO
  LCDBitmap* infoBitmap = getInfoBitmap();
  static char textA[256] = " ";
  static char textB[256] = " ";
  setRoobert10();
  pd->graphics->clearBitmap(infoBitmap, kColorClear);
  pd->graphics->pushContext(infoBitmap);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);
  if (selectedOwned) {
    strcpy(textA, tr(kTRWarpLocations));
    strcat(textA, cspace());
    strcat(textA, getWorldName(selectedID, /*mask*/ false));
    if (selectedID < FLOOR_TYPES) {
      strcpy(textB, tr(kTRWarpSoil));
      strcat(textB, cspace());
      strcat(textB, toStringSoil(getWorldGround(selectedID, 0)));
      strcat(textB, Cspace());
      strcat(textB, toStringSoil(getWorldGround(selectedID, 1)));
      strcat(textB, Cspace());
      strcat(textB, toStringSoil(getWorldGround(selectedID, 2)));
    } else if (selectedID == kWaterWorld) {
      strcpy(textB, tr(kTRWarpSoil));
      strcat(textB, cspace());
      strcat(textB, tr(kTRWater));
      strcat(textB, Cspace());
      strcat(textB, toStringSoil(getWorldGround(kClayWorld, 0)));
    } else if (selectedID == kTranquilWorld) {
      strcpy(textB, tr(kTRWarpSoil));
      strcat(textB, cspace());
      strcat(textB, toStringSoil(getWorldGround(kLoamWoarld, 0)));
    }
  } else {
    strcpy(textA, tr(kTRWarpLocations));
    strcat(textA, cspace());
    strcat(textA, getWorldName(selectedID, /*mask*/ true));

    char textM[32] = "";
    snprintf_c(textM, 32, selectedPriceOrAmount);

    strcpy(textB, tr(kTRShopPrice));
    strcat(textB, c5space());
    strcat(textB, textM);
    if (WDesc[selectedID].unlock == kNoCargo) { // buy for gold
      pd->graphics->drawBitmap(getSprite16(2, 16, 1), TILE_PIX/2 + trLen(kTRShopPrice), TILE_PIX - 2, kBitmapUnflipped); // Coin
    } else {
      pd->graphics->drawBitmap(getSprite16_byidx( CargoDesc[WDesc[selectedID].unlock].UIIcon , 1), TILE_PIX/2 + trLen(kTRShopPrice), TILE_PIX - 2, kBitmapUnflipped);
    }
  }
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(textA, 256, kUTF8Encoding, TILE_PIX/2, +2 +tY());
  pd->graphics->drawText(textB, 256, kUTF8Encoding, TILE_PIX/2, TILE_PIX - 2 +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);

  pd->graphics->popContext();
}

void populateContentWarp() {
  struct Player_t* p = getPlayer();
  int16_t column = 0, row = 0;
  setUIContentHeader(row, kUICatWarp);
  ++row;
  // We actually update the bitmaps here which is not ideal
  for (int32_t i = 0; i < getNSubTypes(kUICatWarp); ++i) {
    LCDBitmap* b = getUIContentItemBitmap(kUICatWarp, i, 0);
    pd->graphics->pushContext(b);
    if (column == ROW_WDTH) { // Only start a new row if we are sure we have something to put in it
      ++row;
      column = 0;
    }
    setUIContentItem(row, column, kUICatWarp, i, 0);
    if (getOwned(kUICatWarp, i)) {
      pd->graphics->drawBitmap(getSprite16_byidx(getUIIcon(kUICatWarp, i), 2), 0, 0, kBitmapUnflipped);
    } else {
      pd->graphics->drawBitmap(getSprite16(11,10,2), 0, 0, kBitmapUnflipped);
    }
    pd->graphics->popContext();
    if (column == getSlot()) {
      setUIContentStickySelected(row, column, 0);
    }
    ++column;
  }
}