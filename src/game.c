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

void tickNear(void);

void tickFar(void);

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

  static uint32_t tickTock = 0;
  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {
    int32_t start = 0;
    int32_t stop = CHUNK_NONNEIGHBORS_ALL/2;
    if (++tickTock % 2) {
      start = CHUNK_NONNEIGHBORS_ALL/2;
      stop = CHUNK_NONNEIGHBORS_ALL;
    }

    for (uint32_t i = start; i < stop; ++i) { 
      //pd->system->logToConsole("B %u = %i", i, (int) currentChunk->m_nonNeighborsALL[i]);
      m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsALL[i], FAR_TICK_AMOUNT*2, zoom); // * 2 due to only calling each chunk 50% of the time 
    }
  } else {
    int32_t start = 0;
    int32_t stop = CHUNK_NONNEIGHBORS_CORNER/2;
    if (++tickTock % 2) {
      start = CHUNK_NONNEIGHBORS_CORNER/2;
      stop = CHUNK_NONNEIGHBORS_CORNER;
    }

    switch (getCurrentQuadrant()) {
      case NE:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNE[i], FAR_TICK_AMOUNT*2, zoom);
        }
        break;
      case SE:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSE[i], FAR_TICK_AMOUNT*2, zoom);
        }
        break;
      case SW:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsSW[i], FAR_TICK_AMOUNT*2, zoom);
        }
        break;
      case NW:;
        for (uint32_t i = start; i < stop; ++i) {
          m_farTickCount += chunkTickChunk(currentChunk->m_nonNeighborsNW[i], FAR_TICK_AMOUNT*2, zoom);
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

  updateUI(m_frameCount);

  render();

  return 1;
}

void menuOptionsCallbackRestart(void* blank) {
  reset();
  generate();
  addObstacles();
  setChunkBackgrounds();
  updateRenderList();
}

void menuOptionsCallbackLoad(void* blank) {
  reset();
  load();
  addObstacles();
  doWetness();
  setChunkBackgrounds();
  updateRenderList();
}

void menuOptionsCallbackSave(void* blank) {
  save();
}

// Call prior to loading anything
void reset() {
  resetCargo();
  resetBuilding();
  resetLocation();
  resetChunk();
  resetWorld();
  resetPlayer();
  resetUI();
}

void initGame() {
  pd->system->addMenuItem("restart", menuOptionsCallbackRestart, NULL);

  pd->system->addMenuItem("load", menuOptionsCallbackLoad, NULL);

  pd->system->addMenuItem("save", menuOptionsCallbackSave, NULL);
}
