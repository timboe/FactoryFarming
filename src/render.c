#include <math.h>
#include "render.h"
#include "location.h"
#include "cargo.h"
#include "player.h"
#include "sprite.h"
#include "chunk.h"

void chunkAddToRender(struct Chunk_t* _chunk);

/// ///

void render() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setDrawOffset(getOffX(), getOffY());
  pd->display->setScale(getZoom()); 

  pd->sprite->updateAndDrawSprites();

  ///////////////

  pd->graphics->setDrawOffset(0, 0);
  //pd->display->setScale(1); // Breaks...

  // GUI
  pd->graphics->fillRect(SCREEN_PIX_X/getZoom(), 0, TILE_PIX/getZoom(), DEVICE_PIX_Y/getZoom(), kColorBlack);
  pd->graphics->fillRect(0, SCREEN_PIX_Y/getZoom(), DEVICE_PIX_X/getZoom(), TILE_PIX/getZoom(), kColorBlack);

  // Draw FPS indicator (dbg only)
  #ifdef DEBUG_MODE
  pd->system->drawFPS(0, 0);
  #endif

  static char text[32];
  snprintf(text, 32, "Conveyors:%u", getNConveyors());
  setRoobert10();
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  pd->graphics->drawText(text, 16, kASCIIEncoding, 0, SCREEN_PIX_Y);
  snprintf(text, 32, "Cargo:%u", getNCargo());
  pd->graphics->drawText(text, 16, kASCIIEncoding, 10*TILE_PIX, SCREEN_PIX_Y);
  
}

void chunkAddToRender(struct Chunk_t* _chunk) {
  // TODO in the future all chunks should have all backgrounds populated
  //pd->system->logToConsole("CATR %i %i", _chunk->m_x, _chunk->m_y);
  if (_chunk->m_bkgSprite) pd->sprite->addSprite(_chunk->m_bkgSprite);
  if (getZoom() > 1) {
    for (uint32_t i = 0; i < _chunk->m_nLocations; ++i) {
      struct Location_t* loc = _chunk->m_locations[i];
      pd->sprite->addSprite(loc->m_sprite);
    }
  }
  for (uint32_t i = 0; i < _chunk->m_nCargos; ++i) {
    struct Cargo_t* cargo = _chunk->m_cargos[i];
    pd->sprite->addSprite(cargo->m_sprite);
  }
}

void updateRenderList() {
  struct Chunk_t* currentChunk = getCurrentChunk();

  pd->sprite->removeAllSprites();

  pd->sprite->addSprite(getPlayer()->m_sprite);

  chunkAddToRender(currentChunk);

  if (getZoom() == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      chunkAddToRender(currentChunk->m_neighborsALL[i]);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        chunkAddToRender(currentChunk->m_neighborsNE[0]);
        chunkAddToRender(currentChunk->m_neighborsNE[1]);
        chunkAddToRender(currentChunk->m_neighborsNE[2]);
        break;
      case SE:;
        chunkAddToRender(currentChunk->m_neighborsSE[0]);
        chunkAddToRender(currentChunk->m_neighborsSE[1]);
        chunkAddToRender(currentChunk->m_neighborsSE[2]);
        break;
      case SW:;
        chunkAddToRender(currentChunk->m_neighborsSW[0]);
        chunkAddToRender(currentChunk->m_neighborsSW[1]);
        chunkAddToRender(currentChunk->m_neighborsSW[2]);
        break;
      case NW:;
        chunkAddToRender(currentChunk->m_neighborsNW[0]);
        chunkAddToRender(currentChunk->m_neighborsNW[1]);
        chunkAddToRender(currentChunk->m_neighborsNW[2]);
        break;
    }
  }
}