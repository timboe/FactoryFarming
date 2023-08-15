#pragma once
#include "../game.h"
#include "../building.h"

void conveyorUpdateFn(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void assignNeighborsConveyor(struct Building_t* _building);

void buildingSetupConveyor(struct Building_t* _building);

bool canBePlacedConveyor(const struct Location_t* _loc, enum kDir _dir, union kSubType _subType);

struct Location_t* getTunnelOutLocation(const struct Location_t* _in, enum kDir _dir);

struct Tile_t* getTunnelOutTile(const struct Location_t* _in, enum kDir _dir);

void upgradeConveyor(struct Building_t* _building, bool _forFree);

void conveyorLocationUpdate(struct Building_t* _building, uint8_t _zoom);

void drawUIInspectConveyor(struct Building_t* _building);

void updateConveyorDirection(struct Building_t* _building, uint8_t _tickLength, uint8_t _tickID, uint8_t _zoom);

void conveyorUpdateSprite(struct Building_t* _building);

void checkConveyorSpritesAroundLoc(struct Location_t* _loc);