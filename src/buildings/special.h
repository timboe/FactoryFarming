#pragma once
#include "../game.h"
#include "../building.h"

void specialUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsSpecial(struct Building_t* _building);

void buildingSetupSpecial(struct Building_t* _building);

int16_t distanceFromBuy(void);

int16_t distanceFromSell(void);

int16_t distanceFromWarp(void);

int16_t distanceFromOut(void);

int16_t distanceFromIn(void);

void updateExport(void);

void resetExport(void);

struct Building_t* getImportBox(void);

void drawUIInspectSpecial(struct Building_t* _building);

bool isCamouflaged(void); 