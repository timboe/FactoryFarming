#include <math.h>
#include "render.h"
#include "location.h"
#include "cargo.h"
#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "input.h"
#include "ui.h"
#include "buildings/conveyor.h"

float m_trauma = 0.0f, m_decay = 0.0f;

void chunkAddToRender(struct Chunk_t* _chunk, uint8_t _zoom);

/// ///

void addTrauma(float _amount) {
  m_trauma += _amount;
  m_trauma *= -1;
  m_decay = _amount;
}

void render() {

  struct Player_t* p = getPlayer();

  if (p->m_enableScreenShake && m_decay > 0.0f) {
    m_decay -= TRAUMA_DECAY;
    m_trauma += (m_trauma > 0 ? -m_decay : m_decay);
    pd->display->setOffset(0, m_trauma * TRAUMA_AMPLIFICATION);
  } else {
    pd->display->setOffset(0, 0);
  }

  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setDrawOffset(getOffX(), getOffY());

  if (getFrameCount() % NEAR_TICK_FREQUENCY == 0) {
    pd->sprite->updateAndDrawSprites();
  } else {
    pd->sprite->drawSprites();
  }

  // Draw FPS indicator (dbg only)
  if (p->m_enableDebug) {
    pd->system->drawFPS(0, 0);
  }
}

void chunkAddToRender(struct Chunk_t* _chunk, uint8_t _zoom) {
  if (_chunk->m_bkgSprite[_zoom]) pd->sprite->addSprite(_chunk->m_bkgSprite[_zoom]);
  for (uint32_t i = 0; i < _chunk->m_nBuildingsRender; ++i) {
    // TODO - efficient way to not include conveyors if getPlayer()->m_enableConveyorAnimation is false?
    struct Building_t* b = _chunk->m_buildingsRender[i];
    if (b->m_sprite[_zoom]) {
      pd->sprite->addSprite(b->m_sprite[_zoom]);
    }
    // Force update the drawing of the current location of the cargo
    if (b->m_type == kConveyor && b->m_location->m_cargo) {
      conveyorLocationUpdate(b, _zoom);
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
  if (getGameMode() == kMenuCredits) {
    pd->sprite->removeAllSprites();
    addUIToSpriteList();
    return;
  }

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