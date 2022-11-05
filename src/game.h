#pragma once

//#define DEV

//#define SSHOT_ON_AUTOSAVE

//#define SLOW_LOAD

// Careful - can seem to break things 
//#define FAST_PROGRESS

//#define DEMO

//#define TITLE_LOGO_ONLY

#define ONLY_SLOW_TICKS 

//#define ONLY_FAST_TICKS

//#define SYNCHRONOUS_SAVE_ENABLED

// Aggressive free: dealocate used sprites
// Agressive alocate: allocate all sprites at load
// Neither: Allocate as we go, don't free

//#define AGGRESSIVE_FREE_CARGO_SPRITES

//#define AGGRESSIVE_ALLOCATE_CARGO_SPRITES


#define PRETEND_ZOOMED_IN false

#include <stdio.h>
#include <stdbool.h> 

#include "pd_api.h"
#include "constants.h"

// Directions
enum kDir{SN, WE, NS, EW, kDirN};

extern PlaydateAPI* pd;

void setPDPtr(PlaydateAPI* _p);

int gameLoop(void* _data);

int getFrameCount(void);

uint16_t getNearTickCount(void);

uint16_t getFarTickCount(void);

uint16_t locToPix(uint16_t _loc);

uint16_t pixToLoc(uint16_t _pix);

void menuOptionsCallbackMenu(void*);

void menuOptionsCallbackDelete(void* _save);

void menuOptionsCallbackLoad(void*);

void menuOptionsCallbackSave(void*);

void populateMenuTitle(void);

void populateMenuGame(void);

void initGame(void);

uint16_t getCactusUnlock(void);

int32_t getNUnlocks(void);

void reset(bool _resetThePlayer);
