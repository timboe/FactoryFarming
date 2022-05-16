#include <math.h>

#include "game.h"
#include "sprite.h"
#include "generate.h"
#include "render.h"
#include "sound.h"


PlaydateAPI* pd = NULL;

int32_t m_frameCount = 0;

int getFrameCount() { 
  return m_frameCount; 
}


void setPDPtr(PlaydateAPI* _p) {
  pd = _p;
}

void gameClickConfigHandler(uint32_t _buttonPressed) {
  //if (kButtonUp == buttonPressed)

}


// Temporary until the playdate eventHandler is functional for inputs
void clickHandlerReplacement() {
  PDButtons current, pushed, released = 0;
  pd->system->getButtonState(&current, &pushed, &released);
  if (pushed & kButtonUp) gameClickConfigHandler(kButtonUp);
  if (pushed & kButtonRight) gameClickConfigHandler(kButtonRight);
  if (pushed & kButtonDown) gameClickConfigHandler(kButtonDown);

}

void render() {
  pd->display->setScale(1);


  // Draw FPS indicator (dbg only)
  #ifdef DEBUG_MODE
  pd->system->drawFPS(0, 0);
  #endif
}



int gameLoop(void* _data) {

  clickHandlerReplacement();

  if (++m_frameCount == ANIM_FPS) m_frameCount = 0;
  bool requestRedraw = true;
  

  if (requestRedraw) {
    render();
  }

  // TODO: The docs say that returning 0 does not request a redraw,
  // but it is currently required
  requestRedraw = true;

  return (int)requestRedraw;
}


bool movePlayer() {
  return true;
}

void menuOptionsCallback(void* userdata) {

}

void initGame() {
}

void deinitGame() {
}
