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
float m_vX = 0, m_vY = 0;
float m_actualX = 0, m_actualY = 0;

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
      m_offX = m_actualX - (SCREEN_X / (m_zoom * 2)); // TODO this is wrong
      m_offY = m_actualY - (SCREEN_Y / (m_zoom * 2));
    }
  } else if (m_rot < -45.0f) {
    m_rot = 0.0f;
    if (m_zoom > 1) {
      m_zoom /= 2;
      m_offX = m_actualX - (SCREEN_X / (m_zoom * 2));
      m_offY = m_actualY - (SCREEN_Y / (m_zoom * 2));
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

  static char text[32];
  snprintf(text, 32, "Conveyors:%u", getNConveyors());
  setRoobert11();
  pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_SIZE - m_offX, 1*TILE_SIZE - m_offY);
  snprintf(text, 32, "Cargo:%u", getNCargo());
  pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_SIZE - m_offX, 2*TILE_SIZE - m_offY);
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


bool movePlayer() {
  LCDSprite* p = getPlayer();

  float goalX, goalY;
  pd->sprite->getPosition(p, &goalX, &goalY); // TODO Docs have int instead of float
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

  m_vX = (m_vX + diffX) * PLAYER_FRIC;
  m_vY = (m_vY + diffY) * PLAYER_FRIC;

  goalX += m_vX;
  goalY += m_vY;
*/

  if (m_pressed[0]) goalX -= 4;
  if (m_pressed[1]) goalX += 4;
  if (m_pressed[2]) goalY -= 4;
  if (m_pressed[3]) goalY += 4;

  int len;
  SpriteCollisionInfo* collInfo = pd->sprite->moveWithCollisions(p, goalX, goalY, &m_actualX, &m_actualY, &len);
  free(collInfo);

  pd->sprite->setZIndex(p, (int16_t)m_actualY);

  static bool disable = false;

  if      ((m_offX + m_actualX) > ((SCREEN_X / m_zoom) * SCROLL_EDGE))          m_offX = ((SCREEN_X / m_zoom) * SCROLL_EDGE) - m_actualX;
  else if ((m_offX + m_actualX) < ((SCREEN_X / m_zoom) * (1.0f - SCROLL_EDGE))) m_offX = ((SCREEN_X / m_zoom) * (1.0f - SCROLL_EDGE)) - m_actualX;

  if      ((m_offY + m_actualY) > ((SCREEN_Y / m_zoom) * SCROLL_EDGE))          m_offY = ((SCREEN_Y / m_zoom) * SCROLL_EDGE) - m_actualY;
  else if ((m_offY + m_actualY) < ((SCREEN_Y / m_zoom) * (1.0f - SCROLL_EDGE))) m_offY = ((SCREEN_Y / m_zoom) * (1.0f - SCROLL_EDGE)) - m_actualY;

  if (m_actualX > TOT_WORLD_X) {
    pd->sprite->moveTo(p, m_actualX - TOT_WORLD_X, m_actualY);
    m_offX += TOT_WORLD_X;
  } else if (m_actualX < 0) {
    pd->sprite->moveTo(p, m_actualX + TOT_WORLD_X, m_actualY);
    m_offX -= TOT_WORLD_X;
  }

  if (m_actualY > TOT_WORLD_Y) {
    pd->sprite->moveTo(p, m_actualX, m_actualY - TOT_WORLD_Y);
    m_offY += TOT_WORLD_Y;
  } else if (m_actualY < 0) {
    pd->sprite->moveTo(p, m_actualX, m_actualY + TOT_WORLD_Y);
    m_offY -= TOT_WORLD_Y;
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
