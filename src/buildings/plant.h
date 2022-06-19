#pragma once
#include "../game.h"
#include "../building.h"

void plantUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsPlant(struct Building_t* _building);

void buildingSetupPlant(struct Building_t* _building);

bool canBePlacedPlant(struct Location_t* _loc, enum kDir _dir, union kSubType _subType);