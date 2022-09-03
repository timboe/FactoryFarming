#pragma once
#include "../game.h"
#include "../ui.h"

enum kNewReturnStatus {
  kNewYes,
  kNewNoNeedsFarming,
  kNewNoNeedsTutorial,
  kNewNoUnlockedAll
};

enum kBuildingType getNewBuildingType(void);

uint32_t getNewID(void);

const char* getNewText(void);

// This function has side effects (changes to New menu if returning true)
bool checkShowNew(void);

enum kNewReturnStatus checkHasNewToShow(struct Player_t* _p);

