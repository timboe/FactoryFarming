#pragma once
#include "../game.h"
#include "../ui.h"


void doSettings(void);

void redrawSettingsMenuLine(LCDBitmap* _bitmap, int32_t _line);

void redrawAllSettingsMenuLines(void);

void populateContentSettingsMenu(void);

void populateInfoSettingsMenu(void);

LCDBitmap* getPauseImage(void);