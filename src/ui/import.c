#include "import.h"
#include "../player.h"
#include "../ui.h"
#include "../sprite.h"
#include "../buildings/special.h"

bool isEmptyImport(void);

/// ///

bool isEmptyImport() {
  for (int32_t dir = 0; dir < 4; ++dir) {
    enum kUICat cat = kUICatImportN + dir;
    for (int32_t i = 1; i < getNSubTypes(cat); ++i) { // Start at 1 to miss kNoCargo
      const float tot = getTotalCargoExport(i);
      const uint16_t totConsumers = getCargoImportConsumers(i);
      if (!tot && !totConsumers) {
        continue; // Am not exporting this type of cargo
      }
      return false;
    }
  }
  return true;
}

void doImport() {
  struct Building_t* import = getImportBox();  
  struct Player_t* p = getPlayer();
  const uint16_t newExport =  getUIContentID();
  const enum kUICat selectedCat = getUIContentCategory();

  if (isEmptyImport() || selectedCat < kUICatImportN || selectedCat > kUICatImportW || newExport == kNoCargo) { // Later should never happen, just double-checking
    return;
  }

  // Get current export
  enum kCargoType currentExport = kNoCargo;

  switch (selectedCat) {
    case kUICatImportN:; currentExport = import->m_stored[3]; break;
    case kUICatImportE:; currentExport = import->m_stored[4]; break;
    case kUICatImportS:; currentExport = import->m_stored[5]; break;
    case kUICatImportW:; currentExport = import->m_mode.mode8[1]; break;
    default: break;
  }
  
  #ifdef DEV
  pd->system->logToConsole("doImport: newExport %i %s - consumers %i, selected cat %i, currentExport %i %s - consumers %i ",
    newExport, toStringCargoByType(newExport, true), p->m_importConsumers[newExport], 
    selectedCat,
    currentExport, toStringCargoByType(currentExport, true), p->m_importConsumers[currentExport]);
  #endif

  if (currentExport == kNoCargo) {
    // If currentExport == none, just add the export

    switch (selectedCat) {
      case kUICatImportN:; import->m_stored[3] = newExport; break;
      case kUICatImportE:; import->m_stored[4] = newExport; break;
      case kUICatImportS:; import->m_stored[5] = newExport; break;
      case kUICatImportW:; import->m_mode.mode8[1] = newExport; break;
      default: break;
    }

    p->m_importConsumers[newExport]++;

  } else if (currentExport == newExport) {
    // If currentExport == newExport, then toggle the export to off

    switch (selectedCat) {
      case kUICatImportN:; import->m_stored[0] = 0; import->m_stored[3] = kNoCargo; break;
      case kUICatImportE:; import->m_stored[1] = 0; import->m_stored[4] = kNoCargo; break;
      case kUICatImportS:; import->m_stored[2] = 0; import->m_stored[5] = kNoCargo; break;
      case kUICatImportW:; import->m_mode.mode8[0] = 0; import->m_mode.mode8[1] = kNoCargo; break;
      default: break;
    }

    p->m_importConsumers[newExport]--;

  } else {
    // Deregister the old import, clear any holding, register the new import

    switch (selectedCat) {
      case kUICatImportN:; import->m_stored[0] = 0; import->m_stored[3] = newExport; break;
      case kUICatImportE:; import->m_stored[1] = 0; import->m_stored[4] = newExport; break;
      case kUICatImportS:; import->m_stored[2] = 0; import->m_stored[5] = newExport; break;
      case kUICatImportW:; import->m_mode.mode8[0] = 0; import->m_mode.mode8[1] = newExport; break;
      default: break;
    }

    p->m_importConsumers[currentExport]--;
    p->m_importConsumers[newExport]++;
  }

  UIDirtyMain();
}

void populateInfoImport() {
  struct Building_t* import = getImportBox();  
  const uint16_t selectedID =  getUIContentID();
  const enum kUICat selectedCat = getUIContentCategory();
  const float totExport = getTotalCargoExport(selectedID);
  const uint16_t totConsumers = getCargoImportConsumers(selectedID);
  
  pd->sprite->setVisible(getCannotAffordSprite(), 0);

  bool isStickySelected = false;
  switch (selectedCat) {
    case kUICatImportN:; isStickySelected = (import->m_stored[3] == selectedID); break;
    case kUICatImportE:; isStickySelected = (import->m_stored[4] == selectedID); break;
    case kUICatImportS:; isStickySelected = (import->m_stored[5] == selectedID); break;
    case kUICatImportW:; isStickySelected = (import->m_mode.mode8[1] == selectedID); break;
    default: break;
  }
  const float importPerConsumer = isStickySelected ? totExport / totConsumers : 0.0f;

  // INFO
  LCDBitmap* infoBitmap = getInfoBitmap();
  static char textA[128] = " ";
  static char textB[128] = " ";
  static char textC[128] = " ";
  static char strFloat[16] = " ";

  setRoobert10();
  pd->graphics->clearBitmap(infoBitmap, kColorClear);
  pd->graphics->pushContext(infoBitmap);
  roundedRect(1, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorBlack);
  roundedRect(3, TILE_PIX*18, TILE_PIX*2, TILE_PIX/2, kColorWhite);

  strcpy(textA, tr(kTRUIImportOf));
  strcat(textA, cspace());
  strcat(textA, toStringCargoByType(selectedID, /*plural=*/true));
  
  snprintf(textB, 128, tr(kTRUIImporters), totConsumers);
  snprintf(textC, 128, tr(kTRUIImported), ftos(importPerConsumer, 16, strFloat));

  pd->graphics->setDrawMode(kDrawModeFillBlack);
  pd->graphics->drawText(textA, 128, kUTF8Encoding, TILE_PIX/2, +2 +tY());
  pd->graphics->drawText(textB, 128, kUTF8Encoding, TILE_PIX/2, TILE_PIX - 2 +tY());
  pd->graphics->drawText(textC, 128, kUTF8Encoding, (getLanguage() == kRU ? 8*TILE_PIX : 9*TILE_PIX), TILE_PIX - 2 +tY());
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->popContext();
}

bool populateContentImport() {
  struct Building_t* import = getImportBox();
  int16_t column = 0, row = 0;
  if (isEmptyImport()) {
    setUIContentHeader(row, kUICatImportN);
    return true;
  }

  for (int32_t dir = 0; dir < 4; ++dir) {
    column = 0;
    if (dir) ++row;
    enum kUICat cat = kUICatImportN + dir;
    setUIContentHeader(row, cat);
    ++row;

    for (int32_t i = 1; i < getNSubTypes(cat); ++i) { // Start at 1 to miss kNoCargo
      const float tot = getTotalCargoExport(i);
      const uint16_t totConsumers = getCargoImportConsumers(i);
      if (!tot && !totConsumers) {
        continue; // Am not exporting this type of cargo
      }
      #ifdef DEV
      if (!dir) pd->system->logToConsole("populate content import, cargo %s: total exports = %f", toStringCargoByType(i, true), (double)tot);
      #endif
 
      if (column == ROW_WDTH) { // Only start a new row if we are sure we have something to put in it
        ++row;
        column = 0;
      }
      setUIContentItem(row, column, cat, i, 0);
 
      // Chosen?
      bool chosen = false;
      switch (cat) {
        case kUICatImportN:; chosen = (import->m_stored[3] == i); break;
        case kUICatImportE:; chosen = (import->m_stored[4] == i); break;
        case kUICatImportS:; chosen = (import->m_stored[5] == i); break;
        case kUICatImportW:; chosen = (import->m_mode.mode8[1] == i); break; 
        default: break;
      }
      if (chosen) {
        setUIContentStickySelected(row, column, dir); // dir just selects the correct identical sticky sprite, 0-4
      }

      ++column;
    }
  }
  return false;
}