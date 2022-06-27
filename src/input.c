#include "input.h"
#include "location.h"
#include "player.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "ui.h"
#include "buildings/special.h"

uint8_t m_pressed[4] = {0};

uint8_t m_zoom = 1;

bool characterMoveInput(uint32_t _buttonPressed);

void clickHandleWander(uint32_t _buttonPressed);

void clickHandleMenuBuy(uint32_t _buttonPressed);

void clickHandleMenuPlayer(uint32_t _buttonPressed);

void clickHandlePlacement(uint32_t _buttonPressed);

void rotateHandleWander(float _rotation);

void rotateHandlePlacement(float _rotation);

void toggleZoom(void);

/// ///

uint8_t getZoom() {
  return m_zoom;
}

uint8_t getPressed(uint32_t _i) {
  return m_pressed[_i];
}

void toggleZoom() {
  if (++m_zoom == ZOOM_LEVELS) {
    m_zoom = 1;
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
    case kMenuBuy:; return clickHandleMenuBuy(_buttonPressed);
    case kMenuPlayer:; return clickHandleMenuPlayer(_buttonPressed);
    case kPlacement:; return clickHandlePlacement(_buttonPressed);
  }
}

void clickHandleWander(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {}
  else if (kButtonA == _buttonPressed) {
    if (distanceFromBuy() < ACTIVATE_DISTANCE) setGameMode(kMenuBuy);
    else setGameMode(kMenuPlayer);
  }
  //else if (kButtonB == _buttonPressed) toggleZoom();
}

void clickHandleMenuBuy(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) {
    doPurchace();
  } else if (kButtonB == _buttonPressed) {
    setGameMode(kWander);
  } else {
    moveCursor(_buttonPressed);
  }
}

void clickHandleMenuPlayer(uint32_t _buttonPressed) {
  if (kButtonA     == _buttonPressed) {
    //setGameMode(kPlacement);
    //updateBlueprint();
  } else if (kButtonB   == _buttonPressed) {
    setGameMode(kWander);
  } else {
    moveCursor(_buttonPressed);
  }
}

void clickHandlePlacement(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {
    // noop
  } else if (kButtonA    == _buttonPressed) {
    /*
    switch (getUISelectedID()) {
      case kMenuConveyor:; newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kBelt}   ); break;
      case kMenuSplitI:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kSplitI} ); break;
      case kMenuSplitL:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kSplitL} ); break;
      case kMenuSplitT:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kSplitT} ); break;
      case kMenuFilterL:;  newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kFilterL} ); break;
      case kMenuTunnel:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kConveyor, (union kSubType) {.conveyor = kTunnelIn} ); break;
      case kMenuApple:;    newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kPlant, (union kSubType) {.plant = kAppleTree} ); break;
      case kMenuCarrot:;   newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kPlant, (union kSubType) {.plant = kCarrotPlant} ); break;
      case kMenuWheat:;    newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kPlant, (union kSubType) {.plant = kWheatPlant} ); break;
      case kMenuCheese:;   newCargo(getPlayerLookingAtLocation(), kCheese, true); break;
      case kMenuExtractor:;newBuilding(getPlayerLookingAtLocation(), getUISelectedRotation(), kExtractor, (union kSubType) {.extractor = kCropHarvester} ); break;
      case kMenuBin:;      clearLocation(getPlayerLookingAtLocation(), true, true); break; 
    }
    */
  } else if (kButtonB    == _buttonPressed) {
    setGameMode(kWander);
    updateBlueprint();
  }
}

void rotateHandleWander(float _rotation) {
  static float rot = 0.0f;
  rot += _rotation;
  if (rot > UI_ROTATE_ACTION) {
    rot = 0.0f;
    if (++m_zoom == ZOOM_LEVELS) m_zoom = ZOOM_LEVELS-1;
    pd->system->logToConsole("ZOOM IN");
    updateRenderList();
    updateBlueprint();
  } else if (rot < -UI_ROTATE_ACTION) {
    rot = 0.0f;
    pd->system->logToConsole("ZOOM OUT");
    if (--m_zoom == 0) m_zoom = 1;
    updateRenderList();
    updateBlueprint();
  }
}

void rotateHandlePlacement(float _rotation) {
  static float rot = 0.0f;
  rot += _rotation;
  if (rot > UI_ROTATE_ACTION) {
    rot = 0.0f;
    rotateCursor(true);
    if (getGameMode() == kPlacement) updateBlueprint();
  } else if (rot < -UI_ROTATE_ACTION) {
    rot = 0.0f;
    rotateCursor(false);
    if (getGameMode() == kPlacement) updateBlueprint();
  }
}

// Temporary until the playdate eventHandler is functional for inputs
void clickHandlerReplacement() {
  static uint8_t multiClickCount = 16, multiClickNext = 16;
  enum kGameMode gm = getGameMode();
  PDButtons current, pushed, released = 0;
  pd->system->getButtonState(&current, &pushed, &released);
  if (pushed & kButtonUp) gameClickConfigHandler(kButtonUp);
  if (pushed & kButtonRight) gameClickConfigHandler(kButtonRight);
  if (pushed & kButtonDown) gameClickConfigHandler(kButtonDown);
  if (pushed & kButtonLeft) gameClickConfigHandler(kButtonLeft);
  if (released & kButtonB) gameClickConfigHandler(kButtonB);
  if (released & kButtonA) gameClickConfigHandler(kButtonA);
  else if (current & kButtonA)  {
    if (gm == kPlacement) {
      gameClickConfigHandler(kButtonA); // Special, allow placing rows of conveyors
    } else if (gm >= kMenuBuy) {
      if (--multiClickCount == 0) {
        gameClickConfigHandler(kButtonA); // Special, allow speed buying/selling
        if (multiClickNext > 2) --multiClickNext;
        multiClickCount = multiClickNext;
      }
    }
  }

  if (released & kButtonLeft) m_pressed[0] = 0;
  if (released & kButtonRight) m_pressed[1] = 0;
  if (released & kButtonUp) m_pressed[2] = 0;
  if (released & kButtonDown) m_pressed[3] = 0;
  if (released & kButtonA) {
    multiClickCount = 16;
    multiClickNext = 16;
  }

  switch (gm) {
    case kWander:; rotateHandleWander(pd->system->getCrankChange()); break;
    case kMenuPlayer:; // fall through
    case kPlacement:; rotateHandlePlacement(pd->system->getCrankChange()); break;
    case kMenuBuy:; break;
  }

}