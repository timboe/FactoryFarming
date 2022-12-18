#pragma once
#include "game.h"

enum SfxSample {
 kSfxDestroy,
 kSfxClearObstruction,
 kSfxPickCargo,
 kSfxPlacePlant,
 kSfxPlaceConv,
 kSfxPlaceFac,
 kSfxPlaceExtract,
 kSfxPlaceUtil,
 kSfxPlaceCargo,
 kSfxUnlock,
 kSfxSell,
 kSfxBuy,
 kSfxA,
 kSfxB,
 kSfxD,
 kSfxMenuOpen,
 kSfxRotate,
 kSfxWarp,
 kSfxZoom,
 kSfxNo,
 kSfxConveyor,
 kFootstepDefault,
 kFootstepDefault1,
 kFootstepDefault2,
 kFootstepDefault3,
 kFootstepDefault4,
 kFootstepDefault5,
 kFootstepWater,
 kFootstepWater1,
 kFootstepWater2,
 kFootstepWater3,
 kFootstepWater4,
 kFootstepWater5,
 kNSFX
};

#define N_FOOTSTEPS 6

void initSound(void);

void updateMusic(bool _isTitle);

void updateMusicVol(void);

void updateSfx(void);

void pauseMusic(void);

void chooseMusic(int8_t _id);

void resumeMusic(void);

void sfx(enum SfxSample _sample);