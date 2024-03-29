#include "sprite.h"
#include "input.h"

LCDBitmap* m_conveyorMasters[2][kDirN] = {NULL}; // For two different speed settings

LCDBitmapTable* m_sheet2;

LCDBitmapTable* m_sheet6;

LCDBitmapTable* m_sheet16;

LCDBitmapTable* m_sheet18;

LCDBitmapTable* m_sheet48;

LCDBitmapTable* m_sheetNavGuideTop;

LCDBitmapTable* m_sheetNavGuideBot;

LCDBitmap* m_bitmap16_zoom[ZOOM_LEVELS][SHEET16_SIZE];

LCDBitmap* m_bitmap18_zoom[ZOOM_LEVELS][SHEET16_SIZE];

LCDBitmap* m_bitmap48_zoom[ZOOM_LEVELS][SHEET48_SIZE];

LCDBitmap* m_bitmapNavGuideTop_zoom[ZOOM_LEVELS][SHEETNAVGUIDE_SIZE];

LCDBitmap* m_bitmapNavGuideBot_zoom[ZOOM_LEVELS][SHEETNAVGUIDE_SIZE];

LCDBitmap* m_retirementNo[ZOOM_LEVELS];

LCDBitmap* m_stickySelected;

LCDBitmap* m_titleSelected;

LCDBitmap* m_settingsMenuSelected;

LCDBitmap* m_new;

LCDBitmap* m_parchment[2];

LCDBitmap* m_splash;

LCDBitmap* m_menuBacking;

LCDBitmap* m_langArrow;

#ifdef DEMO
LCDBitmap* m_demo;
#endif

LCDBitmap* m_truck[ZOOM_LEVELS];

LCDFont* m_fontRoobert24;

LCDFont* m_fontRoobert10;

LCDFont* m_fontCooperHewitt12;

LCDFont* m_fontNotoBold;

LCDFont* m_fontNotoThin;

LCDFont* m_fontNotoLarge;

LCDBitmap* loadImageAtPath(const char* _path);

LCDBitmapTable* loadImageTableAtPath(const char* _path);

LCDFont* loadFontAtPath(const char* _path);

void populateResizedSprites(void);

/// ///

LCDBitmap* loadImageAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDBitmap* _img = pd->graphics->loadBitmap(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading image at path '%s': %s", _path, _outErr);
  }
  return _img;
}

LCDBitmapTable* loadImageTableAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDBitmapTable* _table = pd->graphics->loadBitmapTable(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading image table at path '%s': %s", _path, _outErr);
  }
  return _table;
}

LCDFont* loadFontAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDFont* _f = pd->graphics->loadFont(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading font at path '%s': %s", _path, _outErr);
  }
  return _f;
}

LCDBitmap* getSpriteNew() { return m_new; }

LCDBitmap* getSpriteSplash() { return m_splash; }

LCDBitmap* getSpriteTruck(uint8_t _zoom) { return m_truck[_zoom]; }

LCDBitmap* getSprite2(uint32_t _id) {
  return pd->graphics->getTableBitmap(m_sheet2, _id);
}

LCDBitmap* getSprite6(uint32_t _id) {
  return pd->graphics->getTableBitmap(m_sheet6, _id);
}

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y, uint8_t _zoom) {
  return getSprite16_byidx(SPRITE16_ID(_x, _y), _zoom);
}

LCDBitmap* getSprite16_byidx(uint32_t _idx, uint8_t _zoom) {
  if (_zoom == 1) {
    return pd->graphics->getTableBitmap(m_sheet16, _idx);
  } else if (_zoom < ZOOM_LEVELS) {
    return m_bitmap16_zoom[_zoom][_idx];
  }
  pd->system->error("getSprite16 Error");
  return NULL;
}

LCDBitmap* getSprite18(uint32_t _x, uint32_t _y, uint8_t _zoom) {
  return getSprite18_byidx(SPRITE18_ID(_x, _y), _zoom);
}

LCDBitmap* getSprite18_byidx(uint32_t _idx, uint8_t _zoom) {
  if (_zoom == 1) {
    return pd->graphics->getTableBitmap(m_sheet18, _idx);
  } else if (_zoom < ZOOM_LEVELS) {
    return m_bitmap18_zoom[_zoom][_idx];
  }
  pd->system->error("getSprite18 Error");
  return NULL;
}


LCDBitmap* getSprite48(uint32_t _x, uint32_t _y, uint8_t _zoom) {
  return getSprite48_byidx(SPRITE48_ID(_x, _y), _zoom);
}

LCDBitmap* getSprite48_byidx(uint32_t _idx, uint8_t _zoom) {
  if (_zoom == 1) {
    return pd->graphics->getTableBitmap(m_sheet48, _idx);
  } else if (_zoom < ZOOM_LEVELS) {
    return m_bitmap48_zoom[_zoom][_idx];
  }
  pd->system->error("getSprite48 Error");
  return NULL;
}

LCDBitmap* getSpriteNavGuideTop(uint32_t _idx, uint8_t _zoom) {
  if (_zoom == 1) {
    return pd->graphics->getTableBitmap(m_sheetNavGuideTop, _idx);
  } else if (_zoom < ZOOM_LEVELS) {
    return m_bitmapNavGuideTop_zoom[_zoom][_idx];
  }
  pd->system->error("getSpriteNavGuideTop Error");
  return NULL;
}

LCDBitmap* getSpriteNavGuideBot(uint32_t _idx, uint8_t _zoom) {
  if (_zoom == 1) {
    return pd->graphics->getTableBitmap(m_sheetNavGuideBot, _idx);
  } else if (_zoom < ZOOM_LEVELS) {
    return m_bitmapNavGuideBot_zoom[_zoom][_idx];
  }
  pd->system->error("getSpriteNavGuideBot Error");
  return NULL;
}

// Note: Only works at zoom level 2
void animateConveyor() {
  static int8_t tickSlow = 0, tickFast = 0;
  tickSlow = (tickSlow + 1) % 8;
  tickFast = (tickFast + 2) % 8;

  pd->graphics->setDrawMode(kDrawModeCopy);
  for (int32_t i = 0; i < kDirN; ++i) {
    pd->graphics->setDrawMode(kDrawModeInverted);
    pd->graphics->pushContext(m_conveyorMasters[1][i]);
    pd->graphics->drawBitmap(getSprite16(tickFast, CONV_START_Y + i, 2), 0, 0, kBitmapUnflipped);
    pd->graphics->popContext();

    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->pushContext(m_conveyorMasters[0][i]);
    pd->graphics->drawBitmap(getSprite16(tickSlow, CONV_START_Y + i, 2), 0, 0, kBitmapUnflipped);
    pd->graphics->popContext();
  }
}

LCDBitmap* getConveyorMaster(enum kDir _dir, uint8_t _speed) {
  switch (_speed) {
    case 1: return m_conveyorMasters[0][_dir];
    case 2: return m_conveyorMasters[1][_dir];
    default: pd->system->error("Error unknown conveyor speed %i", _speed);
  }
  return NULL;
}

LCDBitmap* getSpriteParchment(bool _left) {
  return m_parchment[ _left ? 0 : 1 ];
}

LCDBitmap* getRetirementNoBitmap(uint8_t _zoom) {
  return m_retirementNo[_zoom];
}

LCDBitmap* getStickySelectedBitmap() {
  return m_stickySelected;
}

LCDBitmap* getTitleSelectedBitmap() {
  return m_titleSelected;
}

LCDBitmap* getSettingsMenuSelectedBitmap() {
  return m_settingsMenuSelected;
}

LCDBitmap* getMenuBacking() {
  return m_menuBacking;
}

LCDBitmap* getLangArrowSprite() {
  return m_langArrow;
}

#ifdef DEMO
LCDBitmap* getDemoSplash() {
  return m_demo;
}
#endif

void setCooperHewitt12() {
  pd->graphics->setFont(getCooperHewitt12());
}

void setRoobert10() {
  pd->graphics->setFont(getRoobert10());
}

void setRoobert24() {
  pd->graphics->setFont(getRoobert24());
}

LCDFont* getRoobert24() {
  return getLanguage() == kEN ? m_fontRoobert24 : m_fontNotoLarge;
}

LCDFont* getRoobert10(void) {
  return getLanguage() == kEN ? m_fontRoobert10 : m_fontNotoBold;
}

LCDFont* getCooperHewitt12(void) {
  return getLanguage() == kEN ? m_fontCooperHewitt12 : m_fontNotoThin;
}

void populateResizedSprites() {
  for (uint32_t zoom = 2; zoom < ZOOM_LEVELS; ++zoom) {
    //
    m_retirementNo[zoom] = pd->graphics->newBitmap(TILE_PIX*3 * zoom, TILE_PIX*3 * zoom, kColorClear);
    pd->graphics->pushContext(m_retirementNo[zoom]);
    pd->graphics->drawScaledBitmap(m_retirementNo[1], 0, 0, zoom, zoom);
    pd->graphics->popContext();
    //
    m_truck[zoom] = pd->graphics->newBitmap(TILE_PIX*2 * zoom, TILE_PIX * zoom, kColorClear);
    pd->graphics->pushContext(m_truck[zoom]);
    pd->graphics->drawScaledBitmap(m_truck[1], 0, 0, zoom, zoom);
    pd->graphics->popContext();
    //
    for (uint32_t i = 0; i < SHEET16_SIZE; ++i) {
      LCDBitmap* original = pd->graphics->getTableBitmap(m_sheet16, i);
      m_bitmap16_zoom[zoom][i] = pd->graphics->newBitmap(TILE_PIX * zoom, TILE_PIX * zoom, kColorClear);
      pd->graphics->pushContext(m_bitmap16_zoom[zoom][i]);
      pd->graphics->drawScaledBitmap(original, 0, 0, zoom, zoom);
      pd->graphics->popContext();
    }
    for (uint32_t i = 0; i < SHEET18_SIZE; ++i) {
      LCDBitmap* original = pd->graphics->getTableBitmap(m_sheet18, i);
      m_bitmap18_zoom[zoom][i] = pd->graphics->newBitmap(TILE_PIX * zoom, 18 * zoom, kColorClear);
      pd->graphics->pushContext(m_bitmap18_zoom[zoom][i]);
      pd->graphics->drawScaledBitmap(original, 0, 0, zoom, zoom);
      pd->graphics->popContext();
    }
    for (uint32_t i = 0; i < SHEET48_SIZE; ++i) {
      LCDBitmap* original = pd->graphics->getTableBitmap(m_sheet48, i);
      m_bitmap48_zoom[zoom][i] = pd->graphics->newBitmap(TILE_PIX*3 * zoom, TILE_PIX*3 * zoom, kColorClear);
      pd->graphics->pushContext(m_bitmap48_zoom[zoom][i]);
      pd->graphics->drawScaledBitmap(original, 0, 0, zoom, zoom);
      pd->graphics->popContext();
    }
    for (uint32_t i = 0; i < SHEETNAVGUIDE_SIZE; ++i) {
      LCDBitmap* original = pd->graphics->getTableBitmap(m_sheetNavGuideTop, i);
      m_bitmapNavGuideTop_zoom[zoom][i] = pd->graphics->newBitmap(TILE_PIX*3 * zoom, TILE_PIX*3 * zoom, kColorClear);
      pd->graphics->pushContext(m_bitmapNavGuideTop_zoom[zoom][i]);
      pd->graphics->drawScaledBitmap(original, 0, 0, zoom, zoom);
      pd->graphics->popContext();
      //
      original = pd->graphics->getTableBitmap(m_sheetNavGuideBot, i);
      m_bitmapNavGuideBot_zoom[zoom][i] = pd->graphics->newBitmap(TILE_PIX*3 * zoom, TILE_PIX*3 * zoom, kColorClear);
      pd->graphics->pushContext(m_bitmapNavGuideBot_zoom[zoom][i]);
      pd->graphics->drawScaledBitmap(original, 0, 0, zoom, zoom);
      pd->graphics->popContext();
    }
  }
}

void initSprite() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  m_sheet2 = loadImageTableAtPath("images/sheet2");
  m_sheet6 = loadImageTableAtPath("images/sheet6");
  m_sheet16 = loadImageTableAtPath("images/sheet16");
  m_sheet18 = loadImageTableAtPath("images/sheet18");
  m_sheet48 = loadImageTableAtPath("images/sheet48");
  m_sheetNavGuideTop = loadImageTableAtPath("images/navGuideTop");
  m_sheetNavGuideBot = loadImageTableAtPath("images/navGuideBottom");
  m_stickySelected = loadImageAtPath("images/selected");
  m_titleSelected = loadImageAtPath("images/titleSelected");
  m_settingsMenuSelected = loadImageAtPath("images/mainmenuSelected");
  m_new = loadImageAtPath("images/new");
  m_parchment[0] = loadImageAtPath("images/map_l");
  m_parchment[1] = loadImageAtPath("images/map_r");
  m_splash = loadImageAtPath("images/splash");
  m_menuBacking = loadImageAtPath("images/backing");
  m_retirementNo[1] = loadImageAtPath("images/cross9x9");
  m_langArrow = loadImageAtPath("images/langArrow");
  #ifdef DEMO
  m_demo = loadImageAtPath("images/tech");
  #endif

  m_truck[1] = loadImageAtPath("images/truck");

  populateResizedSprites();

  for (int32_t i = 0; i < kDirN; ++i) {
    m_conveyorMasters[0][i] = pd->graphics->copyBitmap(getSprite16(0, CONV_START_Y + i, 2));
    m_conveyorMasters[1][i] = pd->graphics->copyBitmap(getSprite16(0, CONV_START_Y + i, 2));
  }

  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert10 = loadFontAtPath("fonts/Roobert-10-Bold");
  m_fontCooperHewitt12  = loadFontAtPath("fonts/CooperHewitt-Light");

  m_fontNotoThin = loadFontAtPath("fonts/NotoSans-Light-10");
  m_fontNotoBold = loadFontAtPath("fonts/NotoSans-Bold-10");
  m_fontNotoLarge  = loadFontAtPath("fonts/NotoSans-Medium-24");
  pd->graphics->setFont(m_fontRoobert24);

}
