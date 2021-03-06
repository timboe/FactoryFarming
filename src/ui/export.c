#include "export.h"
#include "../player.h"
#include "../ui.h"
#include "../sprite.h"
#include "../buildings/special.h"

/// ///

void doExport() {
  // noop
}

void populateInfoExport() {
  const uint16_t selectedID =  getUIContentID();
  updateExport();

  pd->sprite->setVisible(getCannotAffordSprite(), false);

  // INFO
  LCDBitmap* infoBitmap = getInfoBitmap();
  static char textA[128] = " ";
  static char textB[128] = " ";
  static char textC[128] = " ";
  setRoobert10();
  pd->graphics->clearBitmap(infoBitmap, kColorClear);
  pd->graphics->pushContext(infoBitmap);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);
  snprintf(textA, 128, "Exporting: %s", toStringCargoByType(selectedID));
  snprintf(textB, 128, "This Plot: %.2f/s", (double)getThisWorldCargoExport(selectedID));
  snprintf(textC, 128, "All Plots: %.2f/s", (double)getTotalCargoExport(selectedID));
  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(textA, 128, kASCIIEncoding, 1*TILE_PIX, +2);
  pd->graphics->drawText(textB, 128, kASCIIEncoding, 1*TILE_PIX, TILE_PIX - 2);
  pd->graphics->drawText(textC, 128, kASCIIEncoding, 9*TILE_PIX, TILE_PIX - 2);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->popContext();
  
}

bool populateContentExport() {
  updateExport();
  int16_t column = 0, row = 0;
  bool empty = true;
  setUIContentHeader(row, kUICatCargo);
  ++row;
  for (int32_t i = 0; i < getNSubTypes(kUICatCargo); ++i) {
    const float tot = getTotalCargoExport(i);
    if (!tot) {
      continue; // Am not exporting this type of cargo
    }    
    setUIContentItem(row, column, kUICatCargo, i, 0);
    empty = false;
    if (++column == ROW_WDTH) {
      ++row;
      column = 0;
    }
  }
  return empty;
}