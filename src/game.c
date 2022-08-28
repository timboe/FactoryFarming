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
#include "ui/settings.h"

PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;

uint16_t m_nearTickCount = 0;

uint16_t m_farTickCount = 0;

uint16_t m_autoSaveTimer = 0;

uint16_t m_cactusUnlock = 0; // Used to unlock 2nd half of the game

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



void tickNear() {
  if (m_frameCount % NEAR_TICK_FREQUENCY) {
    return;
  }

  m_nearTickCount = 0;

  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (zoom > 1) animateConveyor();

  m_nearTickCount += chunkTickChunk(currentChunk, NEAR_TICK_AMOUNT, zoom);

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsALL[i], NEAR_TICK_AMOUNT, zoom);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNE[0], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNE[1], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNE[2], NEAR_TICK_AMOUNT, zoom);
        break;
      case SE:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSE[0], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSE[1], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSE[2], NEAR_TICK_AMOUNT, zoom);
        break;
      case SW:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSW[0], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSW[1], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsSW[2], NEAR_TICK_AMOUNT, zoom);
        break;
      case NW:;
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNW[0], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNW[1], NEAR_TICK_AMOUNT, zoom);
        m_nearTickCount += chunkTickChunk(currentChunk->m_neighborsNW[2], NEAR_TICK_AMOUNT, zoom);
        break;
    }
  }
}

void tickFar() {
  if((m_frameCount + 3) % FAR_TICK_FREQUENCY == 0) { // +3 to put this out-of-sync with tickNear() and tickFar()
    updateRenderList();
  }

  if((m_frameCount + 1) % FAR_TICK_FREQUENCY) { // +1 to put this out-of-sync with tickNear()
    return;
  }

  m_farTickCount = 0;

  //static uint32_t tickTock = 0;
  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {

    // Save CPU mode
    /*
    int32_t start = 0;
    int32_t stop = CHUNK_NONNEIGHBORS_ALL/2;
    int32_t amount = FAR_TICK_AMOUNT*2;
    if (++tickTock % 2) {
      start = CHUNK_NONNEIGHBORS_ALL/2;
      stop = CHUNK_NONNEIGHBORS_ALL;
    }
    */

    // Regular mode
    int32_t start = 0;
    int32_t stop = CHUNK_NONNEIGHBORS_ALL;
    int32_t amount = FAR_TICK_AMOUNT;

    for (uint32_t i = start; i < stop; ++i) { 
      //pd->system->logToConsole("B %u = %i", i, (int) currentChunk->m_nonNeighborsALL[i]);
      m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsALL[i], amount, zoom); // [not currently active] * 2 due to only calling each chunk 50% of the time 
    }
  } else {
    
    // Save CPU mode
    /*
    int32_t start = 0;
    int32_t stop = CHUNK_NONNEIGHBORS_CORNER/2;
    int32_t amount = FAR_TICK_AMOUNT*2;
    if (++tickTock % 2) {
      start = CHUNK_NONNEIGHBORS_CORNER/2;
      stop = CHUNK_NONNEIGHBORS_CORNER;
    }
    */

    // Regular mode
    int32_t start = 0;
    int32_t stop = CHUNK_NONNEIGHBORS_CORNER;
    int32_t amount = FAR_TICK_AMOUNT;

    switch (getCurrentQuadrant()) {
      case NE:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNE[i], amount, zoom);
        }
        break;
      case SE:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSE[i], amount, zoom);
        }
        break;
      case SW:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSW[i], amount, zoom);
        }
        break;
      case NW:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNW[i], amount, zoom);
        }
        break;
    }
  }
} 



int gameLoop(void* _data) {
  ++m_frameCount;
  pd->graphics->setBackgroundColor(kColorBlack);

  if (IOOperationInProgress()) { 
    pd->sprite->removeAllSprites();
    enactIO();
    pd->sprite->drawSprites();
    return 1;
  }

  const enum kGameMode gm = getGameMode();

  clickHandlerReplacement();

  if (gm < kMenuBuy) {
    movePlayer();
  }

  tickNear();
  tickFar();

  if (gm == kTitles) updateUITitles(m_frameCount);
  else updateUI(m_frameCount);

  render();

  struct Player_t* p = getPlayer(); 
  ++p->m_playTime;

  if (gm != kTitles && p->m_enableAutosave) {
    if (++m_autoSaveTimer > 600*TICK_FREQUENCY) {
      m_autoSaveTimer = 0;
      doIO(kDoSave, /*and then*/ kDoNothing);
    }
  }

  return 1;
}

void menuOptionsCallbackDelete(void* _save) {
  int save = (uintptr_t) _save;
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackDelete %i", save);
  #endif
  setSave(save);
  doIO(kDoSaveDelete, /*and then*/ kDoScanSlots);
}

void menuOptionsCallbackLoad(void* blank) {
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackLoad");
  #endif
  setForceSlot(-1); // -1 loads from the slot stored in the player's save file
  doIO(kDoLoad, /*and then*/ kDoNothing);
}

void menuOptionsCallbackSave(void* blank) {
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackSave");
  #endif
  doIO(kDoSave, /*and then*/ kDoNothing);
}

void menuOptionsCallbackMenu(void* blank) {
  #ifdef DEV
  pd->system->logToConsole("menuOptionsCallbackMenu");
  #endif
  checkReturnDismissTutorialMsg();
  drawUITop("Main Menu");
  redrawAllSettingsMenuLines();
  setGameMode(kMenuMain);
}

// Call prior to loading anything
void reset(bool _resetThePlayer) {
  resetCargo();
  resetBuilding();
  resetExportAndSales(); // Essentially "reset special"
  resetLocation();
  resetChunk();
  resetWorld();
  if (_resetThePlayer) resetPlayer();
  forceTorus();
  resetUI();
}

void populateMenuTitle() {
  pd->system->removeAllMenuItems();
  pd->system->addMenuItem("delete save 1", menuOptionsCallbackDelete, (void*)0);
  pd->system->addMenuItem("delete save 2", menuOptionsCallbackDelete, (void*)1);
  pd->system->addMenuItem("delete save 3", menuOptionsCallbackDelete, (void*)2);
}

void populateMenuGame() {
  pd->system->removeAllMenuItems();
  pd->system->addMenuItem("load", menuOptionsCallbackLoad, NULL);
  pd->system->addMenuItem("save", menuOptionsCallbackSave, NULL);
  pd->system->addMenuItem("menu", menuOptionsCallbackMenu, NULL);
}

uint16_t getCactusUnlock() {
  return m_cactusUnlock;
}

void initGame() {

  // Perform runtime sanity check. Ignore placeholders
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

  if (CREDITS_TO_ROW > MAX_ROWS) {
    pd->system->error("INSUFFICIENT ROWS FOR CREDITS!");
  }
  
  if (PLAYER_ANIM_FRAMES != N_FOOTSTEPS) {
    pd->system->error("PLAYER_ANIM_FRAMES != ");
  }

  return;

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
    if (UnlockDecs[i].type == FINAL_UNLOCK_TYPE && UnlockDecs[i].subType.raw == FINAL_UNLOCK_SUBTYPE) break;
  }

}
