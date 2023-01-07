#include "inventory.h"
#include "../player.h"
#include "../location.h"
#include "../render.h"
#include "../sprite.h"
#include "../input.h"
#include "../sound.h"
#include "../generate.h"
#include "../buildings/conveyor.h"
#include "../buildings/utility.h"

bool doConveyorUpgrade(struct Location_t* _loc);

bool doClearObstruction(struct Location_t* _loc);

bool isInRangeOfCarrotPlant(struct Location_t* _placeLocation);

bool doPickAtLocation(struct Location_t* _loc);

/// ///

void doInventoryClick() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();

  #ifdef DEMO
  switch (selectedCat) {
    case kUICatTool: switch(selectedID) {
      case kToolPickup:; return setGameMode(kPickMode);
      case kToolInspect:; return setGameMode(kInspectMode);
      case kToolDestroy:; return setGameMode(kDestroyMode);
      case kNToolTypes:; break;
    } break;
    case kUICatCargo: return setGameMode(kPlaceMode); 
    default: break;
  }
  sfx(kSfxNo);
  return;
  #endif

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
    case kUICatWarp: break;
    case kUICatImportN: case kUICatImportE: case kUICatImportS: case kUICatImportW: break; 
    case kNUICats: break;
  }
}

bool doConveyorUpgrade(struct Location_t* _loc) {
  int32_t min = 0, max = 0;
  switch (getRadius()) {
    case 1: min = 0; max = 1; break;
    case 3: min = -1; max = 2; break;
    case 5: min = -2; max = 3; break;
  }
  for (int32_t x = min; x < max; ++x) {
    for (int32_t y = min; y < max; ++y) {
      struct Location_t* l = getLocation(_loc->m_x + x, _loc->m_y + y);
      upgradeConveyor(l->m_building, /*forFree=*/ false);
    }
  }
  return false; // We have handled the removal of the grease(es) inside upgradeConveyor
}

bool doClearObstruction(struct Location_t* _loc) {
  int32_t min = 0, max = 0;
  switch (getRadius()) {
    case 1: min = 0; max = 1; break;
    case 3: min = -1; max = 2; break;
    case 5: min = -2; max = 3; break;
  }
  bool removed = false;
  for (int32_t x = min; x < max; ++x) {
    for (int32_t y = min; y < max; ++y) {
      struct Location_t* l = getLocation(_loc->m_x + x, _loc->m_y + y);
      removed |= tryRemoveObstruction(l);
    }
  }
  if (removed) sfx(kSfxClearObstruction);
  return false; // We have handled the removal of the obstruction removers inside tryRemoveObstruction
}

bool isInRangeOfCarrotPlant(struct Location_t* _placeLocation) {
  for (int32_t x = -3; x < 4; ++x) {
    for (int32_t y = -3; y < 4; ++y) {
      struct Location_t* loc = getLocation(_placeLocation->m_x + x, _placeLocation->m_y + y);
      if (loc->m_building && loc->m_building->m_type == kPlant && loc->m_building->m_subType.plant == kCarrotPlant) {
        return true;
      }
    }
  }
  return false;
}

void doPlace() {
  const enum kUICat selectedCat = getUIContentCategory();
  const uint16_t selectedID =  getUIContentID();
  if (getOwned(selectedCat, selectedID) == 0) {
    return;
  }
  struct Location_t* placeLocation = getPlayerLocation();
  // Fence are placed one tile away, dependent on rotation
  if (selectedCat == kUICatUtility && selectedID == kFence) {
    switch (getCursorRotation()) {
      case SN: placeLocation = getLocation(placeLocation->m_x, placeLocation->m_y - 1); break;
      case NS: placeLocation = getLocation(placeLocation->m_x, placeLocation->m_y + 1); break;
      case WE: placeLocation = getLocation(placeLocation->m_x + 1, placeLocation->m_y); break;
      case EW: placeLocation = getLocation(placeLocation->m_x - 1, placeLocation->m_y); break;
    }
  }

  bool placed = false;
  switch (selectedCat) {
    case kUICatTool: break; // Impossible
    case kUICatPlant: placed = newBuilding(placeLocation, SN, kPlant, (union kSubType) {.plant = selectedID} ); break;
    case kUICatConv:;
      // If we already have a conv here of the same subtype and rotation then there is nothing to do
      if (placeLocation->m_building
        && placeLocation->m_building->m_type == kConveyor
        && placeLocation->m_building->m_subType.conveyor == selectedID
        && placeLocation->m_building->m_dir == getCursorRotation())
      {
        break;
      } 
      bool carryForwardConveyorUpgrade = false;
      // We can place conveyors on top of other conveyors, to do so we first have to delete the existing one
      if (placeLocation->m_building && placeLocation->m_building->m_type == kConveyor) {
        // Refund it assuming that it is not a tunnel exit (these get placed "for free" when the entrance is placed)
        if (placeLocation->m_building->m_subType.conveyor != kTunnelOut) {
          modOwned(kUICatConv, placeLocation->m_building->m_subType.raw, /*add = */ true);
        }
        // Check if it was upgraded
        carryForwardConveyorUpgrade = (placeLocation->m_building->m_stored[0] == 2);
        // Demolish to make way for the new building
        clearLocation(placeLocation, /*cargo=*/ false, /*building=*/ true);
      }
      placed = newBuilding(placeLocation, getCursorRotation(), kConveyor, (union kSubType) {.conveyor = selectedID} );
      if (!placed) {
        break;
      }
      if (carryForwardConveyorUpgrade) { // Carry forward the upgrade without costing another application of grease
        upgradeConveyor(placeLocation->m_building, /*forFree=*/ true);
      } else if (getPlayer()->m_autoUseConveyorBooster) { // Test auto upgrade of conveyor belts, will only work with grease
        upgradeConveyor(placeLocation->m_building, /*forFree=*/ false);
      }
      // If we are placing a tunnel then we additionally need to place the exit location too... (could this be handled better?)
      if (placeLocation->m_building->m_subType.conveyor != kTunnelIn) {
        break;
      } 
      struct Location_t* placeLocationTwo = getTunnelOutLocation(placeLocation, getCursorRotation());
      carryForwardConveyorUpgrade = false;
      if (placeLocationTwo->m_building && placeLocationTwo->m_building->m_type == kConveyor) {
        // Refund it assuming that it is not a tunnel exit (these get placed "for free" when the entrance is placed)
        if (placeLocationTwo->m_building->m_subType.conveyor != kTunnelOut) {
          modOwned(kUICatConv, placeLocationTwo->m_building->m_subType.raw, /*add = */ true);
        }
        // Check if it was upgraded
        carryForwardConveyorUpgrade = (placeLocationTwo->m_building->m_stored[0] == 2);
        // Demolish to make way for the new building
        clearLocation(placeLocationTwo, /*cargo=*/ false, /*building=*/ true);
      }
      placed = newBuilding(placeLocationTwo, getCursorRotation(), kConveyor, (union kSubType) {.conveyor = kTunnelOut} );
      if (placed) {
        if (carryForwardConveyorUpgrade) { // Carry forward the upgrade without costing another application of grease
          upgradeConveyor(placeLocationTwo->m_building, /*forFree=*/ false);
        } else if (getPlayer()->m_autoUseConveyorBooster) { // Test auto upgrade of conveyor belts, will only work with grease
          upgradeConveyor(placeLocationTwo->m_building, /*forFree=*/ false);
        }
      }
      break;
    case kUICatExtractor: placed = newBuilding(placeLocation, getCursorRotation(), kExtractor, (union kSubType) {.extractor = selectedID} ); break;
    case kUICatFactory: placed = newBuilding(placeLocation, getCursorRotation(), kFactory, (union kSubType) {.factory = selectedID} ); break;
    case kUICatUtility: 
      if (selectedID == kConveyorGrease) placed = doConveyorUpgrade(placeLocation);
      else if (selectedID == kObstructionRemover) placed = doClearObstruction(placeLocation);
      else if (selectedID == kLandfill) placed = doPlaceLandfill(placeLocation);
      else if (selectedID == kRetirement) placed = doPlaceRetirement(placeLocation);
      else placed = newBuilding(placeLocation, getCursorRotation(), kUtility, (union kSubType) {.utility = selectedID} );
      break;
    case kUICatCargo: 
      if (placeLocation->m_building && placeLocation->m_building->m_type == kUtility && placeLocation->m_building->m_subType.utility == kSign) {
        if (placeLocation->m_building->m_mode.mode16 != selectedID) {
          placeLocation->m_building->m_mode.mode16 = selectedID;
          sfx(kSfxA);
          renderChunkBackgroundImage(placeLocation->m_chunk);
        }
      } else {
        placed = newCargo(placeLocation, selectedID, /*add to display*/ true); 
      }
      break;
    case kUICatWarp: break;
    case kUICatImportN: case kUICatImportE: case kUICatImportS: case kUICatImportW: break; 
    case kNUICats: break;
  }
  if (placed) {
    switch (selectedCat) {
      case kUICatPlant:  addTrauma(0.5f); sfx(kSfxPlacePlant); break;
      case kUICatConv:  addTrauma(0.4f); sfx(kSfxPlaceConv); break;
      case kUICatExtractor: addTrauma(1.0f); sfx(kSfxPlaceExtract); break;
      case kUICatFactory: addTrauma(1.0f); sfx(kSfxPlaceFac); break;
      case kUICatUtility: if (selectedID != kConveyorGrease) { addTrauma(0.5f); }  sfx(kSfxPlaceUtil); break;
      case kUICatCargo: sfx(kSfxPlaceCargo); break;
      default: break;
    }
    
    modOwned(selectedCat, selectedID, /*add=*/ false);
    const enum kUITutorialStage tut = getTutorialStage(); 
    // Tutorial
    if (tut <= kTutPlantCarrots && selectedCat == kUICatPlant && selectedID == kCarrotPlant) {
      makeTutorialProgress();
    }
    // Tutorial
    if (tut == kTutBuildHarvester && selectedCat == kUICatExtractor && selectedID == kCropHarvesterSmall && isInRangeOfCarrotPlant(placeLocation)) {
      makeTutorialProgress();
    }
    // Tutorial
    if (tut == kTutBuildQuarry && selectedCat == kUICatExtractor && selectedID == kChalkQuarry) {
      makeTutorialProgress();
    }
  } else {
    if (selectedCat != kUICatPlant && selectedCat != kUICatConv && selectedCat != kUICatCargo && selectedCat != kUICatUtility) {
      sfx(kSfxNo);
    }
  }
}

void doPick() {
  bool update = false;
  int32_t min = 0, max = 0;
  switch (getRadius()) {
    case 1: min = 0; max = 1; break;
    case 3: min = -1; max = 2; break;
    case 5: min = -2; max = 3; break;
  }
  struct Location_t* ploc = getPlayerLocation();
  for (int32_t x = min; x < max; ++x) {
    for (int32_t y = min; y < max; ++y) {
      struct Location_t* loc = getLocation(ploc->m_x + x, ploc->m_y + y);
      update |= doPickAtLocation(loc);
    }
  }
  if (update) {
    updateRenderList();
    sfx(kSfxPickCargo);
  }
}

bool doPickAtLocation(struct Location_t* _loc) {
  bool update = false;
  if (_loc->m_cargo) {
    modOwned(kUICatCargo, _loc->m_cargo->m_type, /*add=*/ true);
    // Tutorial
    if (getTutorialStage() == kTutGetCarrots && _loc->m_cargo->m_type == kCarrot) {
      makeTutorialProgress();
    }
    clearLocation(_loc, /*cargo=*/ true, /*building=*/ false);
    update = true;
  }
  if (_loc->m_building) {
    struct Building_t* building = _loc->m_building;
    struct Player_t* p = getPlayer();
    bool doBoxHarvest = false;
    if (building->m_type == kExtractor) {
      if (building->m_subType.extractor == kCropHarvesterSmall) doBoxHarvest = true;
      if (building->m_subType.extractor == kCropHarvesterLarge) doBoxHarvest = true;
    } else if (building->m_type == kUtility) {
      if (building->m_subType.utility == kBuffferBox || building->m_subType.utility == kStorageBox) doBoxHarvest = true;
    }
    if (doBoxHarvest) {
      for (int32_t compartment = 0; compartment < 3; ++compartment) {
        if (!building->m_stored[compartment]) continue;

        p->m_carryCargo[ building->m_stored[(MAX_STORE/2) + compartment] ] += building->m_stored[compartment];
        building->m_stored[compartment] = 0;
        building->m_stored[(MAX_STORE/2) + compartment] = kNoCargo;
        update = true;
      }
    }
    if (building->m_type == kFactory) {
      if (building->m_stored[0]) {
        p->m_carryCargo[ FDesc[building->m_subType.factory].out ] += building->m_stored[0];
        building->m_stored[0] = 0;
        update = true;
      }
      if (building->m_stored[1]) {
        p->m_carryCargo[ FDesc[building->m_subType.factory].in1 ] += building->m_stored[1];
        building->m_stored[1] = 0;
        update = true;
      }
      if (building->m_stored[2]) {
        p->m_carryCargo[ FDesc[building->m_subType.factory].in2 ] += building->m_stored[2];
        building->m_stored[2] = 0;
        update = true;
      }
      if (building->m_stored[3]) {
        p->m_carryCargo[ FDesc[building->m_subType.factory].in3 ] += building->m_stored[3];
        building->m_stored[3] = 0;
        update = true;
      }
      if (building->m_stored[4]) {
        p->m_carryCargo[ FDesc[building->m_subType.factory].in4 ] += building->m_stored[4];
        building->m_stored[4] = 0;
        update = true;
      }
      if (building->m_stored[5]) {
        p->m_carryCargo[ FDesc[building->m_subType.factory].in5 ] += building->m_stored[5];
        building->m_stored[5] = 0;
        update = true;
      }
    } else if (building->m_type == kConveyor) {
      m_recursionCount = MAX_RECURSION; // This prevents the call here from acting recursivly 
      updateConveyorDirection(building, 0, 0, 0);
    }
  }
  return update;
}


void doDestroy() {
  #ifdef DEMO
  sfx(kSfxNo);
  return;
  #endif
  bool update = false;
  int32_t min = 0, max = 0;
  switch (getRadius()) {
    case 1: min = 0; max = 1; break;
    case 3: min = -1; max = 2; break;
    case 5: min = -2; max = 3; break;
  }
  struct Location_t* ploc = getPlayerLocation();
  bool cleared = false;
  pauseMusic();
  for (int32_t x = min; x < max; ++x) {
    for (int32_t y = min; y < max; ++y) {
      struct Location_t* loc = getLocation(ploc->m_x + x, ploc->m_y + y);
      if (getPlayer()->m_enablePickupOnDestroy) {
        doPickAtLocation(loc);
      }
      bool doBuilding = true; 
      if (loc->m_building) {
        if (loc->m_building->m_type == kSpecial) doBuilding = false;
        else if (loc->m_building->m_type == kUtility && loc->m_building->m_subType.utility == kRetirement) doBuilding = false;
      }
      if (loc->m_building && doBuilding) {
        // Refund - but not conveyor exits
        if (!(loc->m_building->m_type == kConveyor && loc->m_building->m_subType.conveyor == kTunnelOut)) {
          modOwned(getBuildingTypeCat(loc->m_building->m_type), loc->m_building->m_subType.raw, /*add*/ true);
        }
      }
      cleared |= clearLocation(loc, /*cargo=*/ true, /*building=*/ doBuilding);
    }
  }
  resumeMusic();
  if (cleared) {
    sfx(kSfxDestroy);
    addTrauma(1.1f);
  }
}

void populateContentInventory(void) {
  struct Player_t* p = getPlayer();
  int16_t column = 0, row = 0;
  for (enum kUICat c = 0; c <= kUICatCargo; ++c) {
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
      if ((c >= kUICatConv && c < kUICatUtility) || (c == kUICatUtility && i == kBuffferBox)) rot = getCursorRotation();
      if (column == ROW_WDTH) { // Only start a new row if we are sure we have something to put in it
        ++row;
        column = 0;
      }
      setUIContentItem(row, column++, c, i, rot);
    }
    ++row;
  }
}

void populateInfoInventory() {
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
    case kUICatTool:  snprintf(textA, 128, "%s", 
      toStringTool(selectedID)); snprintf(textB, 128, "%s", 
      toStringToolInfo(selectedID)); break;
    case kUICatPlant:; 
      if (selectedID == kSeaweedPlant || selectedID == kSeaCucumberPlant) {
        snprintf(textA, 128, "Plant %s (%s)",
          toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
          toStringSoil(PDesc[selectedID].soil)); 
      } else {
        snprintf(textA, 128, "Plant %s (%s %s)",
          toStringBuilding(selectedCatType, (union kSubType) {.plant = selectedID}, false),
          toStringWetness(PDesc[selectedID].wetness),
          toStringSoil(PDesc[selectedID].soil)); 
      }
      break;
    case kUICatConv:; 
      snprintf(textA, 128, "Place %s", toStringBuilding(selectedCatType, (union kSubType) {.conveyor = selectedID}, false)); 
      break;
    case kUICatExtractor:; 
      snprintf(textA, 128, "Build %s", toStringBuilding(selectedCatType, (union kSubType) {.extractor = selectedID}, false));
      break;
    case kUICatFactory:; 
      snprintf(textA, 128, "Build %s", toStringBuilding(selectedCatType, (union kSubType) {.factory = selectedID}, false));
      break;
    case kUICatUtility:;
      snprintf(textA, 128, "Place %s", toStringBuilding(selectedCatType, (union kSubType) {.utility = selectedID}, false));
      break;
    case kUICatCargo:; 
      snprintf(textA, 128, "Place %s", toStringCargoByType(selectedID, /*plural=*/false));
      break;
    case kUICatWarp:; break;
    case kUICatImportN: case kUICatImportE: case kUICatImportS: case kUICatImportW: break; 
    case kNUICats:; break;
  }
  if (selectedCat != kUICatTool) snprintf(textB, 128, "Inventory: %i", selectedOwned);
  snprintf(textC, 2, " ");
  int32_t cOff = 0;
  if (selectedCat == kUICatCargo) {
    char textM[32] = "";
    snprintf_c(textM, 32, selectedPrice);
    snprintf(textC, 128, "Value:      %s", textM);
  } else if (selectedCat == kUICatExtractor) {
    if (selectedID == kChalkQuarry) snprintf(textC, 128, "Build on %s", toStringSoil(kChalkyGround));
    else if (selectedID == kPump) snprintf(textC, 128, "Build on Water");
    else if (selectedID == kSaltMine) snprintf(textC, 128, "Build on %s", toStringSoil(kPeatyGround));
  } else if (selectedCat == kUICatPlant) {
    if (selectedID == kSeaweedPlant) snprintf(textC, 128, "Sow on Water");
    else if (selectedID == kSeaCucumberPlant) snprintf(textC, 128, "Place in the Ocean");
  } else if (selectedCat == kUICatUtility) {
    if (selectedID == kLandfill) snprintf(textC, 128, "Cannot be removed");
    if (selectedID == kRetirement) { snprintf(textC, 128, "Build on Tranquil Plains"); cOff = TILE_PIX; }
  }
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, 9*TILE_PIX - cOff, TILE_PIX - 2);
  pd->graphics->setDrawMode(kDrawModeCopy); // Draw coin
  if (selectedCat == kUICatCargo) pd->graphics->drawBitmap(getSprite16(2, 16, 1), 11*TILE_PIX + TILE_PIX/2, TILE_PIX - 2, kBitmapUnflipped);
  pd->graphics->popContext();
}
