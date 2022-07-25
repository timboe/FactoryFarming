#include "settings.h"

/// ///

void populateContentMainmenu() {
  int16_t column = 0, row = 0;
  for (int32_t i = 0; i < MAX_ROWS; ++i) {
    setUIContentMainMenu(row, i==0);
  }
}

void populateInfoMainmenu() {

}
