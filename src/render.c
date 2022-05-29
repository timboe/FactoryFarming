#include <math.h>
#include "render.h"
#include "location.h"
#include "cargo.h"
#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "input.h"
#include "ui.h"

void chunkAddToRender(struct Chunk_t* _chunk);

/// ///

void render() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setDrawOffset(getOffX(), getOffY());
  pd->display->setScale(getZoom()); 

  if (getFrameCount() % NEAR_TICK_FREQUENCY == 0) {
    pd->sprite->updateAndDrawSprites();
  } else {
    pd->sprite->drawSprites();
  }

  // Draw FPS indicator (dbg only)
  #ifdef DEBUG_MODE
  pd->system->drawFPS(0, 0);
  #endif 
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

  struct Player_t* player = getPlayer();
  pd->sprite->addSprite(player->m_sprite);
  pd->sprite->addSprite(player->m_blueprint);

  pd->sprite->addSprite(getUISpriteBottom());
  pd->sprite->addSprite(getUISpriteRight());

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