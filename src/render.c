#include <math.h>
#include "render.h"
#include "location.h"
#include "cargo.h"
#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "io.h"
#include "input.h"
#include "ui.h"
#include "buildings/conveyor.h"

float m_trauma = 0.0f, m_decay = 0.0f;

void chunkAddToRender(struct Chunk_t* _chunk, uint8_t _zoom, bool _includeCargoAndObs);

/// ///

void addTrauma(float _amount) {
  m_trauma += _amount;
  m_trauma *= -1;
  m_decay = _amount;
}

void render() {

  struct Player_t* p = getPlayer();
  const uint8_t zoom = getZoom();

  if (p->m_enableScreenShake && m_decay > 0.0f) {
    m_decay -= TRAUMA_DECAY;
    m_trauma += (m_trauma > 0 ? -m_decay : m_decay);
    pd->display->setOffset(0, m_trauma * TRAUMA_AMPLIFICATION);
  } else {
    pd->display->setOffset(0, 0);
  }

  const float offX = -(p->m_camera_pix_x*zoom - (SCREEN_PIX_X/2));
  const float offY = -(p->m_camera_pix_y*zoom - (SCREEN_PIX_Y/2));

  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->setDrawOffset(offX, offY);

  if (p->m_enableConveyorAnimation && zoom == 2) {

    if (getFrameCount() % NEAR_TICK_FREQUENCY == 0) {
      pd->sprite->updateAndDrawSprites();
    } else {
      pd->sprite->drawSprites();
    }

  } else {

    pd->sprite->drawSprites();

  }

  // Draw FPS indicator (dbg only)
  if (p->m_enableDebug || ALWAYS_FPS) {
    pd->system->drawFPS(0, 0);
  }
}

bool checkBound(LCDSprite* _s, uint16_t _hw, uint16_t _hh, int16_t _sX, int16_t _sY, uint8_t _zoom) {
  float x, y;
  pd->sprite->getPosition(_s, &x, &y);
  const int16_t x_i = x / _zoom, y_i = y / _zoom;
  // pd->system->logToConsole(" -- -- -- sprite is at: %i %i, it has hw, hh:%i %i, zoom is:%i", x_i, y_i, _hw, _hh, _zoom);
  // pd->system->logToConsole(" -- -- screen is at: %i %i", _sX, _sY);
  // pd->system->logToConsole(" -- -- bounds are: < %i  ^ %i > %i v %i ", 
  //   -(HALF_SCREEN_PIX_X/_zoom),
  //   -(HALF_SCREEN_PIX_Y/_zoom),
  //   HALF_SCREEN_PIX_X/_zoom,
  //   HALF_SCREEN_PIX_Y/_zoom);
  // pd->system->logToConsole(" -- veto if x %i <  %i - %s", (x_i + _hw - _sX), -(HALF_SCREEN_PIX_X/_zoom), x_i + _hw - _sX < -(HALF_SCREEN_PIX_X/_zoom) ? "VETO" : "OK");
  // pd->system->logToConsole(" -- veto if x %i >= %i - %s", (x_i - _hw - _sX), (HALF_SCREEN_PIX_X/_zoom),  x_i - _hw - _sX >= (HALF_SCREEN_PIX_X/_zoom) ? "VETO" : "OK");
  // pd->system->logToConsole(" -- veto if y %i <  %i - %s", (y_i + _hh - _sY), -(HALF_SCREEN_PIX_Y/_zoom), y_i + _hh - _sY < -(HALF_SCREEN_PIX_Y/_zoom) ? "VETO" : "OK");
  // pd->system->logToConsole(" -- veto if y %i >= %i - %s", (y_i - _hh - _sY), (HALF_SCREEN_PIX_Y/_zoom),  y_i - _hh - _sY >= (HALF_SCREEN_PIX_Y/_zoom) ? "VETO" : "OK");

  if (x_i + _hw - _sX < -(HALF_SCREEN_PIX_X/_zoom)) return false;// { pd->system->logToConsole(" - V0"); return false; }
  if (x_i - _hw - _sX >= HALF_SCREEN_PIX_X/_zoom) return false;//{ pd->system->logToConsole(" - V1"); return false; }
  //
  if (y_i + _hh - _sY < -(HALF_SCREEN_PIX_Y/_zoom)) return false;//{ pd->system->logToConsole(" - V2"); return false; }
  if (y_i - _hh - _sY >= HALF_SCREEN_PIX_Y/_zoom) return false;//{ pd->system->logToConsole(" - V3"); return false; }
  //pd->system->logToConsole(" - OK");
  return true;
}

void chunkAddToRender(struct Chunk_t* _chunk, uint8_t _zoom, bool _includeCargoAndObs) {
  LCDSprite* bg = _chunk->m_bkgSprite[_zoom];
  struct Player_t* p = getPlayer();
  // 
  // Disabled for now as player can run faster than tick update
  //if (bg && checkBound(bg, CHUNK_PIX_X/2, CHUNK_PIX_Y/2, p->m_camera_pix_x, p->m_camera_pix_y, _zoom)) {
  //  pd->sprite->addSprite(bg);
  //}
  //
  if (bg) pd->sprite->addSprite(bg);
  //
  const bool animOn = p->m_enableConveyorAnimation; 
  for (uint32_t i = 0; i < _chunk->m_nBuildingsRender; ++i) {
    struct Building_t* b = _chunk->m_buildingsRender[i];
    if (b->m_sprite[_zoom] && (animOn || b->m_type != kConveyor)) {
      //pd->sprite->addSprite(b->m_sprite[_zoom]);
      LCDSprite* s = b->m_sprite[_zoom];
      if (checkBound(s, (TILE_PIX*3)/2, (TILE_PIX*3)/2, p->m_camera_pix_x, p->m_camera_pix_y, _zoom)) {
        pd->sprite->addSprite(s);
      }
    }
    // Force update the drawing of the current location of the cargo
    if (b->m_type == kConveyor && b->m_location->m_cargo) {
      conveyorLocationUpdate(b, _zoom);
    }
  }
  if (!_includeCargoAndObs) {
    return;
  }
  for (uint32_t i = 0; i < _chunk->m_nCargos; ++i) {
    LCDSprite* s = _chunk->m_cargos[i]->m_sprite[_zoom];
    if (checkBound(s, TILE_PIX/2, TILE_PIX/2, p->m_camera_pix_x, p->m_camera_pix_y, _zoom)) {
      pd->sprite->addSprite(s);
    }
  }
  for (uint32_t i = 0; i < _chunk->m_nObstacles; ++i) {
    LCDSprite* s = _chunk->m_obstacles[i][_zoom];
    if (checkBound(s, TILE_PIX/2, TILE_PIX/2, p->m_camera_pix_x, p->m_camera_pix_y, _zoom)) {
      pd->sprite->addSprite(s);
    }
  }
}

void updateRenderList() {
  const enum kGameMode gm = getGameMode();
  if (gm == kMenuCredits) {
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

  const enum kSaveLoadRequest io = currentIOAction();

  if (io != kDoScreenShot) {
    addUIToSpriteList();
  }

  const bool includeCargoAndObs = (gm < kMenuBuy || gm == kTitles || io == kDoScreenShot);

  chunkAddToRender(currentChunk, zoom, includeCargoAndObs);
  if (getZoom() == 1 && !PRETEND_ZOOMED_IN) {
    for (uint32_t i = 0; i < CHUNK_NEIGHBORS_ALL; ++i) {
      chunkAddToRender(currentChunk->m_neighborsALL[i], zoom, includeCargoAndObs);
    }
  } else {
    switch (getCurrentQuadrant()) {
      case NE:;
        chunkAddToRender(currentChunk->m_neighborsNE[0], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsNE[1], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsNE[2], zoom, includeCargoAndObs);
        break;
      case SE:;
        chunkAddToRender(currentChunk->m_neighborsSE[0], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsSE[1], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsSE[2], zoom, includeCargoAndObs);
        break;
      case SW:;
        chunkAddToRender(currentChunk->m_neighborsSW[0], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsSW[1], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsSW[2], zoom, includeCargoAndObs);
        break;
      case NW:;
        chunkAddToRender(currentChunk->m_neighborsNW[0], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsNW[1], zoom, includeCargoAndObs);
        chunkAddToRender(currentChunk->m_neighborsNW[2], zoom, includeCargoAndObs);
        break;
    }
  }
}