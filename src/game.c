#include <math.h>

#include "game.h"
#include "sprite.h"
#include "generate.h"
#include "render.h"
#include "chunk.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "ui.h"
#include "io.h"
#include "building.h"
#include "buildings/special.h"
#include "buildings/conveyor.h"
#include "ui/settings.h"

PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;

uint16_t m_nearTickCount = 0;

uint16_t m_farTickCount = 0;

uint16_t m_autoSaveTimer = 0;

uint16_t m_cactusUnlock = 0; // Used to unlock 2nd half of the game

int32_t m_nUnlocks = 0;

uint8_t m_tickID;

void tickNear(void);

void tickFar(void);

////////////

void setPDPtr(PlaydateAPI* _p) {
  pd = _p;
}

uint16_t getNearTickCount() {
  return m_nearTickCount;
}

uint16_t getFarTickCount() {
  return m_farTickCount;
}

int getFrameCount() { 
  return m_frameCount; 
}

uint16_t locToPix(uint16_t _loc) {
  return TILE_PIX*_loc + TILE_PIX/2.0;
}

uint16_t pixToLoc(uint16_t _pix) {
  return (_pix - TILE_PIX/2) / TILE_PIX;
}

enum kCurrentHoliday getCurrentHoliday() {
  struct PDDateTime current;
  pd->system->convertEpochToDateTime(pd->system->getSecondsSinceEpoch(NULL), &current);
  if (current.month == 12) {
    return kChristmas;
  } else if ((current.month == 3 && current.day >= 22) || (current.month == 4 && current.day <= 24)) {
    return kEaster;
  }
  return kNoHoliday;
}

void tickNear() {
  #ifdef ONLY_SLOW_TICKS
  return;
  #endif

  if (m_frameCount % NEAR_TICK_FREQUENCY) {
    return;
  }

  ++m_tickID;
  m_nearTickCount = 0;

  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (zoom > 1 && getPlayer()->m_enableConveyorAnimation) animateConveyor();

  #ifdef ONLY_FAST_TICKS
  m_nearTickCount += chunkTickChunk(currentChunk, NEAR_TICK_AMOUNT, m_tickID, zoom);
  for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
    m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsALL[i], NEAR_TICK_AMOUNT, m_tickID, zoom);
    currentChunk->m_neighborsALL[i]->m_isNearTick = true;
  }
  for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_ALL; ++i) { 
    m_nearTickCount += chunkTickChunk(currentChunk->m_nonNeighborsALL[i], NEAR_TICK_AMOUNT, m_tickID, zoom); 
    currentChunk->m_nonNeighborsALL[i]->m_isNearTick = true;
  }
  return;
  #endif

  m_nearTickCount += chunkTickChunk(currentChunk, NEAR_TICK_AMOUNT, m_tickID, zoom);

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {

    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsALL[i], NEAR_TICK_AMOUNT, m_tickID, zoom);
      currentChunk->m_neighborsALL[i]->m_isNearTick = true;
    }

  } else {

    switch (getCurrentQuadrant()) {
      case NE:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNE[0], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNE[1], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNE[2], NEAR_TICK_AMOUNT, m_tickID, zoom);
        currentChunk->m_neighborsNE[0]->m_isNearTick = true;
        currentChunk->m_neighborsNE[1]->m_isNearTick = true;
        currentChunk->m_neighborsNE[2]->m_isNearTick = true;
        break;
      case SE:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSE[0], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSE[1], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSE[2], NEAR_TICK_AMOUNT, m_tickID, zoom);
        currentChunk->m_neighborsSE[0]->m_isNearTick = true;
        currentChunk->m_neighborsSE[1]->m_isNearTick = true;
        currentChunk->m_neighborsSE[2]->m_isNearTick = true;
        break;
      case SW:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSW[0], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSW[1], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSW[2], NEAR_TICK_AMOUNT, m_tickID, zoom);
        currentChunk->m_neighborsSW[0]->m_isNearTick = true;
        currentChunk->m_neighborsSW[1]->m_isNearTick = true;
        currentChunk->m_neighborsSW[2]->m_isNearTick = true;
        break;
      case NW:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNW[0], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNW[1], NEAR_TICK_AMOUNT, m_tickID, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNW[2], NEAR_TICK_AMOUNT, m_tickID, zoom);
        currentChunk->m_neighborsNW[0]->m_isNearTick = true;
        currentChunk->m_neighborsNW[1]->m_isNearTick = true;
        currentChunk->m_neighborsNW[2]->m_isNearTick = true;
        break;
    }

  }
}

void tickFar() {
  #ifdef ONLY_FAST_TICKS
  return;
  #endif

  if((m_frameCount + TICK_OFFSET_LARGETICK) % FAR_TICK_FREQUENCY) { 
    return;
  }

  ++m_tickID;
  m_farTickCount = 0;

  //static uint32_t tickTock = 0;
  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  #ifdef ONLY_SLOW_TICKS
  m_farTickCount += chunkTickChunk(currentChunk, FAR_TICK_AMOUNT, m_tickID, zoom);
  for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
    m_farTickCount += chunkTickChunk(currentChunk->m_neighborsALL[i], FAR_TICK_AMOUNT, m_tickID, zoom);
    currentChunk->m_neighborsALL[i]->m_isNearTick = false;
  }
  for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_ALL; ++i) { 
    m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsALL[i], FAR_TICK_AMOUNT, m_tickID, zoom); 
    currentChunk->m_nonNeighborsALL[i]->m_isNearTick = false;
  }
  return;
  #endif

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {

    for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_ALL; ++i) { 
      //pd->system->logToConsole("B %u = %i", i, (int) currentChunk->m_nonNeighborsALL[i]);
      m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsALL[i], FAR_TICK_AMOUNT, m_tickID, zoom);
      currentChunk->m_nonNeighborsALL[i]->m_isNearTick = false;
    }

  } else {
    
    switch (getCurrentQuadrant()) {
      case NE:;
        for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_CORNER; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNE[i], FAR_TICK_AMOUNT, m_tickID, zoom);
          currentChunk->m_nonNeighborsNE[i]->m_isNearTick = false;
        }
        break;
      case SE:;
        for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_CORNER; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSE[i], FAR_TICK_AMOUNT, m_tickID, zoom);
          currentChunk->m_nonNeighborsSE[i]->m_isNearTick = false;
        }
        break;
      case SW:;
        for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_CORNER; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSW[i], FAR_TICK_AMOUNT, m_tickID, zoom);
          currentChunk->m_nonNeighborsSW[i]->m_isNearTick = false;
        }
        break;
      case NW:;
        for (uint32_t i = 0; i < CHUNK_NONNEIGHBORS_CORNER; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNW[i], FAR_TICK_AMOUNT, m_tickID, zoom);
          currentChunk->m_nonNeighborsNW[i]->m_isNearTick = false;
        }
        break;
    }

  }

  #ifdef PRINT_MAX_RECURSION
  printRecursionSummary();
  #endif
} 

int gameLoop(void* _data) {
  ++m_frameCount;
  pd->graphics->setBackgroundColor(kColorBlack);

  if (IOOperationInProgress()) { 
    pd->sprite->removeAllSprites();
    enactIO();
    pd->sprite->drawSprites();
    m_autoSaveTimer = 0;
    return 1;
  }

  const enum kGameMode gm = getGameMode();

  clickHandlerReplacement();

  if (gm < kMenuBuy) {
    movePlayer(/*forceUpdate*/ false);
  }

  tickNear();
  tickFar();

  if((m_frameCount + TICK_OFFSET_SPRITELIST) % SCREENUPDATE_TICK_FREQUENCY == 0) {
    updateRenderList();
  }

  if (gm == kTitles) updateUITitles(m_frameCount);
  else updateUI(m_frameCount);

  render();

  struct Player_t* p = getPlayer(); 
  ++p->m_playTime;

  #ifndef DEMO
  if (p->m_enableAutosave) {
    if (++m_autoSaveTimer > p->m_enableAutosave*60*TICK_FREQUENCY && gm == kWanderMode ) {
      m_autoSaveTimer = 0;
      #ifdef SSHOT_ON_AUTOSAVE
      doIO(kDoSave, /*and then*/ kDoScreenShot, /*and finally*/ kDoNothing);
      #else
      doIO(kDoSave, /*and then*/ kDoNothing, /*and finally*/ kDoNothing);
      #endif
    }
  }
  #endif

  return 1;
}

void menuOptionsCallbackDelete(void* _save) {
  if (IOOperationInProgress()) { return; }
  int save = (uintptr_t) _save;
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackDelete %i", save);
  #endif
  setSave(save);
  doIO(kDoSaveDelete, /*and then*/ kDoScanSlots, /*and finally*/ kDoNothing);
}

void menuOptionsCallbackLoad(void* blank) {
  if (IOOperationInProgress()) { return; }
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackLoad");
  #endif
  setForceSlot(-1); // -1 loads from the slot stored in the player's save file
  doIO(kDoLoad, /*and then*/ kDoNothing, /*and finally*/ kDoNothing);
  m_autoSaveTimer = 0;
}

void menuOptionsCallbackSave(void* blank) {
  if (IOOperationInProgress()) { return; }
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackSave");
  #endif
  doIO(kDoSave, /*and then*/ kDoNothing, /*and finally*/ kDoNothing);
  m_autoSaveTimer = 0;
}

void menuOptionsCallbackMenu(void* blank) {
  if (IOOperationInProgress()) { return; }
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackMenu");
  #endif
  checkReturnDismissTutorialMsg();
  drawUITop(tr(kTRMainMenu), kDrawModeCopy);
  redrawAllSettingsMenuLines();
  setGameMode(kMenuSettings);
}

// Call prior to loading anything
void reset(bool _resetThePlayer) {
  resetCargo();
  resetBuilding();
  resetExportAndSales(); // Essentially "reset special"
  resetLocation();
  resetChunk();
  resetWorld();
  if (_resetThePlayer) {
    resetPlayer();
    resetUI();
  }  
}

void populateMenuTitle() {
  pd->system->removeAllMenuItems();
  #ifdef DEMO
  return;
  #endif
  pd->system->addMenuItem(tr(kTRDeleteSave1), menuOptionsCallbackDelete, (void*)0);
  pd->system->addMenuItem(tr(kTRDeleteSave2), menuOptionsCallbackDelete, (void*)1);
  pd->system->addMenuItem(tr(kTRDeleteSave3), menuOptionsCallbackDelete, (void*)2);
}

void populateMenuGame() {
  pd->system->removeAllMenuItems();
  #ifndef DEMO
  pd->system->addMenuItem(tr(kTRLoad), menuOptionsCallbackLoad, NULL);
  pd->system->addMenuItem(tr(kTRSave), menuOptionsCallbackSave, NULL);
  #endif
  pd->system->addMenuItem(tr(kTRMenu), menuOptionsCallbackMenu, NULL);
}

uint16_t getCactusUnlock() {
  return m_cactusUnlock;
}

void initGame() {

  // Make seasonal adjustments
  const enum kCurrentHoliday currentHoliday = getCurrentHoliday();
  if (currentHoliday == kEaster) {
    CargoDesc[kChocolate].UIIcon = SID(9,25); // Switch chocolate bar for egg
  } else if (currentHoliday == kChristmas) {
    CargoDesc[kPartyPack].UIIcon = SID(10,25); // Switch party pack for present
  }

  // Populate unlock ordering
  int32_t i = -1;
  while (true) {
    ++i;
    if (UnlockDecs[i].type == kConveyor) {
      for (int32_t j = 0; j < kNConvSubTypes; ++j) {
        if (CDesc[j].subType == UnlockDecs[i].subType.conveyor) {
          CDesc[j].unlock = i;
          #ifdef DEV
          pd->system->logToConsole("%s gets unlock ID %i", toStringBuilding(kConveyor, (union kSubType) {.conveyor = j}, false), i);
          #endif
          break;
        }
      }
    } else if (UnlockDecs[i].type == kPlant) {
      if (UnlockDecs[i].subType.plant == kCactusPlant) m_cactusUnlock = i;
      for (int32_t j = 0; j < kNPlantSubTypes; ++j) {
        if (PDesc[j].subType == UnlockDecs[i].subType.plant) {
          PDesc[j].unlock = i;
          #ifdef DEV
          pd->system->logToConsole("%s gets unlock ID %i", toStringBuilding(kPlant, (union kSubType) {.plant = j}, false), i);
          #endif
          break;
        }
      }
    } else if (UnlockDecs[i].type == kExtractor) {
      for (int32_t j = 0; j < kNExtractorSubTypes; ++j) {
        if (EDesc[j].subType == UnlockDecs[i].subType.extractor) {
          EDesc[j].unlock = i;
          #ifdef DEV
          pd->system->logToConsole("%s gets unlock ID %i", toStringBuilding(kExtractor, (union kSubType) {.extractor = j}, false), i);
          #endif
          break;
        }
      }
    } else if (UnlockDecs[i].type == kFactory) {
      for (int32_t j = 0; j < kNFactorySubTypes; ++j) {
        if (FDesc[j].subType == UnlockDecs[i].subType.factory) {
          FDesc[j].unlock = i;
          #ifdef DEV
          pd->system->logToConsole("%s gets unlock ID %i", toStringBuilding(kFactory, (union kSubType) {.factory = j}, false), i);
          #endif
          break;
        }
      }
    } else if (UnlockDecs[i].type == kUtility) {
      for (int32_t j = 0; j < kNUtilitySubTypes; ++j) {
        if (UDesc[j].subType == UnlockDecs[i].subType.utility) {
          UDesc[j].unlock = i;
          #ifdef DEV
          pd->system->logToConsole("%s gets unlock ID %i", toStringBuilding(kUtility, (union kSubType) {.utility = j}, false), i);
          #endif
          break;
        }
      }
    }
    if (UnlockDecs[i].type == FINAL_UNLOCK_TYPE && UnlockDecs[i].subType.raw == FINAL_UNLOCK_SUBTYPE) {
      m_nUnlocks = i;
      break;
    }
  }

  setPrices();

  // Perform runtime sanity check.
  for (int32_t i = 0; i < kNFactorySubTypes; ++i) {
    if (i != FDesc[i].subType) pd->system->error("FACTORY DESCRIPTOR ORDERING IS WRONG!");
  }
  for (int32_t i = 0; i < kNUtilitySubTypes; ++i) {
    if (i != UDesc[i].subType) pd->system->error("UTILITY DESCRIPTOR ORDERING IS WRONG!");
  }
  for (int32_t i = 0; i < kNConvSubTypes; ++i) {
    if (i != CDesc[i].subType) pd->system->error("CONVEYOR DESCRIPTOR ORDERING IS WRONG!");
  }
  for (int32_t i = 0; i < kNPlantSubTypes; ++i) {
    if (i != PDesc[i].subType) pd->system->error("PLANT DESCRIPTOR ORDERING IS WRONG!");
  }
  for (int32_t i = 0; i < kNExtractorSubTypes; ++i) {
    if (i != EDesc[i].subType) pd->system->error("EXTRACTOR DESCRIPTOR ORDERING IS WRONG!");
  }
  for (int32_t i = 0; i < kNCargoType; ++i) {
    if (i != CargoDesc[i].subType) pd->system->error("CARGO DESCRIPTOR ORDERING IS WRONG!");
  }
  for (int32_t i = 0; i < kNWorldTypes; ++i) {
    if (i != WDesc[i].subType) pd->system->error("WORLD DESCRIPTOR ORDERING IS WRONG!");
  }

  if (CREDITS_TO_ROW > MAX_ROWS) {
    pd->system->error("INSUFFICIENT ROWS FOR CREDITS!");
  }
  
  if (PLAYER_ANIM_FRAMES != N_FOOTSTEPS) {
    pd->system->error("PLAYER_ANIM_FRAMES != ");
  }

  // Check unlocks are all in the expected order
  #define DIS UINT16_MAX
  uint8_t skip = 1;
  for (int32_t i = 1; i < kNConvSubTypes; ++i) {
    if (CDesc[i].unlock == DIS) { ++skip; continue; } // Skip placeholders
    if (CDesc[i-skip].unlock > CDesc[i].unlock) pd->system->error("CONVEYOR UNLOCK ORDERING IS INCONSISTENT!");
    skip = 1;
  }
  skip = 1;
  for (int32_t i = 1; i < kNExtractorSubTypes; ++i) {
    if (EDesc[i].unlock == DIS) { ++skip; continue; } // Skip placeholders
    if (EDesc[i-skip].unlock > EDesc[i].unlock) pd->system->error("EXTRACTOR UNLOCK ORDERING IS INCONSISTENT!");
    skip = 1;
  }
  skip = 1;
  for (int32_t i = 1; i < kNFactorySubTypes; ++i) {
    if (FDesc[i].unlock == DIS) { ++skip; continue; } // Skip placeholders
    if (FDesc[i-skip].unlock > FDesc[i].unlock) pd->system->error("FACTORY UNLOCK ORDERING IS INCONSISTENT!");
    skip = 1;
  }
  skip = 1;
  for (int32_t i = 1; i < kNPlantSubTypes; ++i) {
    if (PDesc[i].unlock == DIS) { ++skip; continue; } // Skip placeholders
    if (PDesc[i-skip].unlock > PDesc[i].unlock) pd->system->error("PLANT UNLOCK ORDERING IS INCONSISTENT!");
    skip = 1;
  }
  skip = 1;
  for (int32_t i = 1; i < kNUtilitySubTypes; ++i) {
    if (UDesc[i].unlock == DIS) { ++skip; continue; } // Skip placeholders
    if (UDesc[i-skip].unlock > UDesc[i].unlock) pd->system->error("UTILITY UNLOCK ORDERING IS INCONSISTENT!");
    skip = 1;
  }
}

int32_t getNUnlocks() {
  return m_nUnlocks;
}

char* ftos(float _value, int16_t _size, char* _dest) {
  char* tmpSign = (_value < 0) ? "-" : "";
  float tmpVal = (_value < 0) ? -_value : _value;

  int16_t tmpInt1 = tmpVal;
  float tmpFrac = tmpVal - tmpInt1;
  int16_t tmpInt2 = trunc(tmpFrac * 10000);

  char truncStr[8];
  snprintf (_dest, _size, "%02d", tmpInt2);
  snprintf (truncStr, 8, "%.2s", _dest);

  snprintf (_dest, _size, "%s%d.%s", tmpSign, tmpInt1, truncStr);
  return _dest;
}

const char* space(void) { return " "; }

const char* Cspace(void) { return ", "; }

const char* cspace(void) { 
  if (getLanguage() == kFR) return " : ";
  return ": ";
}

const char* c5space(void) { 
  if (getLanguage() == kFR) return " :     ";
  return ":      ";
}

const char* lb(void) { return "("; }

const char* rb(void) { return ")"; }
