#include "input.h"
#include "location.h"
#include "player.h"
#include "sprite.h"
#include "render.h"
#include "ui.h"

uint8_t m_pressed[4] = {0};



uint8_t m_zoom = 1;

bool characterMoveInput(uint32_t _buttonPressed);

void clickHandleWander(uint32_t _buttonPressed);

void clickHandleMenuSelect(uint32_t _buttonPressed);

void clickHandleMenuOptionSelected(uint32_t _buttonPressed);

void rotateHandleWander(float _rotation);

void rotateHandleMenuSelect(float _rotation);

void toggleZoom(void);

/// ///

uint8_t getZoom() {
  return m_zoom;
}

uint8_t getPressed(uint32_t _i) {
  return m_pressed[_i];
}

void toggleZoom() {
  switch (m_zoom) {
    case 1: m_zoom = 2; break;
    case 2: m_zoom = 4; break;
    case 4: m_zoom = 1; break;
  }
  updateRenderList();
  updateBlueprint();
}

bool characterMoveInput(uint32_t _buttonPressed) {
  if (kButtonLeft == _buttonPressed) m_pressed[0] = 1;
  else if (kButtonRight == _buttonPressed) m_pressed[1] = 1;
  else if (kButtonUp == _buttonPressed) m_pressed[2] = 1;
  else if (kButtonDown == _buttonPressed) m_pressed[3] = 1;
  else return false;
  return true;
}

void gameClickConfigHandler(uint32_t _buttonPressed) {
  switch (getGameMode()) {
    case kWander:; return clickHandleWander(_buttonPressed);
    case kMenuSelect:; return clickHandleMenuSelect(_buttonPressed);
    case kMenuOptionSelected:; return clickHandleMenuOptionSelected(_buttonPressed);
  }
}

void clickHandleWander(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {}
  else if (kButtonA == _buttonPressed) setGameMode(kMenuSelect);
  else if (kButtonB == _buttonPressed) toggleZoom();
}

void clickHandleMenuSelect(uint32_t _buttonPressed) {
  if      (kButtonUp    == _buttonPressed) modUISelectedID(false);
  else if (kButtonDown  == _buttonPressed) modUISelectedID(true);
  else if (kButtonLeft  == _buttonPressed) modUISelectedRotation(false);
  else if (kButtonRight == _buttonPressed) modUISelectedRotation(true);
  else if (kButtonA     == _buttonPressed) {
    setGameMode(kMenuOptionSelected);
    updateBlueprint();
  } else if (kButtonB     == _buttonPressed) {
    setGameMode(kWander);
  }
}

void clickHandleMenuOptionSelected(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {
    // noop
  } else if (kButtonA    == _buttonPressed) {
    switch (getUISelectedID()) {
      case kMenuConveyor:; newConveyor(getPlayerLocation(), getUISelectedRotation());
      case kMenuSplitI:;   break;
      case kMenuSplitL:;   break;
      case kMenuSplitT:;   break;     
    }
  } else if (kButtonB    == _buttonPressed) {
    setGameMode(kMenuSelect);
    updateBlueprint();
  }
}

void rotateHandleWander(float _rotation) {
  static float rot = 0.0f;
  rot += _rotation;
  if (rot > 260.0f) {
    rot = 0.0f;
    if (m_zoom < 4) {
      pd->system->logToConsole("ZOOM IN");
      m_zoom *= 2;
      updateRenderList();
      updateBlueprint();
    }
  } else if (rot < -260.0f) {
    rot = 0.0f;
    pd->system->logToConsole("ZOOM OUT");
    if (m_zoom > 1) {
      m_zoom /= 2;
      updateRenderList();
      updateBlueprint();
    }
  }
}

void rotateHandleMenuSelect(float _rotation) {
  static float rot = 0.0f;
  rot += _rotation;
  if (rot > 260.0f) {
    rot = 0.0f;
    modUISelectedRotation(true);
    if (getGameMode() == kMenuOptionSelected) updateBlueprint();
  } else if (rot < -260.0f) {
    rot = 0.0f;
    modUISelectedRotation(false);
    if (getGameMode() == kMenuOptionSelected) updateBlueprint();
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
  else if ((current & kButtonA) && getGameMode() == kMenuOptionSelected) {
    gameClickConfigHandler(kButtonA); // Special, allow placing rows of conveyors
  }

  if (released & kButtonLeft) m_pressed[0] = 0;
  if (released & kButtonRight) m_pressed[1] = 0;
  if (released & kButtonUp) m_pressed[2] = 0;
  if (released & kButtonDown) m_pressed[3] = 0;

  switch (getGameMode()) {
    case kWander:; rotateHandleWander(pd->system->getCrankChange()); break;
    case kMenuSelect:; // fall through
    case kMenuOptionSelected:; rotateHandleMenuSelect(pd->system->getCrankChange());
  }

}