#include <math.h>

#include "game.h"
#include "sprite.h"
#include "generate.h"
#include "render.h"
#include "sound.h"

PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;
uint8_t m_pressed[4] = {0};
float m_rot = 0;
uint8_t m_zoom = 1;

int16_t m_offX, m_offY = 0; // Screen offsets

uint16_t m_curChunkX = 0;
uint16_t m_curChunkY = 0; 

uint8_t m_curSubChunkX = 0;
uint8_t m_curSubChunkY = 0;
enum kChunkQuad m_quadrant = 0;


int getFrameCount() { 
  return m_frameCount; 
}


void setPDPtr(PlaydateAPI* _p) {
  pd = _p;
}

void gameClickConfigHandler(uint32_t _buttonPressed) {
    static uint32_t x = 0, x2 = 4;
  if (kButtonLeft == _buttonPressed) m_pressed[0] = 1;
  else if (kButtonRight == _buttonPressed) m_pressed[1] = 1;
  else if (kButtonUp == _buttonPressed) m_pressed[2] = 1;
  else if (kButtonDown == _buttonPressed) m_pressed[3] = 1;
  else if (kButtonB == _buttonPressed) {
    x += 5;
    for (int i = 0; i < TOT_TILES_Y; ++i) newConveyor(x, i, x % 2 ? SN : NS);

  } else if (kButtonA == _buttonPressed) {
    for (int i = 0; i < TOT_TILES_Y; i += rand()%4 + 1) newCargo(x, i, kApple);
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
  if (m_rot > 45.0f) {
    m_rot = 0.0f;
    if (m_zoom < 4) {
      m_zoom *= 2;
      m_offX = getPlayer()->m_x - (SCREEN_PIX_X / (m_zoom * 2)); // TODO this is wrong
      m_offY = getPlayer()->m_y - (SCREEN_PIX_Y / (m_zoom * 2));
    }
  } else if (m_rot < -45.0f) {
    m_rot = 0.0f;
    if (m_zoom > 1) {
      m_zoom /= 2;
      m_offX = getPlayer()->m_x - (SCREEN_PIX_X / (m_zoom * 2));
      m_offY = getPlayer()->m_y - (SCREEN_PIX_Y / (m_zoom * 2));
    }
  }
}

uint8_t getZoom() {
  return m_zoom;
}

void render() {
  pd->display->setScale(m_zoom);


  pd->sprite->updateAndDrawSprites();

  // Draw FPS indicator (dbg only)
  #ifdef DEBUG_MODE
  pd->system->drawFPS(0, 0);

  if (getZoom() == 1) {
    static char text[32];
    snprintf(text, 32, "Conveyors:%u", getNConveyors());
    setRoobert11();
    pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_PIX - m_offX, 1*TILE_PIX - m_offY);
    snprintf(text, 32, "Cargo:%u", getNCargo());
    pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_PIX - m_offX, 2*TILE_PIX - m_offY);
  }
  #endif
}



int gameLoop(void* _data) {

  clickHandlerReplacement();

  if (++m_frameCount == 1024) m_frameCount = 0;
  bool requestRedraw = true;

  animateConveyor();
  
  movePlayer();

  if (requestRedraw) {
    render();
  }

  // TODO: The docs say that returning 0 does not request a redraw,
  // but it is currently required
  requestRedraw = true;

  return (int)requestRedraw;
}

void updateRenderListZoom(struct Chunk_t* chunk, enum kChunkQuad quadrant) {
  pd->sprite->removeAllSprites();

  pd->sprite->addSprite(getPlayer()->m_sprite);

  pd->sprite->addSprite(chunk->m_bkgSprite);
  switch (quadrant) {
    case NE:;
      pd->sprite->addSprite(chunk->m_neighboursNE[0]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursNE[1]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursNE[2]->m_bkgSprite);
      break;
    case SE:;
      pd->sprite->addSprite(chunk->m_neighboursSE[0]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursSE[1]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursSE[2]->m_bkgSprite);
      break;
    case SW:;
      pd->sprite->addSprite(chunk->m_neighboursSW[0]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursSW[1]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursSW[2]->m_bkgSprite);
      break;
    case NW:;
      pd->sprite->addSprite(chunk->m_neighboursNW[0]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursNW[1]->m_bkgSprite);
      pd->sprite->addSprite(chunk->m_neighboursNW[2]->m_bkgSprite);
      break;
  }
}


bool movePlayer() {
  struct Player_t* p = getPlayer();

  pd->sprite->getPosition(p->m_sprite, &(p->m_x), &(p->m_y)); // TODO Docs have int instead of float
  float goalX = p->m_x;
  float goalY = p->m_y;
  
  /*

  float diffX = 0;
  float diffY = 0;

  if (m_pressed[0]) diffX -= PLAYER_A;
  if (m_pressed[1]) diffX += PLAYER_A;
  if (m_pressed[2]) diffY -= PLAYER_A;
  if (m_pressed[3]) diffY += PLAYER_A;

  // Note floating point == 0 check, hopefully work in this case
  if (diffX && diffY) {
    diffX *= SQRT_HALF;
    diffY *= SQRT_HALF;
  }

  p->m_vX = (p->m_vX + diffX) * PLAYER_FRIC;
  p->m_vY = (p->m_vY + diffY) * PLAYER_FRIC;

  goalX += p->m_vX;
  goalY += p->m_vY;
*/

  if (m_pressed[0]) goalX -= 4;
  if (m_pressed[1]) goalX += 4;
  if (m_pressed[2]) goalY -= 4;
  if (m_pressed[3]) goalY += 4;


  //pd->system->logToConsole("GOAL %f %f CURRENT %f %f", goalX, goalY, p->m_x, p->m_y);

  int len;
  SpriteCollisionInfo* collInfo = pd->sprite->moveWithCollisions(p->m_sprite, goalX, goalY, &(p->m_x), &(p->m_y), &len);
  free(collInfo);

  pd->sprite->setZIndex(p->m_sprite, (int16_t)p->m_y);

  if      ((m_offX + p->m_x) > ((SCREEN_PIX_X / m_zoom) * SCROLL_EDGE))          m_offX = ((SCREEN_PIX_X / m_zoom) * SCROLL_EDGE) - p->m_x;
  else if ((m_offX + p->m_x) < ((SCREEN_PIX_X / m_zoom) * (1.0f - SCROLL_EDGE))) m_offX = ((SCREEN_PIX_X / m_zoom) * (1.0f - SCROLL_EDGE)) - p->m_x;

  if      ((m_offY + p->m_y) > ((SCREEN_PIX_Y / m_zoom) * SCROLL_EDGE))          m_offY = ((SCREEN_PIX_Y / m_zoom) * SCROLL_EDGE) - p->m_y;
  else if ((m_offY + p->m_y) < ((SCREEN_PIX_Y / m_zoom) * (1.0f - SCROLL_EDGE))) m_offY = ((SCREEN_PIX_Y / m_zoom) * (1.0f - SCROLL_EDGE)) - p->m_y;

  if (p->m_x > TOT_WORLD_PIX_X) {
    pd->sprite->moveTo(p->m_sprite, p->m_x - TOT_WORLD_PIX_X, p->m_y);
    pd->sprite->getPosition(p->m_sprite, &(p->m_x), &(p->m_y));
    m_offX += TOT_WORLD_PIX_X;
  } else if (p->m_x < 0) {
    pd->sprite->moveTo(p->m_sprite, p->m_x + TOT_WORLD_PIX_X, p->m_y);
    pd->sprite->getPosition(p->m_sprite, &(p->m_x), &(p->m_y));
    m_offX -= TOT_WORLD_PIX_X;
  }

  if (p->m_y > TOT_WORLD_PIX_Y) {
    pd->sprite->moveTo(p->m_sprite, p->m_x, p->m_y - TOT_WORLD_PIX_Y);
    pd->sprite->getPosition(p->m_sprite, &(p->m_x), &(p->m_y));
    m_offY += TOT_WORLD_PIX_Y;
  } else if (p->m_y < 0) {
    pd->sprite->moveTo(p->m_sprite, p->m_x, p->m_y + TOT_WORLD_PIX_Y);
    pd->sprite->getPosition(p->m_sprite, &(p->m_x), &(p->m_y));
    m_offY -= TOT_WORLD_PIX_Y;
  }
  //pd->system->logToConsole("OFF %f %f", m_offX, m_offY);

  //pd->system->logToConsole("CHX %f / %f = %f", p->m_x, (float)CHUNK_PIX_X, ((p->m_x)/((float)CHUNK_PIX_X)));


  // Check chunk change
  uint16_t chunkX = p->m_x / ((float)CHUNK_PIX_X);
  uint16_t chunkY = p->m_y / ((float)CHUNK_PIX_Y);

  uint8_t subChunkX = (uint16_t)(p->m_x / ((float)CHUNK_PIX_X/2.0f)) % 2;
  uint8_t subChunkY = (uint16_t)(p->m_y / ((float)CHUNK_PIX_Y/2.0f)) % 2;
  enum kChunkQuad quadrant = NW;
  if (subChunkX && subChunkY) {
    quadrant = SE;
  } else if (subChunkX) {
    quadrant = NE;
  } else if (subChunkY) {
    quadrant = SW;
  }

  if (chunkX != m_curChunkX || chunkY != m_curChunkY) {
    m_curChunkX = chunkX;
    m_curChunkY = chunkY;
    m_curSubChunkX = subChunkX;
    m_curSubChunkY = subChunkY;
    pd->system->logToConsole("CHUNKCHANGE %u %u (%u %u)", chunkX, chunkY, subChunkX, subChunkY);
  } else if (m_quadrant != quadrant) {
    m_quadrant = quadrant;
    pd->system->logToConsole("SUBCHUNKCHANGE (%u %u)", subChunkX, subChunkY);
    updateRenderListZoom(getChunk_noCheck(m_curChunkX, m_curChunkY), m_quadrant);
  }



  pd->graphics->setDrawOffset(m_offX, m_offY);

  return true;
}

void menuOptionsCallback(void* userdata) {

}

void initGame() {
  generate();
}

void deinitGame() {
}
