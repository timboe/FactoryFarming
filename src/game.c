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

int32_t m_nUnlocks = 0;

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

  // Set prices
  #define DIS UINT16_MAX

  // Note: Plants must be done first, as their prices will then help to set factory output prices
  PDesc[kCarrotPlant].price = 1; // Careful here, player must not be able to go broke
  uint8_t skip = 1;
  for (int32_t i = 1; i < kNPlantSubTypes; ++i) {
    // Skip placeholders
    if (PDesc[i].price == DIS) {
      ++skip;
      continue;
    }

    PDesc[i].price = PDesc[i-skip].price * PDesc[i].multi; 
    CargoDesc[ PDesc[i].out ].price = CargoDesc[ PDesc[i-skip].out ].price * CargoDesc[ PDesc[i].out ].multi;

    #ifdef DEV
    pd->system->logToConsole("Plant: %s will cost %i * %.2f = %i, its output %s will cost %i * %.2f = %i",
      toStringBuilding(kPlant, (union kSubType) {.plant = i}, false),
      PDesc[i-skip].price,
      (double) PDesc[i].multi,
      PDesc[i].price,
      toStringCargoByType(PDesc[i].out, false),
      CargoDesc[ PDesc[i-skip].out ].price,
      (double) CargoDesc[ PDesc[i].out ].multi,
      CargoDesc[ PDesc[i].out ].price);
    #endif

    skip = 1;
  }

  skip = 1;
  FDesc[kVitiminFac].price = 100;
  for (int32_t i = 1; i < kNFactorySubTypes; ++i) {
    // Skip placeholders
    if (FDesc[i].price == DIS) { 
      ++skip;
      continue;
    }

    FDesc[i].price = FDesc[i-skip].price * FDesc[i].multi;
    // Get inputs prices
    uint32_t inputsPrice = CargoDesc[ FDesc[i].in1 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in2 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in3 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in4 ].price;
    inputsPrice += CargoDesc[ FDesc[i].in5 ].price;

    CargoDesc[ FDesc[i].out ].price = inputsPrice * CargoDesc[ FDesc[i].out ].multi;  

    #ifdef DEV

    // Find who unlocks me
    enum kCargoType unlockedWith = kNoCargo;
    int16_t unlockAmount = 0;
    for (int32_t j = 0; j < m_nUnlocks; ++j) {
      if (UnlockDecs[j].type == kFactory && UnlockDecs[j].subType.factory == i) {
        unlockAmount = UnlockDecs[j].fromSelling;
        unlockedWith = UnlockDecs[j].ofCargo;
        break;
      }
    }

    pd->system->logToConsole("Factory: %s will cost %i * %.2f = %i, its output %s will cost %i * %.2f = %i. "
      "Need to sell %i of %s to unlock, and to sell %.2f at %i each to afford",
      toStringBuilding(kFactory, (union kSubType) {.factory = i}, false),
      FDesc[i-skip].price,
      (double) FDesc[i].multi,
      FDesc[i].price,
      //
      toStringCargoByType(FDesc[i].out, false),
      inputsPrice,
      (double) CargoDesc[ FDesc[i].out ].multi,
      CargoDesc[ FDesc[i].out ].price,
      //
      unlockAmount,
      toStringCargoByType(unlockedWith, false),
      (double) FDesc[i].price / CargoDesc[ unlockedWith ].price,
      CargoDesc[ unlockedWith ].price
    );
    #endif

    skip = 1;
  }

  // Extractor prices
  EDesc[kCropHarvesterSmall].price = 50;
  EDesc[kChalkQuarry].price = 75;
  EDesc[kSaltMine].price = FDesc[kCrispsFac].price * 0.75f;
  EDesc[kPump].price = FDesc[kEthanolFac].price * 0.75f;
  EDesc[kCropHarvesterLarge].price = FDesc[kJelloShotFac].price * 0.75f;
  EDesc[kCO2Extractor].price = FDesc[kCaffeineFac].price * 0.75f;

  // Utility prices
  UDesc[kPath].price = FDesc[kEthanolFac].price * 0.05f;
  UDesc[kSign].price = FDesc[kBeerFac].price * 0.20f;
  UDesc[kBin].price = FDesc[kTequilaFac].price * 0.25f;
  UDesc[kWell].price = FDesc[kHFCSFac].price * 0.40f;
  UDesc[kStorageBox].price = FDesc[kEmulsifierFac].price * 0.30f;
  UDesc[kBuffferBox].price = FDesc[kEmulsifierFac].price * 0.60f;
  UDesc[kConveyorGrease].price = FDesc[kBatteryFarm].price * 0.05f;
  UDesc[kObstructionRemover].price = FDesc[kMSGFac].price * 0.40f;
  UDesc[kLandfill].price = FDesc[kPerkPillFac].price * 0.1f;
  UDesc[kRetirement].price = FDesc[kMiraclePowderFac].price * 7.5f;
  UDesc[kFence].price = FDesc[kMiraclePowderFac].price * 0.05f;

  // Warp prices
  WDesc[kChalkWorld].price = FDesc[kBeerFac].price * 5; // First sand
  WDesc[kPeatWorld].price = FDesc[kJelloShotFac].price * 6; // First Clay
  WDesc[kSandWorld].price = FDesc[kCakeFac].price * 7; // First Loam
  WDesc[kClayWorld].price = FDesc[kRaveJuiceFac].price * 8;
  WDesc[kLoamWoarld].price = FDesc[kDessertFac].price * 9;
  WDesc[kWaterWorld].unlock = kPartyPack;
  WDesc[kWaterWorld].price = 500; // This is now number of items
  WDesc[kTranquilWorld].unlock = kMiraclePowder;
  WDesc[kTranquilWorld].price = 1000; // This is now number of items

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
  skip = 1;
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