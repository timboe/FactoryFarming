#include "settings.h"
#include "../sprite.h"

bool isTitle(int32_t _line);

const char* getLine(int32_t _line);

/// ///

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

  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->drawBitmap(getSprite16(5, 19, 1), TILE_PIX*16, 0, kBitmapUnflipped);



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
    case 0: case 3: case 11: return true;
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
    case 11: return "--- Controls ---";
    case 12: return "🟨 ⊙ 🔒 🎣 ✛ ⬆️/➡️/⬇️/⬅️";
    case 13: return "Rotate: 🎣 or Hold Ⓑ + ⬆️/➡️/⬇️/⬅️";
    case 14: return "Run: Hold Ⓑ + ✛";

  }
  return "???";
}
