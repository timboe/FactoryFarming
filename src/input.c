#include "input.h"
#include "location.h"
#include "player.h"
#include "sprite.h"
#include "render.h"
#include "building.h"
#include "ui.h"
#include "io.h"
#include "buildings/special.h"
#include "ui/mainmenu.h"
#include "ui/sell.h"
#include "ui/shop.h"
#include "ui/warp.h"
#include "ui/import.h"
#include "ui/export.h"
#include "ui/new.h"

uint8_t m_pressed[4] = {0};

uint8_t m_zoom = 1;

uint8_t m_pickRadius = 3;

bool characterMoveInput(uint32_t _buttonPressed);

void clickHandleWander(uint32_t _buttonPressed);

void clickHandleMenuBuy(uint32_t _buttonPressed);

void clickHandleMenuNew(uint32_t _buttonPressed);

void clickHandleMenuSell(uint32_t _buttonPressed);

void clickHandleMenuWarp(uint32_t _buttonPressed);

void clickHandleMenuWarp(uint32_t _buttonPressed);

void clickHandleMenuImport(uint32_t _buttonPressed);

void clickHandleMenuExport(uint32_t _buttonPressed);

void clickHandleMenuPlayer(uint32_t _buttonPressed);

void clickHandlePlacement(uint32_t _buttonPressed);

void clickHandlePick(uint32_t _buttonPressed);

void clickHandleInspect(uint32_t _buttonPressed);

void clickHandleDestroy(uint32_t _buttonPressed);

void clickHandleTitles(uint32_t _buttonPressed);

void rotateHandleWander(float _rotation);

void rotateHandlePlacement(float _rotation);

void rotateHandlePick(float _rotation);

void toggleZoom(void);

void modRadius(bool _inc);

bool m_b;

/// ///

void modRadius(bool _inc) {
  if (_inc) {
    m_pickRadius += 2;
    if (m_pickRadius == 7) m_pickRadius = 1;
  } else {
    if (m_pickRadius == 1) m_pickRadius = 7;
    m_pickRadius -= 2;
  }
  updateRenderList();
  updateBlueprint();
}

uint8_t getZoom() {
  return m_zoom;
}

uint8_t getRadius() {
 return m_pickRadius;
}

uint8_t getPressed(uint32_t _i) {
  return m_pressed[_i];
}

bool bPressed() {
  return m_b;
}

void unZoom() {
  m_zoom = 1;
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
    case kMenuNew:; return clickHandleMenuNew(_buttonPressed);
    case kMenuSell:; return clickHandleMenuSell(_buttonPressed);
    case kMenuPlayer:; return clickHandleMenuPlayer(_buttonPressed);
    case kMenuWarp:; return clickHandleMenuWarp(_buttonPressed);
    case kMenuExport:; return clickHandleMenuExport(_buttonPressed);
    case kMenuImport:; return clickHandleMenuImport(_buttonPressed);
    case kPlaceMode:; case kBuildMode:; case kPlantMode:; return clickHandlePlacement(_buttonPressed);
    case kPickMode:; return clickHandlePick(_buttonPressed);
    case kInspectMode:; return clickHandleInspect(_buttonPressed);
    case kDestroyMode:; return clickHandleDestroy(_buttonPressed);
    case kTitles:; return clickHandleTitles(_buttonPressed);
    case kNGameModes:; break;
  }
}

void clickHandleWander(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) { /*noop*/ }
  else if (kButtonA == _buttonPressed) {
    // 254: tutorial finised, 255: tutorial disabled
    const bool ic = isCamouflaged();
    if (getTutorialStage() < TUTORIAL_FINISHED && checkReturnDismissTutorialMsg()) { /*noop*/ } // NOTE: The second function call has side-effects
    else if (distanceFromBuy() < ACTIVATE_DISTANCE) { if (!checkShowNew()) setGameMode(kMenuBuy); }
    else if (distanceFromSell() < ACTIVATE_DISTANCE) setGameMode(kMenuSell);
    else if (!ic && distanceFromWarp() < ACTIVATE_DISTANCE) setGameMode(kMenuWarp);
    else if (!ic && distanceFromOut() < ACTIVATE_DISTANCE) setGameMode(kMenuExport);
    else if (!ic && distanceFromIn() < ACTIVATE_DISTANCE) setGameMode(kMenuImport);
    else setGameMode(kMenuPlayer);
  } else if (kButtonB == _buttonPressed) {
    if (getTutorialStage() < TUTORIAL_FINISHED) checkReturnDismissTutorialMsg();
  }

  //else if (kButtonB == _buttonPressed) toggleZoom(); // Press B to change zoom?
  // Hold down B to go faster?
}

void clickHandleTitles(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) {
    setSave(getTitleCursorSelected());
    if (hasSaveData(getTitleCursorSelected())) {
      setForceSlot(-1); // -1 loads from the slot stored in the player's save file
      doIO(kDoLoad, /*and then*/ kDoNothing);
    } else {
      setForceSlot(0); // This will be the ID of the world we generate
      doIO(kDoResetPlayer, /*and then*/ kDoNewWorld);
    }
  } else if (kButtonLeft == _buttonPressed) {
    modTitleCursor(false);
  } else if (kButtonRight == _buttonPressed) {
    modTitleCursor(true);
  }
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

void clickHandleMenuNew(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed || kButtonB == _buttonPressed) {
    if (!checkShowNew()) {
      setGameMode(kMenuBuy);
    }
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
  if (kButtonA     == _buttonPressed) doMainMenuClick();
  else if (kButtonB   == _buttonPressed) setGameMode(kWanderMode);
  else moveCursor(_buttonPressed);
}

void clickHandleMenuWarp(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) doWarp();
  else if (kButtonB == _buttonPressed) setGameMode(kWanderMode);
  else moveCursor(_buttonPressed);
}

void clickHandleMenuExport(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) doExport();
  else if (kButtonB == _buttonPressed) setGameMode(kWanderMode);
  else moveCursor(_buttonPressed);
}

void clickHandleMenuImport(uint32_t _buttonPressed) {
  if (kButtonA == _buttonPressed) doImport();
  else if (kButtonB == _buttonPressed) setGameMode(kWanderMode);
  else moveCursor(_buttonPressed);
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
  if (characterMoveInput(_buttonPressed)) { /*noop*/} 
  // Note: The tutorial and inspect boxes share the samme UI component
  else if (kButtonA == _buttonPressed) { checkReturnDismissTutorialMsg(); setGameMode(kWanderMode); }
  else if (kButtonB == _buttonPressed) { checkReturnDismissTutorialMsg(); setGameMode(kWanderMode); }
}

void clickHandleDestroy(uint32_t _buttonPressed) {
  if (characterMoveInput(_buttonPressed)) { /*noop*/ }
  else if (kButtonA == _buttonPressed) doDestroy();
  else if (kButtonB == _buttonPressed) setGameMode(kWanderMode);
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

void rotateHandlePick(float _rotation) {
  static float rot = 0.0f;
  rot += _rotation;
  if (rot > UI_ROTATE_ACTION) {
    rot = 0.0f;
    modRadius(true);
  } else if (rot < -UI_ROTATE_ACTION) {
    rot = 0.0f;
    modRadius(false);
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
  if (pushed & kButtonB) m_b = true;
  if (released & kButtonB) {
    gameClickConfigHandler(kButtonB);
    m_b = false;
  }
  if (released & kButtonA) {
    gameClickConfigHandler(kButtonA);
    multiClickCount = 8;
    multiClickNext = 8;
  } else if (current & kButtonA)  {
    if (gm == kPlaceMode || gm == kPickMode || gm == kPlantMode || gm == kDestroyMode) {
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

  switch (gm) {
    case kWanderMode:; rotateHandleWander(pd->system->getCrankChange()); break;
    case kMenuPlayer:; // fall through
    case kMenuNew:;
    case kBuildMode:;
    case kPlaceMode:; rotateHandlePlacement(pd->system->getCrankChange()); break;
    case kPickMode:; // fall through
    case kDestroyMode:; rotateHandlePick(pd->system->getCrankChange()); break;
    case kMenuBuy:; case kMenuSell:; case kMenuWarp:; case kMenuExport:; case kMenuImport:; break;
    case kPlantMode:; case kInspectMode:; case kTitles:; case kNGameModes:; break;
  }

}