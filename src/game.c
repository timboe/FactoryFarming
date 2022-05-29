#include <math.h>

#include "game.h"
#include "sprite.h"
#include "generate.h"
#include "render.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "ui.h"


PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;

enum kGameMode m_mode;

void tickNear(void);

void tickFar(void);

////////////

void setPDPtr(PlaydateAPI* _p) {
  pd = _p;
}

int getFrameCount() { 
  return m_frameCount; 
}

enum kGameMode getGameMode() {
  return m_mode;
}

void setGameMode(enum kGameMode _mode) {
  m_mode = _mode;
}

void tickNear() {
  if (m_frameCount % NEAR_TICK_FREQUENCY) {
    return;
  }

  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  chunkTickChunk(currentChunk, NEAR_TICK_AMOUNT, zoom);

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      chunkTickChunk(currentChunk->m_neighborsALL[i], NEAR_TICK_AMOUNT, zoom);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        chunkTickChunk(currentChunk->m_neighborsNE[0], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsNE[1], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsNE[2], NEAR_TICK_AMOUNT, zoom);
        break;
      case SE:;
        chunkTickChunk(currentChunk->m_neighborsSE[0], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsSE[1], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsSE[2], NEAR_TICK_AMOUNT, zoom);
        break;
      case SW:;
        chunkTickChunk(currentChunk->m_neighborsSW[0], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsSW[1], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsSW[2], NEAR_TICK_AMOUNT, zoom);
        break;
      case NW:;
        chunkTickChunk(currentChunk->m_neighborsNW[0], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsNW[1], NEAR_TICK_AMOUNT, zoom);
        chunkTickChunk(currentChunk->m_neighborsNW[2], NEAR_TICK_AMOUNT, zoom);
        break;
    }
  }
}

void tickFar() {
  if(m_frameCount % FAR_TICK_FREQUENCY) {
    return;
  }

  if (m_mode == kWander) {
    UIDirtyBottom();
  }

  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (zoom == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_ALL - 1); ++i) { // The "- 1" is for "me"
      //pd->system->logToConsole("B %u = %i", i, (int) currentChunk->m_nonNeighborsALL[i]);
      chunkTickChunk(currentChunk->m_nonNeighborsALL[i], FAR_TICK_AMOUNT, zoom);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsNE[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
      case SE:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsSE[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
      case SW:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsSW[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
      case NW:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsNW[i], FAR_TICK_AMOUNT, zoom);
        }
        break;
    }
  }
} 

int gameLoop(void* _data) {
  if (++m_frameCount == TICK_FREQUENCY) m_frameCount = 0;

  clickHandlerReplacement();

  if (getZoom() > 1 && m_frameCount % NEAR_TICK_FREQUENCY == 0) {
    animateConveyor();
  }
  
  movePlayer();

  tickNear();
  tickFar();

  updateUI();

  render();

  return 1;
}

void menuOptionsCallback(void* userdata) {
}

void initGame() {
  generate();
}
