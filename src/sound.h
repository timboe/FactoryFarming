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
 kFootstepDefault,
 kFootstepDefault1,
 kFootstepDefault2,
 kFootstepDefault3,
 kFootstepDefault4,
 kFootstepDefault5,
 kNSFX
};

#define N_FOOTSTEPS 6

void initSound(void);

void updateMusic(bool _isTitle);

void updateSfx(void);

void pauseMusic(void);

void chooseMusic(int8_t _id);

void resumeMusic(void);

void sfx(enum SfxSample _sample);