#include <math.h>

#include "game.h"
#include "sprite.h"
#include "generate.h"
#include "render.h"
#include "sound.h"
#include "player.h"

PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;
uint8_t m_pressed[4] = {0};
float m_rot = 0;
uint8_t m_zoom = 1;



void tickNear(void);

void tickFar(void);

void chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tick);

////////////

void setPDPtr(PlaydateAPI* _p) {
  pd = _p;
}

uint8_t getPressed(uint32_t _i) {
  return m_pressed[_i];
}

int getFrameCount() { 
  return m_frameCount; 
}

uint8_t getZoom() {
  return m_zoom;
}


void gameClickConfigHandler(uint32_t _buttonPressed) {
    static uint32_t x = 0, x2 = 4;
  if (kButtonLeft == _buttonPressed) m_pressed[0] = 1;
  else if (kButtonRight == _buttonPressed) m_pressed[1] = 1;
  else if (kButtonUp == _buttonPressed) m_pressed[2] = 1;
  else if (kButtonDown == _buttonPressed) m_pressed[3] = 1;
  /*
  else if (kButtonB == _buttonPressed) {
    x += 5;
    for (int i = 0; i < TOT_TILES_Y; ++i) newConveyor(x, i, x % 2 ? SN : NS);
  } else if (kButtonA == _buttonPressed) {
    for (int i = 0; i < TOT_TILES_Y; i += rand()%8 + 1) {
      newCargo(x, i, kApple);
      break;
    }
  }
  */
  else if (kButtonB == _buttonPressed) {
    x += 5;
    for (int i = 0; i < TOT_TILES_X; ++i) newConveyor(i, x, x % 2 ? EW : WE);
  } else if (kButtonA == _buttonPressed) {
    for (int i = 0; i < TOT_TILES_Y; i += rand()%8 + 1) {
      newCargo(i, x, kApple);
      break;
    }
  }

}


// Temporary until the playdate eventHandler is functional for inputs
void clickHandlerReplacement() {
  PDButtons current, pushed, released = 0;
  pd->system->getButtonState(&current, &pushed, &released);
  if (pushed & kButtonUp) gameClickConfigHandler(kButtonUp);
  if (pushed & kButtonRight) gameClickConfigHandler(kButtonRight);
  if (pushed & kButtonDown) gameClickConfigHandler(kButtonDown);
  if (pushed & kButtonLeft) gameClickConfigHandler(kButtonLeft);
  if (pushed & kButtonB) gameClickConfigHandler(kButtonB);
  if (pushed & kButtonA) gameClickConfigHandler(kButtonA);

  if (released & kButtonLeft) m_pressed[0] = 0;
  if (released & kButtonRight) m_pressed[1] = 0;
  if (released & kButtonUp) m_pressed[2] = 0;
  if (released & kButtonDown) m_pressed[3] = 0;

  m_rot += pd->system->getCrankChange();
  if (m_rot > 260.0f) {
    m_rot = 0.0f;
    if (m_zoom < 4) {
      pd->system->logToConsole("ZOOM IN");
      m_zoom *= 2;
      //m_offX = getPlayer()->m_x - (SCREEN_PIX_X / (m_zoom * 2)); // TODO this is wrong
      //m_offY = getPlayer()->m_y - (SCREEN_PIX_Y / (m_zoom * 2));
      if (m_zoom == 2) updateRenderList(); // Zoom 1-to-2, add detail
    }
  } else if (m_rot < -260.0f) {
    m_rot = 0.0f;
    pd->system->logToConsole("ZOOM OUT");
    if (m_zoom > 1) {
      m_zoom /= 2;
      //m_offX = getPlayer()->m_x - (SCREEN_PIX_X / (m_zoom * 2)); // done in player?
      //m_offY = getPlayer()->m_y - (SCREEN_PIX_Y / (m_zoom * 2));
      if (m_zoom == 1) updateRenderList(); // Zoom 2-to-1, remove detail
    }
  }
}


void chunkTickChunk(struct Chunk_t* _chunk, uint8_t _tick) {
  //pd->system->logToConsole("Asked to tick %i for %i", _tick, _chunk);
  for (uint32_t i = 0; i < _chunk->m_nLocations; ++i) {
    struct Location_t* loc = _chunk->m_locations[i];
    (*loc->m_updateFn)(loc, _tick);
  }
}

void tickNear() {
  struct Chunk_t* currentChunk = getCurrentChunk();

  chunkTickChunk(currentChunk, NEAR_TICK_AMOUNT);

  if (getZoom() == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      chunkTickChunk(currentChunk->m_neighborsALL[i], NEAR_TICK_AMOUNT);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        chunkTickChunk(currentChunk->m_neighborsNE[0], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsNE[1], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsNE[2], NEAR_TICK_AMOUNT);
        break;
      case SE:;
        chunkTickChunk(currentChunk->m_neighborsSE[0], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsSE[1], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsSE[2], NEAR_TICK_AMOUNT);
        break;
      case SW:;
        chunkTickChunk(currentChunk->m_neighborsSW[0], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsSW[1], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsSW[2], NEAR_TICK_AMOUNT);
        break;
      case NW:;
        chunkTickChunk(currentChunk->m_neighborsNW[0], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsNW[1], NEAR_TICK_AMOUNT);
        chunkTickChunk(currentChunk->m_neighborsNW[2], NEAR_TICK_AMOUNT);
        break;
    }
  }
}

void tickFar() {
  struct Chunk_t* currentChunk = getCurrentChunk();

  if (getZoom() == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_ALL - 1); ++i) { // The "- 1" is for "me"
      //pd->system->logToConsole("B %u = %i", i, (int) currentChunk->m_nonNeighborsALL[i]);
      chunkTickChunk(currentChunk->m_nonNeighborsALL[i], FAR_TICK_AMOUNT);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsNE[i], FAR_TICK_AMOUNT);
        }
        break;
      case SE:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsSE[i], FAR_TICK_AMOUNT);
        }
        break;
      case SW:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsSW[i], FAR_TICK_AMOUNT);
        }
        break;
      case NW:;
        for (uint32_t i = 0; i < (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1); ++i) {
          chunkTickChunk(currentChunk->m_nonNeighborsNW[i], FAR_TICK_AMOUNT);
        }
        break;
    }
  }
} 

int gameLoop(void* _data) {
  clickHandlerReplacement();

  if (++m_frameCount == 1024) m_frameCount = 0;

  if (getZoom() > 1 && m_frameCount % NEAR_TICK_FREQUENCY == 0) {
    animateConveyor();
  }
  
  movePlayer();

  if (m_frameCount % NEAR_TICK_FREQUENCY == 0) tickNear();
  if (m_frameCount % FAR_TICK_FREQUENCY == 0) tickFar();

  render();

  return 1;
}

void menuOptionsCallback(void* userdata) {
}

void initGame() {
  generate();
}
