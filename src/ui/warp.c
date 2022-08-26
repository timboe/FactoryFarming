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
    const int32_t selectedPriceOrAmount = getPrice(kUICatWarp, selectedID);

    bool doUnlock = false;

    if (kWarpUnlockCargo[selectedID] != kNoCargo) {
      doUnlock = (getOwned(kUICatCargo, kWarpUnlockCargo[selectedID]) >= selectedPriceOrAmount);
      if (doUnlock) {
        for (int32_t dummy = 0; dummy < selectedPriceOrAmount; ++dummy) {
          modOwned(kUICatCargo, kWarpUnlockCargo[selectedID], /*add=*/false);
        }
      }
    } else {
      doUnlock = modMoney(-selectedPriceOrAmount);
    }

    if (doUnlock) {
      setForceSlot(selectedID); // This will be the ID of the world we generate
      doIO(kDoSave, /*and then*/ kDoNewWorld);
      // Tutorial
      if (getTutorialStage() == kTutNewPlots) {
        nextTutorialStage();
      }
    } else {
      sfx(kSfxNo);
    }
  } else { // Unlocked world, switch
    setForceSlot(selectedID); // This will be the ID of the slot we load from
    doIO(kDoSave, /*and then*/ kDoLoad);
  }
}

void populateInfoWarp(bool _visible) {
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPriceOrAmount = getPrice(kUICatWarp, selectedID);
  const uint16_t selectedOwned = getOwned(kUICatWarp, selectedID);

  bool canAfford = (getPlayer()->m_money >= selectedPriceOrAmount);
  // Some slots are not unlocked with money
  if (kWarpUnlockCargo[selectedID] != kNoCargo) {
    canAfford = (getOwned(kUICatCargo, kWarpUnlockCargo[selectedID]) >= selectedPriceOrAmount);
  } 

  pd->sprite->setVisible(getCannotAffordSprite(), _visible && !canAfford && !selectedOwned);

  // INFO
  LCDBitmap* infoBitmap = getInfoBitmap();
  static char textA[128] = " ";
  static char textB[128] = " ";
  setRoobert10();
  pd->graphics->clearBitmap(infoBitmap, kColorClear);
  pd->graphics->pushContext(infoBitmap);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);
  if (selectedOwned) {
    snprintf(textA, 128, "Plot Location: %s", getWorldName(selectedID, /*mask*/ false));
    if (selectedID < FLOOR_TYPES) {
      snprintf(textB, 128, "Soils: %s, %s, %s", 
        toStringSoil(getWorldGround(selectedID, 0)),
        toStringSoil(getWorldGround(selectedID, 1)),
        toStringSoil(getWorldGround(selectedID, 2)));
    } else if (selectedID == kWaterWorld) {
      snprintf(textB, 128, "Soils: Water, %s", toStringSoil(getWorldGround(kClayWorld, 0)));
    } else if (selectedID == kTranquilWorld) {
      snprintf(textB, 128, "Soils: %s", toStringSoil(getWorldGround(kLoamWoarld, 0)));
    }
  } else { 
    snprintf(textA, 128, "Plot Location: %s", getWorldName(selectedID, /*mask*/ true));
    snprintf(textB, 128, "Price:       %u", (unsigned)selectedPriceOrAmount);
    if (kWarpUnlockCargo[selectedID] == kNoCargo) { // buy for gold
      pd->graphics->drawBitmap(getSprite16(2, 16, 1), 3*TILE_PIX + TILE_PIX/2, TILE_PIX - 2, kBitmapUnflipped);
    } else {
      pd->graphics->drawBitmap(getSprite16_byidx( CargoDesc[kWarpUnlockCargo[selectedID]].UIIcon , 1), 3*TILE_PIX + TILE_PIX/2, TILE_PIX - 2, kBitmapUnflipped);
    }
  }
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
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