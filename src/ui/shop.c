#include "shop.h"
#include "../player.h"
#include "../sprite.h"
#include "../generate.h"

void doPurchace() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(selectedCat, selectedID);
  const uint8_t multiplier = getBuySellMultiplier();

  if (modMoney(-selectedPrice * multiplier)) {
    UIDirtyMain();
    for (int32_t dummy = 0; dummy < multiplier; ++dummy) {
      modOwned(selectedCat, selectedID, /*add=*/ true);
      // Tutorial
      if (getTutorialStage() == kTutWelcomeBuySeeds && selectedCat == kUICatPlant && selectedID == kCarrotPlant) {
        makeTutorialProgress();
      }
    }
  }
}

void populateContentBuy() {
  struct Player_t* p = getPlayer();
  const uint32_t unlockLevel = p->m_buildingsUnlockedTo;
  int16_t column = 0, row = 0;
  // Miss the first (Tools). Miss the last (Cargo)
  for (int32_t c = kUICatPlant; c < kUICatCargo; ++c) {
    int32_t myLevel = getUnlockLevel(c, 0);
    if (myLevel > unlockLevel) { // Can the first thing be bought? Should always be the 1st thing to be unlocked per cat
      continue;
    }
    setUIContentHeader(row, c);
    ++row;
    column = 0;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      myLevel = getUnlockLevel(c, i);
      if (myLevel > unlockLevel) {
        continue;
      }
      if (column == ROW_WDTH) { // Only start a new row if we are sure we have something to put in it
        ++row;
        column = 0;
      }
      setUIContentItem(row, column++, c, i, 0);
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
    case kPlant:; 
      if (selectedID == kSeaweedPlant || selectedID == kSeaCucumberPlant) {
        snprintf(textA, 128, "Buy %s (%s)",
          toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
          toStringSoil( PDesc[selectedID].soil )); 
      } else {
        snprintf(textA, 128, "Buy %s (%s %s)",
          toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
          toStringWetness( PDesc[selectedID].wetness ),
          toStringSoil( PDesc[selectedID].soil )); 
      }
      break;
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