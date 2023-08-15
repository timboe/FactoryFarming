#pragma once
#include "../game.h"
#include "../building.h"

void utilityUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void assignNeighborsUtility(struct Building_t* _building);

void buildingSetupUtility(struct Building_t* _building);

bool canBePlacedUtility(const struct Location_t* _loc, union kSubType _subType);

void drawUIInspectUtility(struct Building_t* _building);

bool doPlaceLandfill(struct Location_t* _loc);

bool doPlaceRetirement(struct Location_t* _loc);

int16_t distanceFromRetirement(void);

void buildRotavator(struct Location_t* _loc);

void destroyRotavator(struct Location_t* _loc);