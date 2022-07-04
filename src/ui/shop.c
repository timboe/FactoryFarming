#include "shop.h"
#include "../player.h"
#include "../sprite.h"
#include "../generate.h"

void doPurchace() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(selectedCat, selectedID);
  if (modMoney(-selectedPrice)) {
    modOwned(selectedCat, selectedID, /*add=*/ true);
    UIDirtyMain();
    // Tutorial
    if (getTutorialStage() == kTutWelcomeBuySeeds && selectedCat == kUICatPlant && selectedID == kCarrotPlant) {
      makeTutorialProgress();
    }
  }
}

void populateContentBuy() {
  struct Player_t* p = getPlayer();
  const uint32_t hwm = p->m_moneyHighWaterMark;
  int16_t column = 0, row = 0;
  // Miss the first (Tools). Miss the last (Cargo)
  for (int32_t c = kUICatPlant; c < kUICatCargo; ++c) {
    int32_t firstEntryCost = getUnlockCost(c, 0);
    if (hwm < firstEntryCost) { // If can unlock 1st entry
      continue;
    }
    setUIContentHeader(row, c);
    ++row;
    column = 0;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      if (hwm < getUnlockCost(c, i)) {
        continue;
      }
      setUIContentItem(row, column, c, i, 0);
      if (++column == ROW_WDTH) {
        ++row;
        column = 0;
      }
    }
    ++row;
  }
}

void populateInfoBuy(bool _visible) {
  struct Player_t* p = getPlayer();
  const enum kUICat selectedCat = getUIContentCategory();
  const enum kBuildingType selectedCatType = getCatBuildingSubType(selectedCat);
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(selectedCat, selectedID);
  const uint16_t selectedOwned = getOwned(selectedCat, selectedID);

  // AFFORD
  bool canAfford = (p->m_money >= selectedPrice);
  pd->sprite->setVisible(getCannotAffordSprite(), !canAfford && _visible);

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
  pd->graphics->drawBitmap(getSprite16(2, 16, 1), 11*TILE_PIX + TILE_PIX/4, TILE_PIX - 2, kBitmapUnflipped);
  pd->graphics->popContext();
} 