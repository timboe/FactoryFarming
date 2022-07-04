#include "input.h"
#include "location.h"
#include "player.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "ui.h"
#include "buildings/special.h"
#include "ui/mainmenu.h"
#include "ui/sell.h"
#include "ui/shop.h"

uint8_t m_pressed[4] = {0};

uint8_t m_zoom = 1;

bool characterMoveInput(uint32_t _buttonPressed);

void clickHandleWander(uint32_t _buttonPressed);

void clickHandleMenuBuy(uint32_t _buttonPressed);

void clickHandleMenuSell(uint32_t _buttonPressed);

void clickHandleMenuPlayer(uint32_t _buttonPressed);

void clickHandlePlacement(uint32_t _buttonPressed);

void clickHandlePick(uint32_t _buttonPressed);

void clickHandleInspect(uint32_t _buttonPressed);

void clickHandleDestroy(uint32_t _buttonPressed);

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
    case kWanderMode:; return clickHandleWander(_buttonPressed);
    case kMenuBuy:; return clickHandleMenuBuy(_buttonPressed);
    case kMenuSell:; return clickHandleMenuSell(_buttonPressed);
    case kMenuPlayer:; return clickHandleMenuPlayer(_buttonPressed);
    case kPlaceMode:; case kBuildMode:; case kPlantMode:; return clickHandlePlacement(_buttonPressed);
    case kPickMode:; return clickHandlePick(_buttonPressed);
    case kInspectMode:; return clickHandleInspect(_buttonPressed);
    case kDestroyMode:; return clickHandleDestroy(_buttonPressed);
    case kNGameModes:; break;
  }
}

void clickHandleWander(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) { /*noop*/ }
  else if (kButtonA == _buttonPressed) {
    // 254: tutorial finised, 255: tutorial disabled
    if (getTutorialStage() < 254 && checkReturnDismissTutorialMsg()) { /*noop*/ } // NOTE: The second function call has side-effects
    else if (distanceFromBuy() < ACTIVATE_DISTANCE) setGameMode(kMenuBuy);
    else if (distanceFromSell() < ACTIVATE_DISTANCE) setGameMode(kMenuSell);
    else setGameMode(kMenuPlayer);
  }
  //else if (kButtonB == _buttonPressed) toggleZoom(); // Press B to change zoom?
  // Hold down B to go faster?
}

void clickHandleMenuBuy(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) {
    doPurchace();
  } else if (kButtonB == _buttonPressed) {
    setGameMode(kWanderMode);
    // Tutorial
    if (getTutorialStage() == kTutWelcomeBuySeeds && getTutorialProgress() >= 10) {
      nextTutorialStage();
    }
  } else {
    moveCursor(_buttonPressed);
  }
}

void clickHandleMenuSell(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) {
    doSale();
  } else if (kButtonB == _buttonPressed) {
    setGameMode(kWanderMode);
    // Tutorial
    if (getTutorialStage() == kTutSellCarrots && getTutorialProgress() >= 10) {
      nextTutorialStage();
    }
  } else {
    moveCursor(_buttonPressed);
  }
}

void clickHandleMenuPlayer(uint32_t _buttonPressed) {
  if (kButtonA     == _buttonPressed) {
    doMainMenuClick();
  } else if (kButtonB   == _buttonPressed) {
    setGameMode(kWanderMode);
  } else {
    moveCursor(_buttonPressed);
  }
}

void clickHandlePlacement(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {
    // noop
  } else if (kButtonA    == _buttonPressed) {
    doPlace();
  } else if (kButtonB    == _buttonPressed) {
    setGameMode(kWanderMode);
    updateBlueprint();
    // Tutorial
    if (getTutorialStage() == kTutPlantCarrots && getTutorialProgress() >= 10) {
      nextTutorialStage();
    }
    // Tutorial
    if (getTutorialStage() == kTutBuildHarvester && getTutorialProgress()) {
      nextTutorialStage();
    }
    // Tutorial 
    if (getTutorialStage() == kTutBuildQuarry && getTutorialProgress()) {
      nextTutorialStage();
    }
  }
}

void clickHandlePick(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {
    // noop
  } else if (kButtonA    == _buttonPressed) {
    doPick();
  } else if (kButtonB    == _buttonPressed) {
    setGameMode(kWanderMode);
    // Tutorial
    if (getTutorialStage() == kTutGetCarrots && getTutorialProgress() >= 10) {
      nextTutorialStage();
    }
  }
}

void clickHandleInspect(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {
    // noop
  } else if (kButtonA    == _buttonPressed) {
    // noop
  } else if (kButtonB    == _buttonPressed) {
    setGameMode(kWanderMode);
  }
}

void clickHandleDestroy(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) {
    // noop
  } else if (kButtonA    == _buttonPressed) {
    doDestroy();
  } else if (kButtonB    == _buttonPressed) {
    setGameMode(kWanderMode);
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
  } else if (rot < -UI_ROTATE_ACTION) {
    rot = 0.0f;
    rotateCursor(false);
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
    if (gm == kPlaceMode || gm == kPickMode || gm == kPlantMode) {
      gameClickConfigHandler(kButtonA); // Special, allow pick/placing rows of conveyors
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
    case kWanderMode:; rotateHandleWander(pd->system->getCrankChange()); break;
    case kMenuPlayer:; // fall through
    case kBuildMode:;
    case kPlaceMode:; rotateHandlePlacement(pd->system->getCrankChange()); break;
    case kMenuBuy:; case kMenuSell:; case kPickMode:; case kPlantMode:; case kDestroyMode:; case kInspectMode:; case kNGameModes:; break;
  }

}