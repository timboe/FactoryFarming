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

  #define X_START TILE_PIX
  #define X_END (11*TILE_PIX - TILE_PIX/2)

  if (getGameMode() == kTitles) return m_pause;

  pd->graphics->pushContext(m_pause);
  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 9*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 9*TILE_PIX, TILE_PIX, kColorWhite);
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
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 1*Y_SPACE);

  length = strlen(textB);
  width = pd->graphics->getTextWidth(getRoobert10(), textB, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, CENTRE - width/2, 2*Y_SPACE);

  pd->graphics->drawBitmap(sA, X_START, 2*Y_SPACE, kBitmapUnflipped);
  pd->graphics->drawBitmap(sA, X_END,   2*Y_SPACE, flip);

  length = strlen(textC);
  width = pd->graphics->getTextWidth(getRoobert10(), textC, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, CENTRE - width/2, 3*Y_SPACE);

  pd->graphics->drawBitmap(sB, X_START, 4*Y_SPACE, kBitmapUnflipped);
  pd->graphics->drawBitmap(sB, X_END, 4*Y_SPACE, kBitmapUnflipped);

  length = strlen(textD);
  width = pd->graphics->getTextWidth(getRoobert10(), textD, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textD, 128, kASCIIEncoding, CENTRE - width/2, 4*Y_SPACE);

  length = strlen(textE);
  width = pd->graphics->getTextWidth(getRoobert10(), textE, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textE, 128, kASCIIEncoding, CENTRE - width/2, 5*Y_SPACE);

  pd->graphics->drawLine(X_START + TILE_PIX, 3*Y_SPACE - TILE_PIX/2, X_END, 3*Y_SPACE - TILE_PIX/2, 2, kColorBlack);
  pd->graphics->drawLine(X_START + TILE_PIX, 5*Y_SPACE - TILE_PIX/2, X_END, 5*Y_SPACE - TILE_PIX/2, 2, kColorBlack);

  pd->graphics->setDrawOffset(TILE_PIX/2, TILE_PIX*9 + TILE_PIX/2);
  roundedRect(0, DEVICE_PIX_X/2 - TILE_PIX, 4*TILE_PIX, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX/4, DEVICE_PIX_X/2 - TILE_PIX, 4*TILE_PIX, TILE_PIX, kColorWhite);
  pd->graphics->setDrawOffset(0, 0);

  snprintf(textA, 128, "Total Play Time:");
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 7*Y_SPACE);

  const uint32_t pt = p->m_playTime / TICK_FREQUENCY;
  playTime(textA, pt);
  length = strlen(textA);
  width = pd->graphics->getTextWidth(getRoobert10(), textA, length, kUTF8Encoding, 0);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, CENTRE - width/2, 8*Y_SPACE - TILE_PIX/4);

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
    case 5: p->m_enableScreenShake = !p->m_enableScreenShake; break;
    case 6: if (p->m_enableTutorial == 255) p->m_enableTutorial = 0; else p->m_enableTutorial = 255; break;
    case 7: p->m_enableConveyorAnimation = !p->m_enableConveyorAnimation; break;
    case 8: switch(p->m_enableAutosave) {
              case 0: p->m_enableAutosave = 5; break;
              case 5: p->m_enableAutosave = 10; break;
              case 10: p->m_enableAutosave = 15; break;
              case 15: p->m_enableAutosave = 30; break;
              case 30: p->m_enableAutosave = 0; break;
            }; break;
    case 9: p->m_enablePickupOnDestroy = !p->m_enablePickupOnDestroy; break;
    case 10: p->m_autoUseConveyorBooster = !p->m_autoUseConveyorBooster; break;
    case 11: p->m_enableDebug = !p->m_enableDebug;; break;
    case 13: doIO(kDoSave, /*and then*/ kDoTitle); break;
    case 14: doIO(kDoTitle, /*and then*/ kDoNothing); break;
    //
    case 21: cheatMoney(); break;
    case 22: cheatUnlock(); break;
    //
    case 26: chooseMusic(0); break;
    case 27: chooseMusic(1); break;
    case 28: chooseMusic(4); break;
    case 29: chooseMusic(3); break;
    case 30: chooseMusic(2); break;
    case 31: chooseMusic(5); break;
    //
    case 46: sfx(rand() % kNSFX); break;
  }
  redrawSettingsMenuLine(getSettingsMenuUIBitmap(selectedID), selectedID);
}

#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_START 0
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_A_STOP 12
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_START 48
#define MAX_SETTINGS_ROWS_WHICH_MIGHT_CHANGE_B_STOP 53

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
    case 5: p->m_enableScreenShake ? checked() : unchecked(); break;
    case 6: p->m_enableTutorial != 255 ? checked() : unchecked(); break;
    case 7: p->m_enableConveyorAnimation ? checked() : unchecked(); break;
    case 8: autosave(p->m_enableAutosave); break;
    case 9: p->m_enablePickupOnDestroy ? checked() : unchecked(); break;
    case 10: p->m_autoUseConveyorBooster ? checked() : unchecked(); break;
    case 11: p->m_enableDebug ? checked() : unchecked(); break;
    // 
    case 48: addText(playTime(buf, p->m_playTime / TICK_FREQUENCY)); break;
    case 49: addNumber(p->m_moneyCumulative); break;
    case 50: addNumber(p->m_moneyHighWaterMark); break;
    case 51: addNumber(getTotalSoldCargo()); break;
    case 52: addNumber(getTotalImportedCargo()); break;
    case 53: addNumber(getNBuildings()); break;
    case 54: addNumber(getNCargo()); break;
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
    case 0: case 4: case 12: case 15: case 23: case 25: case 32: case 40: case 45: case 47: case 55: case 60: case 64: return true;
  }
  return false;
}

const char* getLine(int32_t _line) {
  switch (_line) {
    case 0: return "--- Sound Settings ---";
    case 1: return "Music";
    case 2: return "Music Volume";
    case 3: return "Sound Effects";
    //
    case 4: return "--- Game Settings ---";
    case 5: return "Screen Shake";
    case 6: return "Tutorial Enabled";
    case 7: return "Conveyor Animation";
    case 8: return "Auto Save Time";
    case 9: return "Auto Pickup on Destroy";
    case 10: return "Auto Apply Conveyor Grease";
    case 11: return "Show Debug Information";
    //
    case 12: return "--- Exit Game ---";
    case 13: return "Save & Exit to Title";
    case 14: return "Exit to Title WITHOUT Saving";
    //
    case 15: return "--- Controls ---";
    case 16: return "Inventory/Select/Interact: Ⓐ";
    case 17: return "Back/Cancel: Ⓑ";
    case 18: return "Walk: ✛";
    case 19: return "Run: Hold Ⓑ + ✛";
    case 20: return "Zoom in/out: 🎣";
    case 21: return "Rotate: 🎣 or Hold Ⓑ + ⬆️/➡️/⬇️/⬅️";
    case 22: return "Resize: 🎣 or Hold Ⓑ + ⬆️/⬇️";
    //
    case 23: return "--- Credits ---";
    case 24: return "Factory Farming by Tim Martin";
    //
    case 25: return "-- Music --";
    case 26: return "♬ Dr Tikov: 1985";
    case 27: return "♬ BoxCat Games: B-3";
    case 28: return "♬ Eric Skiff: We're the Resistors";
    case 29: return "♬ RoccoW: Sweet Self Satisfaction";
    case 30: return "♬ RoccoW: Pumped";
    case 31: return "♬ Soft & Furious: Horizon Ending";
    //
    case 32: return "-- Art --";
    case 33: return "Kenney: 1-Bit Pack";
    case 34: return "VectorPixelStar: 1-Bit Patterns";
    case 35: return "Josehzz: Farming Crops";
    case 36: return "ScratchIO: Farming Set";
    case 37: return "Varkalandar: Isometric Rocks";
    case 38: return "Withthelove: Character Sprites";
    case 39: return "DinosoftLab: New (NounProject)";
    //
    case 40: return "-- Fonts --";
    case 41: return "Chester Jenkins: Cooper Hewitt";
    case 42: return "Martin Vacha: Roobert";
    case 43: return "Mediengestaltung: Messing Lettern";
    case 44: return "Nick's Fonts: Coventry Garden";
    //
    case 45: return "-- Sound Effects --";
    case 46: return "sfxr.me";
    //
    case 47: return "-- Statistics --";
    case 48: return "Play Time:";
    case 49: return "Total Money Earned:";
    case 50: return "Most Money Held:";
    case 51: return "Total Cargo Sold:";
    case 52: return "Total Cargo Imported:";
    case 53: return "Total Building in this Plot:";
    case 54: return "Total Cargo in this Plot:";
    //
    case 55: return " ";
    case 56: return "© Tim Martin 2022";
    case 57: return "Factory Farming is an MIT";
    case 58: return "Licensed Open Source Project";
    case 59: return "github.com/timboe/FactoryFarming";
    //
    case 60: return " ";
    case 61: return "Thank you for playing!";
    case 62: return "And for your support of niche";
    case 63: return "indy game dev projects :)";
    //
    case 64: return " ";
    case 65: return "Keep on Maximizing Profits,";
    case 66: return "maximizing Efficiency.";
    // can currently go to 120
  }
  return "???";
}
