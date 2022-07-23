#pragma once
#include "../game.h"
#include "../ui.h"

enum kUICat getNewCategory(void);

uint32_t getNewID(void);

const char* getNewText(void);

// This function has side effects (changes to New menu if returning true)
bool checkShowNew(void);