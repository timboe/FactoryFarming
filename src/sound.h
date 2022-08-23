#pragma once
#include "game.h"

void initSound(void);

void updateMusic(bool _isTitle);

void updateSfx(void);

void pauseMusic(void);

void resumeMusic(void);

void sfxDestroy(void);

void sfxClearObstruction(void);

void sfxPickCargo(void);

void sfxPlacePlant(void);

void sfxUnlock(void);

void sfxSell(void);