#pragma once
#include "../game.h"
#include "../ui.h"


void doSettings(void);

void redrawMainmenuLine(LCDBitmap* _bitmap, int32_t _line);

void redrawAllSettingsMenuLines(void);

void populateContentMainmenu(void);

void populateInfoMainmenu(void);

LCDBitmap* getPauseImage(void);