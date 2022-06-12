#include <math.h>
#include "render.h"
#include "location.h"
#include "cargo.h"
#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "input.h"
#include "ui.h"

void chunkAddToRender(struct Chunk_t* _chunk, uint8_t _zoom);

/// ///

void render() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setDrawOffset(getOffX(), getOffY());

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

void chunkAddToRender(struct Chunk_t* _chunk, uint8_t _zoom) {
  // TODO in the future all chunks should have all backgrounds populated
  //pd->system->logToConsole("CATR %i %i", _chunk->m_x, _chunk->m_y);
  pd->sprite->addSprite(_chunk->m_bkgSprite[_zoom]);
  for (uint32_t i = 0; i < _chunk->m_nBuildings; ++i) {
    if (_chunk->m_buildings[i]->m_type != kConveyor || getZoom() == 2) {
      pd->sprite->addSprite(_chunk->m_buildings[i]->m_sprite[_zoom]);
    }
  }
  for (uint32_t i = 0; i < _chunk->m_nCargos; ++i) {
    pd->sprite->addSprite(_chunk->m_cargos[i]->m_sprite[_zoom]);
  }
  for (uint32_t i = 0; i < _chunk->m_nObstacles; ++i) {
    pd->sprite->addSprite(_chunk->m_obstacles[i][_zoom]);
  }
}

void updateRenderList() {
  uint8_t zoom = getZoom();
  struct Chunk_t* currentChunk = getCurrentChunk();

  pd->sprite->removeAllSprites();

  struct Player_t* player = getPlayer();
  pd->sprite->addSprite(player->m_sprite[zoom]);
  pd->sprite->addSprite(player->m_blueprint[zoom]);
  pd->sprite->addSprite(player->m_blueprintRadius[zoom]);

  addUIToSpriteList();

  chunkAddToRender(currentChunk, zoom);

  if (getZoom() == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      chunkAddToRender(currentChunk->m_neighborsALL[i], zoom);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        chunkAddToRender(currentChunk->m_neighborsNE[0], zoom);
        chunkAddToRender(currentChunk->m_neighborsNE[1], zoom);
        chunkAddToRender(currentChunk->m_neighborsNE[2], zoom);
        break;
      case SE:;
        chunkAddToRender(currentChunk->m_neighborsSE[0], zoom);
        chunkAddToRender(currentChunk->m_neighborsSE[1], zoom);
        chunkAddToRender(currentChunk->m_neighborsSE[2], zoom);
        break;
      case SW:;
        chunkAddToRender(currentChunk->m_neighborsSW[0], zoom);
        chunkAddToRender(currentChunk->m_neighborsSW[1], zoom);
        chunkAddToRender(currentChunk->m_neighborsSW[2], zoom);
        break;
      case NW:;
        chunkAddToRender(currentChunk->m_neighborsNW[0], zoom);
        chunkAddToRender(currentChunk->m_neighborsNW[1], zoom);
        chunkAddToRender(currentChunk->m_neighborsNW[2], zoom);
        break;
    }
  }
}