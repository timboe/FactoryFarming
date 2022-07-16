#pragma once
#include "../game.h"
#include "../building.h"

void factoryUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsFactory(struct Building_t* _building);

void buildingSetupFactory(struct Building_t* _building);

bool canBePlacedFactory(struct Location_t* _loc, enum kDir _dir, union kSubType _subType);

uint16_t getNIngreindientsText(enum kFactorySubType _type);

const char* toStringIngredients(enum kFactorySubType _type, uint16_t _n, bool* _isFlavour);

void drawUIInspectFactory(struct Building_t* _building);