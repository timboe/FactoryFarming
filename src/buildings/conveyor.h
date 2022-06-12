#pragma once
#include "../game.h"
#include "../building.h"

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsConveyor(struct Building_t* _building, struct Location_t* _above, struct Location_t* _below, struct Location_t* _left, struct Location_t* _right);

void buildingSpriteSetupConveyor(struct Building_t* _building);

bool canBePlacedConveyor(struct Location_t* _loc);