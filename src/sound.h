#pragma once
#include "game.h"

enum SfxSample {
 kSfxDestroy,
 kSfxClearObstruction,
 kSfxPickCargo,
 kSfxPlacePlant,
 kSfxUnlock,
 kSfxSell,
 kSfxA,
 kSfxB,
 kSfxD,
 kSfxMenuOpen,
 kSfxRotate,
 kSfxWarp,
 kSfxNo,
 kNSFX
};

void initSound(void);

void updateMusic(bool _isTitle);

void updateSfx(void);

void pauseMusic(void);

void resumeMusic(void);

void playCredits(void);

void sfx(enum SfxSample _sample);
