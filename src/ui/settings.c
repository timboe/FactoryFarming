#include "settings.h"
#include "new.h"
#include "../sprite.h"
#include "../ui.h"
#include "../io.h"
#include "../sound.h"

LCDBitmap* m_pause = NULL;

bool isTitle(int32_t _line);

const char* getLine(int32_t _line);

void checked(void);

void unchecked(void);

void autosave(uint32_t _time);

void musicvol(uint32_t _vol);

char* playTime(char* _buf128, uint32_t _playTime);

void addNumber(int32_t _n);

void addText(char* _t);

void cheatMoney(void);

void cheatUnlock(void);

/// ///

LCDBitmap* getPauseImage() {
  if (m_pause == NULL) m_pause = pd->graphics->newBitmap(DEVICE_PIX_X, DEVICE_PIX_Y, kColorClear);

  // Tutorial
  if (getTutorialStage() == kTutSeeObjective) {
    nextTutorialStage();
  }

  pd->graphics->clearBitmap(m_pause, kColorClear);
  int32_t length, width = 0;
  char textA[128] = {0}, textB[128] = {0}, textC[128] = {0}, textD[128] = {0}, textE[128] = {0};
  LCDBitmapFlip flip = kBitmapUnflipped;
  LCDBitmap* sA = NULL;
  LCDBitmap* sB = NULL;

  #define CENTRE (TILE_PIX*6 + TILE_PIX/4)
  #define Y_SPACE (3*TILE_PIX/2)
  #define OFFS 16

  #define X_START TILE_PIX
  #define X_END (11*TILE_PIX - TILE_PIX/2)

  if (getGameMode() == kTitles) return m_pause;

  pd->graphics->pushContext(m_pause);
  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 8*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 8*TILE_PIX, TILE_PIX, kColorWhite);
  pd->graphics->setDrawOffset(0, 0);

  setRoobert10();

  struct Player_t* p = getPlayer();
  const enum kNewReturnStatus nrs = checkHasNewToShow(p);
  if (nrs == kNewNoUnlockedAll) {

    snprintf(textA, 128, "%s", tr(kTRPauseFin0));
    snprintf(textB, 128, "%s", tr(kTRPauseFin1));
    snprintf(textC, 128, "%s", tr(kTRPauseFin2));
    snprintf(textD, 128, "%s", tr(kTRPauseFin3));
    sA = getSprite16(3, 19, 1);
    sB = getSprite16(3, 19, 1);

  } else if (nrs == kNewYes) {

    snprintf(textA, 128, "%s", tr(kTRPauseUnlock0));
    snprintf(textB, 128, "%s", tr(kTRPauseUnlock1));
    snprintf(textC, 128, "%s", tr(kTRPauseUnlockTheNext));
    sA = getSprite16(11, 14, 1);

    const uint32_t nextLevel = p->m_buildingsUnlockedTo + 1;
    enum kBuildingType nextBuildingType = UnlockDecs[nextLevel].type;
    enum kUICat nextBuildingUICat = getBuildingTypeCat(nextBuildingType);

    snprintf(textD, 128, "%s", toStringHeader(nextBuildingUICat, /*plural*/ false));
    sB = getSprite16(11, 13, 1);

  } else if (nrs == kNewNoNeedsFarming) {

    snprintf(textA, 128, "%s", tr(kTRPauseSell0));
    snprintf(textC, 128, "%s", tr(kTRPauseUnlockTheNext));

    const uint32_t nextLevel = p->m_buildingsUnlockedTo + 1;
    const enum kCargoType cargo = UnlockDecs[ nextLevel ].ofCargo;
    #ifdef FAST_PROGRESS
    const uint16_t needToSell = FAST_PROGRESS_SALES;
    #else
    const uint16_t needToSell = UnlockDecs[ nextLevel ].fromSelling;
    #endif
    const uint16_t haveSold = p->m_soldCargo[ cargo ];

    snprintf(textB, 128, "%i %s", needToSell, toStringCargoByType(cargo, /*plural=*/true));
    sA = getSprite16_byidx(CargoDesc[ cargo ].UIIcon, 1);
    flip = kBitmapFlippedX;

    enum kBuildingType nextBuildingType = UnlockDecs[nextLevel].type;
    enum kUICat nextBuildingUICat = getBuildingTypeCat(nextBuildingType);

    snprintf(textD, 128, "%s", toStringHeader(nextBuildingUICat, /*plural*/ false));
    sB = getSprite16(11, 13, 1);

    snprintf(textE, 128, tr(kTRPauseSell2), haveSold);
  
  } else if (nrs == kNewNoNeedsTutorial) {

    snprintf(textA, 128, "%s", tr(kTRPauseTut0));
    snprintf(textB, 128, "%s", tr(kTRPauseTut1));
    snprintf(textC, 128, "%s", tr(kTRPauseUnlockTheNext));
    snprintf(textD, 128, "%s", tr(kTRPauseTut3));
    sA = getSprite16(13, 13, 1);
    sB = getSprite16(11, 13, 1);

  }


  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kUTF8Encoding, CENTRE - width/2, 0*Y_SPACE+OFFS +tY());

  length = strlen(textB);
  width = pd->graphics->getTextWidth(getRoobert10(), textB, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textB, 128, kUTF8Encoding, CENTRE - width/2, 1*Y_SPACE+OFFS +tY());

  pd->graphics->drawBitmap(sA, X_START, 1*Y_SPACE+OFFS, kBitmapUnflipped);
  pd->graphics->drawBitmap(sA, X_END,   1*Y_SPACE+OFFS, flip);

  length = strlen(textC);
  width = pd->graphics->getTextWidth(getRoobert10(), textC, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textC, 128, kUTF8Encoding, CENTRE - width/2, 2*Y_SPACE+OFFS +tY());

  pd->graphics->drawBitmap(sB, X_START, 3*Y_SPACE+OFFS, kBitmapUnflipped);
  pd->graphics->drawBitmap(sB, X_END,   3*Y_SPACE+OFFS, kBitmapUnflipped);

  length = strlen(textD);
  width = pd->graphics->getTextWidth(getRoobert10(), textD, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textD, 128, kUTF8Encoding, CENTRE - width/2, 3*Y_SPACE+OFFS +tY());

  length = strlen(textE);
  width = pd->graphics->getTextWidth(getRoobert10(), textE, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textE, 128, kUTF8Encoding, CENTRE - width/2, 4*Y_SPACE+OFFS +tY());

  pd->graphics->drawLine(X_START + TILE_PIX, 2*Y_SPACE+OFFS - TILE_PIX/2, X_END, 2*Y_SPACE+OFFS - TILE_PIX/2, 2, kColorBlack);
  pd->graphics->drawLine(X_START + TILE_PIX, 4*Y_SPACE+OFFS - TILE_PIX/2, X_END, 4*Y_SPACE+OFFS - TILE_PIX/2, 2, kColorBlack);

  //

  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX*8 + TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 2*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 2*TILE_PIX, TILE_PIX, kColorWhite);
  pd->graphics->setDrawOffset(0, 0);
  
  snprintf(textA, 128, "%s", tr(kTRPauseSave));
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kUTF8Encoding, CENTRE - width/2, 6*Y_SPACE +tY());

  //

  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX*10 + TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 3*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 3*TILE_PIX, TILE_PIX, kColorWhite);
  pd->graphics->setDrawOffset(0, 0);

  snprintf(textA, 128, "%s", tr(kTRPauseTotTime));
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kUTF8Encoding, CENTRE - width/2, 8*Y_SPACE-OFFS +tY());

  const uint32_t pt = p->m_playTime / TICK_FREQUENCY;
  playTime(textA, pt);
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kUTF8Encoding, CENTRE - width/2, 9*Y_SPACE-OFFS - TILE_PIX/4 +tY());

  pd->graphics->popContext();
  return m_pause;
}

char* playTime(char* _buf128, uint32_t _playTime) {
  snprintf(_buf128, 128, tr(kTRPauseTotTimeDisplay), (int) _playTime/3600, (int) (_playTime%3600)/60, (int) (_playTime%3600)%60);
  return _buf128;
}

void checked() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(6, 19, 1), TILE_PIX*16, 0, kBitmapUnflipped);
}

void unchecked() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(5, 19, 1), TILE_PIX*16, 0, kBitmapUnflipped);
}

void addNumber(int32_t _n) {
  char text[128];
  snprintf(text, 128, "%i", (int) _n);
  int32_t length = strlen(text);
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), text, length, kUTF8Encoding, 0);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->fillRect(TILE_PIX*16 - width - TILE_PIX/4, 0, TILE_PIX + width + TILE_PIX/4, TILE_PIX, kColorWhite);
  pd->graphics->drawText(text, length, kUTF8Encoding, TILE_PIX*16 - width, tY());
}

void addText(char* _t) {
  int32_t length = strlen(_t);
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), _t, length, kUTF8Encoding, 0);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->fillRect(TILE_PIX*16 - width - TILE_PIX/4, 0, TILE_PIX + width + TILE_PIX/4, TILE_PIX, kColorWhite);
  pd->graphics->drawText(_t, length, kUTF8Encoding, TILE_PIX*16 - width, tY());
}

void autosave(uint32_t _time) {
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  switch (_time) {
    case 0: pd->graphics->drawText(tr(kTRSettingsAutosaveOff), 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 5: pd->graphics->drawText(tr(kTRSettingsAutosave5m), 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 10: pd->graphics->drawText(tr(kTRSettingsAutosave10m), 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 15: pd->graphics->drawText(tr(kTRSettingsAutosave15m), 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 30: pd->graphics->drawText(tr(kTRSettingsAutosave30m), 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
  }
}

void musicvol(uint32_t _vol) {
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  switch (_vol) {
    case 100: case 0: pd->graphics->drawText("100%", 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 75: pd->graphics->drawText("75%", 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 50: pd->graphics->drawText("50%", 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
    case 25: pd->graphics->drawText("25%", 8, kUTF8Encoding, TILE_PIX*15, tY()); break;
  }
  updateMusicVol();
}


void cheatMoney() {
#ifdef DEMO
  return;
#endif
  static uint8_t counter = 0;
  if (++counter == 20) {
    getPlayer()->m_infiniteMoney = !getPlayer()->m_infiniteMoney;
    counter = 0;
    sfx(kSfxUnlock);
  }
}

void cheatUnlock() {
#ifdef DEMO
  return;
#endif
  static uint8_t counter = 0;
  if (++counter == 20) {
    if (getPlayer()->m_buildingsUnlockedTo != getNUnlocks()) {
      getPlayer()->m_buildingsUnlockedTo = getNUnlocks();
    } else {
      getPlayer()->m_buildingsUnlockedTo = 0;
    }
    sfx(kSfxUnlock);
    counter = 0;
  }
}

void tutMoney(void) {
  if (getPlayer()->m_paidTutorialMoney) return; // Don't pay more than once
  getPlayer()->m_paidTutorialMoney = 1;
  int32_t toGive = EDesc[kCropHarvesterSmall].price * 2;
  toGive += CDesc[kBelt].price * 100;
  toGive += EDesc[kChalkQuarry].price * 2;
  toGive += FDesc[kVitiminFac].price * 2;
  #ifdef DEV
  pd->system->logToConsole("Disable tut give player %i", toGive);
  #endif
  modMoney( toGive );
}

void doSettings(bool _forward) {

  struct Player_t* p = getPlayer();
  const uint16_t selectedID =  getUIContentID();
  switch (selectedID) {
    case 1: p->m_soundSettings = p->m_soundSettings ^ 1; updateMusic(/*isTitle=*/false); break;
    case 2: 
          if (!_forward) {
            switch(p->m_musicVol) {
              case 100: case 0: p->m_musicVol = 75; break;
              case 75: p->m_musicVol = 50; break;
              case 50: p->m_musicVol = 25; break;
              case 25: p->m_musicVol = 100; break;
            }; 
          } else {
            switch(p->m_musicVol) {
              case 100: case 0: p->m_musicVol = 25; break;
              case 75: p->m_musicVol = 100; break;
              case 50: p->m_musicVol = 75; break;
              case 25: p->m_musicVol = 50; break;
            }; 
          }
          break;
    case 3: p->m_soundSettings = p->m_soundSettings ^ 2; updateSfx(); break;
    case 4: p->m_enableSteps = !p->m_enableSteps; break;
    case 6: p->m_enableScreenShake = !p->m_enableScreenShake; break;
    case 7: if (p->m_enableTutorial == 255) { p->m_enableTutorial = 0; } else { p->m_enableTutorial = 255; tutMoney(); } break;
    case 8: p->m_enableConveyorAnimation = !p->m_enableConveyorAnimation; break;
    case 9: p->m_enableExtractorOutlines = !p->m_enableExtractorOutlines; break;
    case 10: p->m_enableCrankConveyor = !p->m_enableCrankConveyor; break;
    case 11: p->m_enableCentreCamera = !p->m_enableCentreCamera; break;
    case 12: p->m_enableZoomWhenMove = !p->m_enableZoomWhenMove; break;
    case 13: 
            if (_forward) {
              switch(p->m_enableAutosave) {
                case 0: p->m_enableAutosave = 5; break;
                case 5: p->m_enableAutosave = 10; break;
                case 10: p->m_enableAutosave = 15; break;
                case 15: p->m_enableAutosave = 30; break;
                case 30: p->m_enableAutosave = 0; break;
              }; 
            } else {
              switch(p->m_enableAutosave) {
                case 0: p->m_enableAutosave = 30; break;
                case 5: p->m_enableAutosave = 0; break;
                case 10: p->m_enableAutosave = 5; break;
                case 15: p->m_enableAutosave = 10; break;
                case 30: p->m_enableAutosave = 15; break;
              }; 
            }
            break;
    case 14: p->m_enablePickupOnDestroy = !p->m_enablePickupOnDestroy; break;
    case 15: p->m_autoUseConveyorBooster = !p->m_autoUseConveyorBooster; break;
    case 16: p->m_enableDebug = !p->m_enableDebug;; break;
    //
    case 18: if (_forward) { doIO(kDoSave, /*and then*/ kDoTitle, /*and finally*/ kDoNothing); } break;
    case 19: if (_forward) { doIO(kDoTitle, /*and then*/ kDoNothing, /*and finally*/ kDoNothing); } break;
    //
    case 21: if (_forward) { doIO(kDoScreenShot, /*and then*/ kDoNothing, /*and finally*/ kDoNothing); } break;
    //
    case 28: cheatMoney(); break;
    case 29: cheatUnlock(); break;
    //
    case 38: chooseMusic(0); break;
    case 39: chooseMusic(1); break;
    case 40: chooseMusic(4); break;
    case 41: chooseMusic(3); break;
    case 42: chooseMusic(2); break;
    case 43: chooseMusic(5); break;
    //
    case 60: sfx(rand() % kNSFX); return; // Do not also play kSfxA
  }
  redrawSettingsMenuLine(getSettingsMenuUIBitmap(selectedID), selectedID);
  sfx(kSfxA);
}

// NOTE: There are defs in constants.h which might need updating too
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_START 0
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_STOP 17
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_START 62
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_STOP 69

void redrawAllSettingsMenuLines() {
  for (int32_t i = MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_START; i < MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_STOP; ++i) {
    redrawSettingsMenuLine(getSettingsMenuUIBitmap(i), i);
  }
  for (int32_t i = MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_START; i < MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_STOP; ++i) {
    redrawSettingsMenuLine(getSettingsMenuUIBitmap(i), i);
  }
}

void redrawSettingsMenuLine(LCDBitmap* _bitmap, int32_t _line) {

  setRoobert10();
  pd->graphics->clearBitmap(_bitmap, kColorWhite);
  pd->graphics->pushContext(_bitmap);
  pd->graphics->setDrawMode(kDrawModeCopy);
  
  int32_t width, length;
  if (isTitle(_line)) {
    const char* text = getLine(_line);
    length = strlen(text);
    width = pd->graphics->getTextWidth(getRoobert10(), text, length, kUTF8Encoding, 0);
    pd->graphics->drawText(text, length, kUTF8Encoding, TILE_PIX*9 - width/2, tY());

  } else {
    if (getLanguage() == kEN) {
      pd->graphics->drawText("..........................................................................", 75, kUTF8Encoding, TILE_PIX, tY());
    } else {
      pd->graphics->drawText("......................................................", 55, kUTF8Encoding, TILE_PIX, tY());
    }
    const char* text = getLine(_line);
    length = strlen(text);
    width = pd->graphics->getTextWidth(getRoobert10(), text, length, kUTF8Encoding, 0);
    pd->graphics->fillRect(0, 0, TILE_PIX + width + TILE_PIX/4, TILE_PIX, kColorWhite);
    pd->graphics->drawText(text, length, kUTF8Encoding, TILE_PIX, tY());
  }

  struct Player_t* p = getPlayer();
  char buf[128];
  switch (_line) {
    case 1: (p->m_soundSettings == 1 || p->m_soundSettings == 3) ? checked() : unchecked(); break;
    case 2: musicvol(p->m_musicVol); break;
    case 3: (p->m_soundSettings == 2 || p->m_soundSettings == 3) ? checked() : unchecked(); break;
    case 4: p->m_enableSteps ? checked() : unchecked(); break;

    case 6: p->m_enableScreenShake ? checked() : unchecked(); break;
    case 7: p->m_enableTutorial != 255 ? checked() : unchecked(); break;
    case 8: p->m_enableConveyorAnimation ? checked() : unchecked(); break;
    case 9: p->m_enableExtractorOutlines ? checked() : unchecked(); break;
    case 10: p->m_enableCrankConveyor ? checked() : unchecked(); break;
    case 11: p->m_enableCentreCamera ? checked() : unchecked(); break;
    case 12: p->m_enableZoomWhenMove ? checked() : unchecked(); break;
    case 13: autosave(p->m_enableAutosave); break;
    case 14: p->m_enablePickupOnDestroy ? checked() : unchecked(); break;
    case 15: p->m_autoUseConveyorBooster ? checked() : unchecked(); break;
    case 16: p->m_enableDebug ? checked() : unchecked(); break;
    // 
    case 62: addText(playTime(buf, p->m_playTime / TICK_FREQUENCY)); break;
    case 63: addNumber(p->m_moneyCumulative); break;
    case 64: addNumber(p->m_moneyHighWaterMark); break;
    case 65: addNumber(getTotalSoldCargo()); break;
    case 66: addNumber(getTotalImportedCargo()); break;
    case 67: addNumber(getNBuildings()); break;
    case 68: addNumber(getNCargo()); break;
  }

  pd->graphics->popContext();
}

void populateContentSettingsMenu() {
  for (int32_t i = 0; i < SETTINGS_TO_ROW; ++i) {
    setUIContentSettingsMenu(i, isTitle(i));
  }
}

void populateInfoSettingsMenu() {
}

bool isTitle(int32_t _line) {
  switch (_line) {
    case 0: case 5: case 17: case 20: case 22: case 33: case 37: 
    case 44: case 52: case 59: case 61: case 69: case 74: case 78: return true;
  }
  return false;
}

const char* getLine(int32_t _line) {
  if (_line >= CREDITS_TO_ROW) return " "; 
  return tr(kTRSettings0 + _line);
  // can currently go to 120
}
