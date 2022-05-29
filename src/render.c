#include <math.h>
#include "render.h"
#include "location.h"
#include "cargo.h"
#include "player.h"
#include "sprite.h"
#include "chunk.h"


void chunkAddToRender(struct Chunk_t* _chunk);

void renderUI(void);

uint16_t m_UIIcons[] = {0,7,  0,11,  1,11,  2,11};

/// ///

void renderUI() {

  pd->graphics->setDrawOffset(0, 0);
  //pd->display->setScale(1); // Breaks...

  // GUI
  // TODO make this not be based on zoom
  pd->graphics->fillRect(SCREEN_PIX_X/getZoom(), 0, TILE_PIX/getZoom(), DEVICE_PIX_Y/getZoom(), kColorBlack);
  pd->graphics->fillRect(0, SCREEN_PIX_Y/getZoom(), DEVICE_PIX_X/getZoom(), TILE_PIX/getZoom(), kColorBlack);

  const enum kGameMode gm = getGameMode();

  for (uint32_t i = 0; i < UI_ITEMS; ++i) {
    pd->graphics->drawBitmap(getSprite16(m_UIIcons[i*2], m_UIIcons[(i*2)+1] + m_selectedRotation), SCREEN_PIX_X, TILE_PIX*2*i + TILE_PIX, kBitmapUnflipped);
    if ((gm == kMenuSelect || gm == kMenuOptionSelected) && i == m_selectedID) {
      LCDBitmapDrawMode m = ((gm == kMenuSelect && getFrameCount() % (TICK_FREQUENCY/2) < TICK_FREQUENCY/4) ? kDrawModeInverted : kDrawModeCopy );
      pd->graphics->setDrawMode(m);
      pd->graphics->drawBitmap(getSprite16(0, 2), SCREEN_PIX_X, TILE_PIX*2*i + TILE_PIX, kBitmapUnflipped);
      pd->graphics->setDrawMode(kDrawModeCopy);
    }
  }

  static char text[32];
  snprintf(text, 32, "Conveyors:%u", getNConveyors());
  setRoobert10();
  pd->graphics->setDrawMode(kDrawModeFillWhite);
  pd->graphics->drawText(text, 16, kASCIIEncoding, 0, SCREEN_PIX_Y);
  snprintf(text, 32, "Cargo:%u", getNCargo());
  pd->graphics->drawText(text, 16, kASCIIEncoding, 10*TILE_PIX, SCREEN_PIX_Y);
}

void render() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setDrawOffset(getOffX(), getOffY());
  pd->display->setScale(getZoom()); 

  pd->sprite->updateAndDrawSprites();

  renderUI();

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