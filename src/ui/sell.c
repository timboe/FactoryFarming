#include "sell.h"
#include "../player.h"
#include "../sprite.h"
#include "../sound.h"
#include "../buildings/special.h"

/// ///

void doSale() {
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(kUICatCargo, selectedID);
  const int32_t owned = getOwned(kUICatCargo, selectedID);
  if (selectedID == kNoCargo) return;
  if (owned == 0) return;

  int32_t toSell = getBuySellMultiplier();
  if (toSell > owned) {
    toSell = owned;
  } 

  sfx(kSfxSell);
  UIDirtyMain();
  for (int32_t dummy = 0; dummy < toSell; ++dummy) {
    modMoney(selectedPrice);
    modOwned(kUICatCargo, selectedID, /*add=*/ false);
    getPlayer()->m_soldCargo[ selectedID ]++;
    // Tutorial
    if (getTutorialStage() == kTutSellCarrots && selectedID == kCarrot) {
      makeTutorialProgress();
    }
  }
}

void populateInfoSell() {
  updateSales();
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(kUICatCargo, selectedID);
  const uint16_t selectedOwned = getOwned(kUICatCargo, selectedID);
  int32_t toSell = getBuySellMultiplier();
  if (toSell > selectedOwned) {
    toSell = selectedOwned;
  }  

  // AFFORD (N/A)
  pd->sprite->setVisible(getCannotAffordSprite(), 0);

  // INFO
  LCDBitmap* infoBitmap = getInfoBitmap();
  static char textA[128] = "";
  static char textB[128] = "";
  static char textC[128] = "";
  setRoobert10();
  pd->graphics->clearBitmap(infoBitmap, kColorClear);
  pd->graphics->pushContext(infoBitmap);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  snprintf(textA, 128, "Sell %s", toStringCargoByType(selectedID, /*plural=*/false));
  snprintf(textB, 128, "Inventory: %i", selectedOwned);
  char textM[32] = "";
  snprintf_c(textM, 32, selectedPrice * toSell);
  snprintf(textC, 128, "Value:      %s", textM);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, 9*TILE_PIX, TILE_PIX - 2);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(2, 16, 1), 11*TILE_PIX + TILE_PIX/2, TILE_PIX - 2, kBitmapUnflipped);
  pd->graphics->popContext();
}

bool populateContentSell(void) {
  struct Player_t* p = getPlayer();
  int16_t column = 0, row = 0;
  setUIContentHeader(row, kUICatCargo);
  bool empty = true;
  ++row;
  for (int32_t i = 0; i < getNSubTypes(kUICatCargo); ++i) {
    if (!getOwned(kUICatCargo, i)) {
      continue;
    }
    if (column == ROW_WDTH) { // Only start a new row if we are sure we have something to put in it
      ++row;
      column = 0;
    }
    setUIContentItem(row, column++, kUICatCargo, i, 0);
    empty = false;
  }
  return empty;
}