#pragma once

#define DEBUG_MODE
#define DEV

#define PRETEND_ZOOMED_IN false

#include <stdio.h>
#include <stdbool.h> 

#include "pd_api.h"
#include "constants.h"

extern PlaydateAPI* pd;

void setPDPtr(PlaydateAPI* _p);

int gameLoop(void* _data);

int getFrameCount(void);

void initGame(void);

void gameClickConfigHandler(uint32_t _buttonPressed);

void clickHandlerReplacement(void);

uint8_t getPressed(uint32_t _i);

uint8_t getZoom(void);

