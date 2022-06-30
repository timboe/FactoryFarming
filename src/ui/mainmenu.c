#include "mainmenu.h"
#include "../player.h"
#include "../location.h"
#include "../render.h"
#include "../sprite.h"
#include "../generate.h"

void doMainMenuClick() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  switch (selectedCat) {
    case kUICatTool: switch(selectedID) {
      case kToolPickup:; return setGameMode(kPickMode);
      case kToolInspect:; return setGameMode(kInspectMode);
      case kToolDestroy:; return setGameMode(kDestroyMode);
      case kNToolTypes:; break;
    } break;
    case kUICatPlant: return setGameMode(kPlantMode);
    case kUICatConv: return setGameMode(kPlaceMode); 
    case kUICatExtractor: return setGameMode(kBuildMode);
    case kUICatFactory: return setGameMode(kBuildMode);
    case kUICatUtility: return setGameMode(kPlaceMode);
    case kUICatCargo: return setGameMode(kPlaceMode); 
    case kNUICats: break;
  }
}

void doPlace() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  if (getOwned(selectedCat, selectedID) == 0) {
    return;
  }
  bool placed = false;
  switch (selectedCat) {
  	case kUICatTool: break; // Impossible
    case kUICatPlant: placed = newBuilding(getPlayerLookingAtLocation(), SN, kPlant, (union kSubType) {.plant = selectedID} ); break;
    case kUICatConv: placed = newBuilding(getPlayerLookingAtLocation(), getCursorRotation(), kConveyor, (union kSubType) {.conveyor = selectedID} ); break;
    case kUICatExtractor: placed = newBuilding(getPlayerLookingAtLocation(), getCursorRotation(), kExtractor, (union kSubType) {.extractor = selectedID} ); break;
    case kUICatFactory: placed = newBuilding(getPlayerLookingAtLocation(), getCursorRotation(), kFactory, (union kSubType) {.factory = selectedID} ); break;
    case kUICatUtility: placed = newBuilding(getPlayerLookingAtLocation(), getCursorRotation(), kUtility, (union kSubType) {.utility = selectedID} ); break;
    case kUICatCargo: placed = newCargo(getPlayerLookingAtLocation(), selectedID, true); break;
    case kNUICats: break;
  }
  if (placed) {
    modOwned(selectedCat, selectedID, /*add=*/ false);
  }
}

// TODO pick up from buildings, crates, etc.
void doPick() {
  bool update = false;
  struct Location_t* ploc = getPlayerLocation();
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Location_t* loc = getLocation(ploc->m_x + x, ploc->m_y + y);
      if (loc->m_cargo) {
        modOwned(kUICatCargo, loc->m_cargo->m_type, /*add=*/ true);
        clearLocation(loc, /*cargo=*/ true, /*building=*/ false);
        update = true;
      }
    }
  }
  if (update) updateRenderList();
}


void doDestroy() {
  bool update = false;
  struct Location_t* ploc = getPlayerLocation();
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      struct Location_t* loc = getLocation(ploc->m_x + x, ploc->m_y + y);
      clearLocation(loc, /*cargo=*/ true, /*building=*/ true);
    }
  }
}

void populateContentMainmenu(void) {
  struct Player_t* p = getPlayer();
  int16_t column = 0, row = 0;
  for (enum kUICat c = 0; c < kNUICats; ++c) {
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
    setUIContentHeader(row, c);
    ++row;
    column = 0;
    for (int32_t i = 0; i < getNSubTypes(c); ++i) {
      if (!getOwned(c, i)) {
        continue;
      }
      int16_t rot = 0;
      if (c >= kUICatConv && c <= kUICatUtility) rot = getCursorRotation();
      setUIContentItem(row, column, c, i, rot);
      if (++column == ROW_WDTH) {
        ++row;
        column = 0;
      }
    }
    ++row;
  }
}

void populateInfoMainmenu() {
  const enum kUICat selectedCat = getUIContentCategory();
  const enum kBuildingType selectedCatType = getCatBuildingSubType(selectedCat);
  const uint16_t selectedID =  getUIContentID();
  const int32_t selectedPrice = getPrice(selectedCat, selectedID);
  const uint16_t selectedOwned = getOwned(selectedCat, selectedID);

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
  switch (selectedCat) {
    case kUICatTool:  snprintf(textA, 128, "%s", toStringTool(selectedID)); snprintf(textB, 128, "%s", toStringToolInfo(selectedID)); break;
    case kUICatPlant:; snprintf(textA, 128, "Plant %s, Likes: %s %s",
      toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
      toStringWetness(kPlantWetness[selectedID]),
      toStringSoil(kPlantSoil[selectedID])); break;
    case kUICatConv:; snprintf(textA, 128, "Place %s (%s)", toStringBuilding(selectedCatType, (union kSubType) {.conveyor = selectedID}, false), getRotationAsString()); break;
    case kUICatExtractor:; snprintf(textA, 128, "Build %s", toStringBuilding(selectedCatType, (union kSubType) {.extractor = selectedID}, false)); break;
    case kUICatFactory:; snprintf(textA, 128, "Build %s", toStringBuilding(selectedCatType, (union kSubType) {.factory = selectedID}, false)); break;
    case kUICatUtility:; snprintf(textA, 128, "Place %s", toStringBuilding(selectedCatType, (union kSubType) {.utility = selectedID}, false)); break;
    case kUICatCargo:; snprintf(textA, 128, "Place %s", toStringCargoByType(selectedID)); break;
    case kNUICats:; break;
  }
  if (selectedCat != kUICatTool) snprintf(textB, 128, "Inventory: %i", selectedOwned);
  if (selectedCat == kUICatCargo) snprintf(textC, 128, "Value:      %i", (int)selectedPrice);
  else snprintf(textC, 128, " ");
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, 9*TILE_PIX, TILE_PIX - 2);
  pd->graphics->setDrawMode(kDrawModeCopy);
  if (selectedCat == 6) pd->graphics->drawBitmap(getSprite16(2, 2, 1), 11*TILE_PIX + TILE_PIX/2, TILE_PIX - 2, kBitmapUnflipped);
  pd->graphics->popContext();
}
