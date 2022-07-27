#include "settings.h"
#include "../sprite.h"
#include "../ui.h"
#include "../io.h"

LCDBitmap* m_pause = NULL;

bool isTitle(int32_t _line);

const char* getLine(int32_t _line);

void checked(void);

void unchecked(void);

void autosave(uint32_t _time);

/// ///

LCDBitmap* getPauseImage() {
  if (m_pause == NULL) m_pause = pd->graphics->newBitmap(DEVICE_PIX_X, DEVICE_PIX_Y, kColorClear);

  pd->graphics->clearBitmap(m_pause, kColorClear);

  if (getGameMode() == kTitles) return m_pause;

  pd->graphics->pushContext(m_pause);
  roundedRect(TILE_PIX,   DEVICE_PIX_X/2, DEVICE_PIX_Y, TILE_PIX, kColorBlack);
  roundedRect(TILE_PIX+ 5, DEVICE_PIX_X/2, DEVICE_PIX_Y, TILE_PIX, kColorWhite);
  pd->graphics->popContext();
  return m_pause;
}

void checked() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(6, 19, 1), TILE_PIX*16, 0, kBitmapUnflipped);
}

void unchecked() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(5, 19, 1), TILE_PIX*16, 0, kBitmapUnflipped);
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

void doSettings() {
  struct Player_t* p = getPlayer();
  const uint16_t selectedID =  getUIContentID();
  switch (selectedID) {
    case 1: p->m_soundSettings = p->m_soundSettings ^ 1; break;
    case 2: p->m_soundSettings = p->m_soundSettings ^ 2; break;
    case 4: p->m_enableScreenShake = !p->m_enableScreenShake; break;
    case 5: if (p->m_enableTutorial == 255) p->m_enableTutorial = 0; else p->m_enableTutorial = 255; break;
    case 6: p->m_enableConveyorAnimation = !p->m_enableConveyorAnimation; break;
    case 7: switch(p->m_enableAutosave) {
              case 0: p->m_enableAutosave = 5; break;
              case 5: p->m_enableAutosave = 10; break;
              case 10: p->m_enableAutosave = 15; break;
              case 15: p->m_enableAutosave = 30; break;
              case 30: p->m_enableAutosave = 0; break;
            }; break;
    case 8: p->m_enablePickupOnDestroy = !p->m_enablePickupOnDestroy; break;
    case 9: p->m_autoUseConveyorBooster = !p->m_autoUseConveyorBooster; break;
    case 10: p->m_enableDebug = !p->m_enableDebug; break;
    case 12: doIO(kDoSave, /*and then*/ kDoTitle); break;
    case 13: doIO(kDoTitle, /*and then*/ kDoNothing); break;
  }
  redrawMainmenuLine(getMainmenuUIBitmap(selectedID), selectedID);
}

void redrawMainmenuLine(LCDBitmap* _bitmap, int32_t _line) {

  setRoobert10();
  pd->graphics->clearBitmap(_bitmap, kColorWhite);
  pd->graphics->pushContext(_bitmap);
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  
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
  switch (_line) {
    case 1: (p->m_soundSettings == 1 || p->m_soundSettings == 3) ? checked() : unchecked(); break;
    case 2: (p->m_soundSettings == 2 || p->m_soundSettings == 3) ? checked() : unchecked(); break;
    case 4: p->m_enableScreenShake ? checked() : unchecked(); break;
    case 5: p->m_enableTutorial != 255 ? checked() : unchecked(); break;
    case 6: p->m_enableConveyorAnimation ? checked() : unchecked(); break;
    case 7: autosave(p->m_enableAutosave); break;
    case 8: p->m_enablePickupOnDestroy ? checked() : unchecked(); break;
    case 9: p->m_autoUseConveyorBooster ? checked() : unchecked(); break;
    case 10: p->m_enableDebug ? checked() : unchecked(); break;
  }

  pd->graphics->popContext();
}

void populateContentMainmenu() {
  for (int32_t i = 0; i < MAX_ROWS; ++i) {
    setUIContentMainMenu(i, isTitle(i));
  }
}

void populateInfoMainmenu() {

}

bool isTitle(int32_t _line) {
  switch (_line) {
    case 0: case 3: case 11: case 14: case 21: return true;
  }
  return false;
}

const char* getLine(int32_t _line) {
  switch (_line) {
    case 0: return "--- Sound Settings ---";
    case 1: return "Music";
    case 2: return "Sound Effects";
    case 3: return "--- Game Settings ---";
    case 4: return "Screen Shake";
    case 5: return "Tutorial Enabled";
    case 6: return "Conveyor Animation";
    case 7: return "Auto Save Time";
    case 8: return "Auto Pickup on Destroy";
    case 9: return "Auto Apply Conveyor Grease";
    case 10: return "Show Debug Information";
    case 11: return "--- Exit Game ---";
    case 12: return "Save & Exit to Title";
    case 13: return "Exit to Title WITHOUT Saving";
    case 14: return "--- Controls ---";
    case 15: return "Inventory/Select/Interact: Ⓐ";
    case 16: return "Back/Cancel: Ⓑ";
    case 17: return "Walk: ✛";
    case 18: return "Run: Hold Ⓑ + ✛";
    case 19: return "Zoom in/out: 🎣";
    case 20: return "Rotate: 🎣 or Hold Ⓑ + ⬆️/➡️/⬇️/⬅️";
    case 21: return "--- Credits ---";
    case 22: return "Factory Farming by Tim Martin";
    // can currently go to 36
  }
  return "???";
}
