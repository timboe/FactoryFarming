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

    snprintf(textA, 128, "Everything is");
    snprintf(textB, 128, "UNLOCKED!");
    snprintf(textC, 128, "Thanks for playing");
    snprintf(textD, 128, "FACTORY FARMING!");
    sA = getSprite16(3, 19, 1);
    sB = getSprite16(3, 19, 1);

  } else if (nrs == kNewYes) {

    snprintf(textA, 128, "Visit:");
    snprintf(textB, 128, "The Shop");
    snprintf(textC, 128, "To unlock the next:");
    sA = getSprite16(11, 14, 1);

    const uint32_t nextLevel = p->m_buildingsUnlockedTo + 1;
    enum kBuildingType nextBuildingType = UnlockDecs[nextLevel].type;
    enum kUICat nextBuildingUICat = getBuildingTypeCat(nextBuildingType);

    snprintf(textD, 128, "%s", toStringHeader(nextBuildingUICat, /*plural*/ false));
    sB = getSprite16(11, 13, 1);

  } else if (nrs == kNewNoNeedsFarming) {

    snprintf(textA, 128, "Sell a total of:");
    snprintf(textC, 128, "To unlock the next:");

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

    snprintf(textE, 128, "Sold so far: %i", haveSold);
  
  } else if (nrs == kNewNoNeedsTutorial) {

    snprintf(textA, 128, "Continue");
    snprintf(textB, 128, "The Tutorial");
    snprintf(textC, 128, "To unlock the next");
    snprintf(textD, 128, "Building");
    sA = getSprite16(13, 13, 1);
    sB = getSprite16(11, 13, 1);

  }


  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 0*Y_SPACE+OFFS);

  length = strlen(textB);
  width = pd->graphics->getTextWidth(getRoobert10(), textB, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, CENTRE - width/2, 1*Y_SPACE+OFFS);

  pd->graphics->drawBitmap(sA, X_START, 1*Y_SPACE, kBitmapUnflipped);
  pd->graphics->drawBitmap(sA, X_END,   1*Y_SPACE, flip);

  length = strlen(textC);
  width = pd->graphics->getTextWidth(getRoobert10(), textC, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, CENTRE - width/2, 2*Y_SPACE+OFFS);

  pd->graphics->drawBitmap(sB, X_START, 3*Y_SPACE, kBitmapUnflipped);
  pd->graphics->drawBitmap(sB, X_END, 3*Y_SPACE, kBitmapUnflipped);

  length = strlen(textD);
  width = pd->graphics->getTextWidth(getRoobert10(), textD, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textD, 128, kASCIIEncoding, CENTRE - width/2, 3*Y_SPACE+OFFS);

  length = strlen(textE);
  width = pd->graphics->getTextWidth(getRoobert10(), textE, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textE, 128, kASCIIEncoding, CENTRE - width/2, 4*Y_SPACE+OFFS);

  pd->graphics->drawLine(X_START + TILE_PIX, 2*Y_SPACE+OFFS - TILE_PIX/2, X_END, 2*Y_SPACE+OFFS - TILE_PIX/2, 2, kColorBlack);
  pd->graphics->drawLine(X_START + TILE_PIX, 4*Y_SPACE+OFFS - TILE_PIX/2, X_END, 4*Y_SPACE+OFFS - TILE_PIX/2, 2, kColorBlack);

  //

  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX*8 + TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 2*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 2*TILE_PIX, TILE_PIX, kColorWhite);
  pd->graphics->setDrawOffset(0, 0);
  
  snprintf(textA, 128, "SAVE BEFORE EXITING!");
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 6*Y_SPACE);

  //

  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX*10 + TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 3*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 3*TILE_PIX, TILE_PIX, kColorWhite);
  pd->graphics->setDrawOffset(0, 0);

  snprintf(textA, 128, "Total Play Time:");
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 8*Y_SPACE-OFFS);

  const uint32_t pt = p->m_playTime / TICK_FREQUENCY;
  playTime(textA, pt);
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 9*Y_SPACE-OFFS - TILE_PIX/4);

  pd->graphics->popContext();
  return m_pause;
}

char* playTime(char* _buf128, uint32_t _playTime) {
  snprintf(_buf128, 128, "%ih:%im:%is", (int) _playTime/3600, (int) (_playTime%3600)/60, (int) (_playTime%3600)%60);
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
  pd->graphics->drawText(text, length, kUTF8Encoding, TILE_PIX*16 - width, 0);
}

void addText(char* _t) {
  int32_t length = strlen(_t);
  int32_t width = pd->graphics->getTextWidth(getRoobert10(), _t, length, kUTF8Encoding, 0);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->fillRect(TILE_PIX*16 - width - TILE_PIX/4, 0, TILE_PIX + width + TILE_PIX/4, TILE_PIX, kColorWhite);
  pd->graphics->drawText(_t, length, kUTF8Encoding, TILE_PIX*16 - width, 0);
}

void autosave(uint32_t _time) {
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  switch (_time) {
    case 0: pd->graphics->drawText("OFF", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 5: pd->graphics->drawText("5m", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 10: pd->graphics->drawText("10m", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 15: pd->graphics->drawText("15m", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 30: pd->graphics->drawText("30m", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
  }
}

void musicvol(uint32_t _vol) {
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  switch (_vol) {
    case 100: case 0: pd->graphics->drawText("100%", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 75: pd->graphics->drawText("75%", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 50: pd->graphics->drawText("50%", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
    case 25: pd->graphics->drawText("25%", 8, kUTF8Encoding, TILE_PIX*15, 0); break;
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
  }
}

void doSettings() {
  struct Player_t* p = getPlayer();
  const uint16_t selectedID =  getUIContentID();
  switch (selectedID) {
    case 1: p->m_soundSettings = p->m_soundSettings ^ 1; updateMusic(/*isTitle=*/false); break;
    case 2: switch(p->m_musicVol) {
              case 100: case 0: p->m_musicVol = 75; break;
              case 75: p->m_musicVol = 50; break;
              case 50: p->m_musicVol = 25; break;
              case 25: p->m_musicVol = 100; break;
            }; break;
    case 3: p->m_soundSettings = p->m_soundSettings ^ 2; updateSfx(); break;
    case 4: p->m_enableSteps = !p->m_enableSteps; break;
    case 6: p->m_enableScreenShake = !p->m_enableScreenShake; break;
    case 7: if (p->m_enableTutorial == 255) p->m_enableTutorial = 0; else p->m_enableTutorial = 255; break;
    case 8: p->m_enableConveyorAnimation = !p->m_enableConveyorAnimation; break;
    case 9: p->m_enableExtractorOutlines = !p->m_enableExtractorOutlines; break;
    case 10: switch(p->m_enableAutosave) {
              case 0: p->m_enableAutosave = 5; break;
              case 5: p->m_enableAutosave = 10; break;
              case 10: p->m_enableAutosave = 15; break;
              case 15: p->m_enableAutosave = 30; break;
              case 30: p->m_enableAutosave = 0; break;
            }; break;
    case 11: p->m_enablePickupOnDestroy = !p->m_enablePickupOnDestroy; break;
    case 12: p->m_autoUseConveyorBooster = !p->m_autoUseConveyorBooster; break;
    case 13: p->m_enableDebug = !p->m_enableDebug;; break;
    //
    case 15: doIO(kDoSave, /*and then*/ kDoTitle, /*and finally*/ kDoNothing); break;
    case 16: doIO(kDoTitle, /*and then*/ kDoNothing, /*and finally*/ kDoNothing); break;
    //
    case 18: doIO(kDoScreenShot, /*and then*/ kDoNothing, /*and finally*/ kDoNothing); break;
    //
    case 25: cheatMoney(); break;
    case 26: cheatUnlock(); break;
    //
    case 33: chooseMusic(0); break;
    case 34: chooseMusic(1); break;
    case 35: chooseMusic(4); break;
    case 36: chooseMusic(3); break;
    case 37: chooseMusic(2); break;
    case 38: chooseMusic(5); break;
    //
    case 53: sfx(rand() % kNSFX); break;
  }
  redrawSettingsMenuLine(getSettingsMenuUIBitmap(selectedID), selectedID);
}

#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_START 0
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_STOP 14
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_START 55
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_STOP 62

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
    pd->graphics->drawText(text, length, kUTF8Encoding, TILE_PIX*9 - width/2, 0);

  } else {
    pd->graphics->drawText("..........................................................................", 75, kASCIIEncoding, TILE_PIX, 0);
    const char* text = getLine(_line);
    length = strlen(text);
    width = pd->graphics->getTextWidth(getRoobert10(), text, length, kUTF8Encoding, 0);
    pd->graphics->fillRect(0, 0, TILE_PIX + width + TILE_PIX/4, TILE_PIX, kColorWhite);
    pd->graphics->drawText(text, length, kUTF8Encoding, TILE_PIX, 0);
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
    case 10: autosave(p->m_enableAutosave); break;
    case 11: p->m_enablePickupOnDestroy ? checked() : unchecked(); break;
    case 12: p->m_autoUseConveyorBooster ? checked() : unchecked(); break;
    case 13: p->m_enableDebug ? checked() : unchecked(); break;
    // 
    case 55: addText(playTime(buf, p->m_playTime / TICK_FREQUENCY)); break;
    case 56: addNumber(p->m_moneyCumulative); break;
    case 57: addNumber(p->m_moneyHighWaterMark); break;
    case 58: addNumber(getTotalSoldCargo()); break;
    case 59: addNumber(getTotalImportedCargo()); break;
    case 60: addNumber(getNBuildings()); break;
    case 61: addNumber(getNCargo()); break;
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
    case 0: case 5: case 14: case 17: case 19: case 30: case 32: case 39: case 47: case 52: case 54: case 62: case 67: case 71: return true;
  }
  return false;
}

const char* getLine(int32_t _line) {
  switch (_line) {
    case 0: return "--- Sound Settings ---";
    case 1: return "Music";
    case 2: return "Music Volume";
    case 3: return "Sound Effects";
    case 4: return "Footsteps";
    //
    case 5: return "--- Game Settings ---";
    case 6: return "Screen Shake";
    case 7: return "Tutorial Enabled";
    case 8: return "Conveyor Animation";
    case 9: return "Show Harvester Outlines";
    case 10: return "Auto Save Time";
    case 11: return "Auto Pickup on Destroy";
    case 12: return "Auto Apply Conveyor Grease";
    case 13: return "Show Debug Information";
    //
    case 14: return "--- Exit Game ---";
    case 15: return "Save & Exit to Title";
    case 16: return "Exit to Title WITHOUT Saving";
    //
    case 17: return "--- Screenshot ---";
    case 18: return "Take Screenshot of Plot";
    //
    case 19: return "--- Controls ---";
    case 20: return "Inventory/Select/Interact: Ⓐ";
    case 21: return "Back/Cancel: Ⓑ";
    case 22: return "Walk: ✛";
    case 23: return "Run: Hold Ⓑ + ✛";
    case 24: return "Zoom in/out: 🎣 or Hold Ⓐ + ⬇️";
    case 25: return "Rotate: 🎣 or Hold Ⓑ + ⬆️/➡️/⬇️/⬅️";
    case 26: return "Resize: 🎣 or Hold Ⓑ + ⬆️/⬇️";
    case 27: return "Quick Pickup Mode: Hold Ⓐ + ⬅️";
    case 28: return "Quick Inspect Mode: Hold Ⓐ + ⬆️";
    case 29: return "Quick Demolition Mode: Hold Ⓐ + ➡️";
    //
    case 30: return "--- Credits ---";
    case 31: return "Factory Farming by Tim Martin";
    //
    case 32: return "-- Music --";
    case 33: return "♬ Dr Tikov: 1985";
    case 34: return "♬ BoxCat Games: B-3";
    case 35: return "♬ Eric Skiff: We're the Resistors";
    case 36: return "♬ RoccoW: Sweet Self Satisfaction";
    case 37: return "♬ RoccoW: Weeklybeats 2014 #4";
    case 38: return "♬ Soft & Furious: Horizon Ending";
    //
    case 39: return "-- Art --";
    case 40: return "Kenney: 1-Bit Pack";
    case 41: return "VectorPixelStar: 1-Bit Patterns";
    case 42: return "Josehzz: Farming Crops";
    case 43: return "ScratchIO: Farming Set";
    case 44: return "Varkalandar: Isometric Rocks";
    case 45: return "Withthelove: Character Sprites";
    case 46: return "DinosoftLab: New (NounProject)";
    //
    case 47: return "-- Fonts --";
    case 48: return "Chester Jenkins: Cooper Hewitt";
    case 49: return "Martin Vacha: Roobert";
    case 50: return "Mediengestaltung: Messing Lettern";
    case 51: return "Nick's Fonts: Coventry Garden";
    //
    case 52: return "-- Sound Effects --";
    case 53: return "sfxr.me";
    //
    case 54: return "-- Statistics --";
    case 55: return "Play Time:";
    case 56: return "Total Money Earned:";
    case 57: return "Most Money Held:";
    case 58: return "Total Cargo Sold:";
    case 59: return "Total Cargo Imported:";
    case 60: return "Total Buildings in this Plot:";
    case 61: return "Total Cargo in this Plot:";
    //
    case 62: return " ";
    case 63: return "© Tim Martin 2022";
    case 64: return "Factory Farming is an MIT";
    case 65: return "Licensed Open Source Project";
    case 66: return "github.com/timboe/FactoryFarming";
    //
    case 67: return " ";
    case 68: return "Thank you for playing!";
    case 69: return "And for your support of niche";
    case 70: return "Indy game dev projects :)";
    //
    case 71: return " ";
    case 72: return "Keep on Maximizing Profit,";
    case 73: return "Maximizing Efficiency.";
    // can currently go to 120
  }
  return "???";
}
