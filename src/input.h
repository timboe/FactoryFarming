#pragma once
#include "game.h"

void gameClickConfigHandler(uint32_t _buttonPressed);

void clickHandlerReplacement(void);

uint8_t getPressed(uint32_t _i);

bool getPressedAny(void);

uint8_t getZoom(void);

void setZoom(uint8_t _z);

uint8_t getRadius(void);

bool bPressed(void);

void unZoom(void);

void resetMomentum(void);

int8_t getAndReduceFollowConveyor(void);