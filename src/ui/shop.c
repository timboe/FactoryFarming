#include "shop.h"
#include "../player.h"
#include "../sprite.h"
#include "../generate.h"
#include "../sound.h"

void doPurchace() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(selectedCat, selectedID);
  const uint8_t multiplier = getBuySellMultiplier();

  if (modMoney(-selectedPrice * multiplier)) {
    UIDirtyMain();
    sfx(kSfxBuy);
    for (int32_t dummy = 0; dummy < multiplier; ++dummy) {
      modOwned(selectedCat, selectedID, /*add=*/ true);
      // Tutorial
      if (getTutorialStage() == kTutWelcomeBuySeeds && selectedCat == kUICatPlant && selectedID == kCarrotPlant) {
        makeTutorialProgress();
      }
    }
  } else {
    sfx(kSfxNo);
  }
}

void populateContentBuy() {
  struct Player_t* p = getPlayer();
  const uint32_t myUnlockLevel = p->m_buildingsUnlockedTo;
  int16_t column = 0, row = 0;
  // Miss the first (Tools). Miss the last (Cargo)
  for (int32_t c = kUICatPlant; c < kUICatCargo; ++c) {
    int32_t firstItemUnlockLevel = getUnlockLevel(c, 0);
    if (myUnlockLevel < firstItemUnlockLevel) { // Can the first thing be bought? Should always be the 1st thing to be unlocked per cat
      continue;
    }
    setUIContentHeader(row, c);
    ++row;
    column = 0;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      int32_t itemUnlockLevel = getUnlockLevel(c, i);
      if (myUnlockLevel < itemUnlockLevel) {
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
  const uint8_t multiplier = getBuySellMultiplier();

  // AFFORD
  const bool canAfford = (p->m_money >= selectedPrice * multiplier || p->m_infiniteMoney);
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
        strcpy(textA, tr(kTRShopBuy));
        strcat(textA, space());
        strcat(textA, toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false));
        strcat(textA, space());
        strcat(textA, lb());
        strcat(textA, toStringSoil( PDesc[selectedID].soil ));
        strcat(textA, rb());
      } else {
        strcpy(textA, tr(kTRShopBuy));
        strcat(textA, space());
        strcat(textA, toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false));
        strcat(textA, space());
        strcat(textA, lb());
        strcat(textA, toStringWetness( PDesc[selectedID].wetness ));
        strcat(textA, space());
        strcat(textA, toStringSoil( PDesc[selectedID].soil ));
        strcat(textA, rb());
      }
      break;
    case kConveyor:;
      strcpy(textA, tr(kTRShopBuy));
      strcat(textA, space());
      strcat(textA, toStringBuilding(selectedCatType, (union kSubType) {.conveyor = selectedID}, false));
      break;
    case kExtractor:; 
      strcpy(textA, tr(kTRShopBuy));
      strcat(textA, space());
      strcat(textA, toStringBuilding(selectedCatType, (union kSubType) {.extractor = selectedID}, false));
      break;
    case kFactory:; 
      strcpy(textA, tr(kTRShopBuy));
      strcat(textA, space());
      strcat(textA, toStringBuilding(selectedCatType, (union kSubType) {.factory = selectedID}, false));
      break;
    case kUtility:; 
      strcpy(textA, tr(kTRShopBuy));
      strcat(textA, space());
      strcat(textA, toStringBuilding(selectedCatType, (union kSubType) {.utility = selectedID}, false));
      break;
    case kSpecial:; case kNoBuilding:; case kNBuildingTypes:; break;
  }

  char textM[32] = "";
  snprintf_c(textM, 32, selectedPrice * multiplier);

  snprintf(textB, 128, tr(kTRInventory), selectedOwned);

  strcpy(textC, tr(kTRShopPrice));
  strcat(textC, c5space());
  strcat(textC, textM);

  pd->graphics->drawText(textA, 128, kUTF8Encoding, TILE_PIX/2, +2 +tY());
  pd->graphics->drawText(textB, 128, kUTF8Encoding, TILE_PIX/2, TILE_PIX - 2 +tY());
  pd->graphics->drawText(textC, 128, kUTF8Encoding, 9*TILE_PIX, TILE_PIX - 2 +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(2, 16, 1), 9*TILE_PIX + trLen(kTRShopPrice), TILE_PIX - 2, kBitmapUnflipped); // Coin
  pd->graphics->popContext();
} 