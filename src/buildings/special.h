#pragma once
#include "../game.h"
#include "../building.h"

void specialUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsSpecial(struct Building_t* _building);

void buildingSetupSpecial(struct Building_t* _building);

bool canBePlacedSpecial(struct Location_t* _loc, enum kDir _dir, union kSubType _subType);