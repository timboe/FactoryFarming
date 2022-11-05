#pragma once
#include "../game.h"
#include "../building.h"

bool plantUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsPlant(struct Building_t* _building);

void buildingSetupPlant(struct Building_t* _building);

bool canBePlacedPlant(struct Location_t* _loc, union kSubType _subType);

void plantTrySpawnCargo(struct Building_t* _building, uint8_t _tick);

void drawUIInspectPlant(struct Building_t* _building);

uint16_t getPlantSmilieSprite(int8_t _value);

int8_t getWaterBonus(enum kGroundWetness _likes, enum kGroundWetness _has);

int8_t getGroundBonus(enum kGroundType _likes, enum kGroundType _has);