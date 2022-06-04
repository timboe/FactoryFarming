#include <math.h>

#include "game.h"
#include "sprite.h"
#include "generate.h"
#include "render.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "ui.h"
#include "io.h"

PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;

uint16_t m_nearTickCount = 0;

uint16_t m_farTickCount = 0;

bool m_queueUpdateRenderList = false;

enum kGameMode m_mode;

void tickNear(void);

void tickFar(void);

void menuOptionsCallbackRestart(void*);

void menuOptionsCallbackLoad(void* _menu);

void menuOptionsCallbackSave(void* _menu);

////////////

void setPDPtr(PlaydateAPI* _p) {
  pd = _p;
}

void queueUpdateRenderList() {
  m_queueUpdateRenderList = true;
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

enum kGameMode getGameMode() {
  return m_mode;
}

void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;
  if (_mode == kMenuSelect) drawUITop("Menu Mode");
  else if (_mode == kMenuOptionSelected) drawUITop("Place Mode");
  else drawUITop(NULL);
}

void tickNear() {
  if (m_frameCount % NEAR_TICK_FREQUENCY) {
    return;
  }
  m_nearTickCount = 0;

  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

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
  if((m_frameCount + 1) % FAR_TICK_FREQUENCY) { // +1 to put this out-of-sync with tickNear()
    return;
  }
  m_farTickCount = 0;

  if (m_queueUpdateRenderList) {
    m_queueUpdateRenderList = false;
    updateRenderList();
  }

  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_ALL - 1); ++i) { // The "- 1" is for "me"
      //pd->system->logToConsole("B %u = %i", i, (int) currentChunk->m_nonNeighborsALL[i]);
      m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsALL[i], FAR_TICK_AMOUNT, zoom);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNE[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
      case SE:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSE[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
      case SW:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSW[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
      case NW:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNW[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
    }
  }
} 

int gameLoop(void* _data) {
  ++m_frameCount;

  clickHandlerReplacement();

  if (getZoom() > 1 && m_frameCount % NEAR_TICK_FREQUENCY == 0) {
    animateConveyor();
  }
  
  movePlayer();

  tickNear();
  tickFar();

  updateUI(m_frameCount, m_mode);

  render();

  return 1;
}

void menuOptionsCallbackRestart(void* blank) {
}

void menuOptionsCallbackLoad(void* _menu) {
  int value = pd->system->getMenuItemValue((PDMenuItem*)_menu);
  setSlot(value);
  load();
}

void menuOptionsCallbackSave(void* _menu) {
  int value = pd->system->getMenuItemValue((PDMenuItem*)_menu);
  setSlot(value);
  save();
}

void initGame() {
  generate();

  pd->system->addMenuItem("restart", menuOptionsCallbackRestart, NULL);


  static const char* slots[] = {"Slot 0", "Slot 1", "Slot 2"};
  PDMenuItem* _menu1 = pd->system->addOptionsMenuItem("load", slots, 3, menuOptionsCallbackLoad, NULL);
  pd->system->setMenuItemUserdata(_menu1, (void*) _menu1); 

  PDMenuItem* _menu0 = pd->system->addOptionsMenuItem("save", slots, 3, menuOptionsCallbackSave, NULL);
  pd->system->setMenuItemUserdata(_menu0, (void*) _menu0);
}
