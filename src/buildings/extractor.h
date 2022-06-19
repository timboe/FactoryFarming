#pragma once
#include "../game.h"
#include "../building.h"

void extractorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsExtractor(struct Building_t* _building);

void buildingSetupExtractor(struct Building_t* _building);

bool canBePlacedExtractor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType);