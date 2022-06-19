#pragma once
#include "../game.h"
#include "../building.h"

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom);

void assignNeighborsConveyor(struct Building_t* _building);

void buildingSetupConveyor(struct Building_t* _building);

bool canBePlacedConveyor(struct Location_t* _loc, enum kDir _dir, union kSubType _subType);

struct Location_t* getTunnelOutLocation(struct Location_t* _in, enum kDir _dir);

struct Tile_t* getTunnelOutTile(struct Location_t* _in, enum kDir _dir);