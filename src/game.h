#pragma once

#include <stdio.h>
#include <stdbool.h> 

#include "pd_api.h"
#include "constants.h"

// Directions
enum kDir{SN, WE, NS, EW, kDirN};

enum kCurrentHoliday{kNoHoliday, kEaster, kChristmas};

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

enum kCurrentHoliday getCurrentHoliday(void);

int32_t getNUnlocks(void);

void reset(bool _resetThePlayer);

char* ftos(float _value, int16_t _size, char* _dest);