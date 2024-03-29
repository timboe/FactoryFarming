#pragma once
#include "../game.h"
#include "../building.h"

void extractorUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void assignNeighborsExtractor(struct Building_t* _building);

void buildingSetupExtractor(struct Building_t* _building);

bool canBePlacedExtractor(const struct Location_t* _loc, union kSubType _subType);

// For buildings which can hold any three types of cargo
void tryPickupAnyCargo(struct Location_t* _from, struct Building_t* _building);

void tryPutdownAnyCargo(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void drawUIInspectExtractor(struct Building_t* _building);