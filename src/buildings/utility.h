#pragma once
#include "../game.h"
#include "../building.h"

void utilityUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsUtility(struct Building_t* _building);

void buildingSetupUtility(struct Building_t* _building);

bool canBePlacedUtility(struct Location_t* _loc, enum kDir _dir, union kSubType _subType);

