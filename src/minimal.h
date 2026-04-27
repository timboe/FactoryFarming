#pragma once

enum kSaveLoadRequest {kDoNothing, kDoSave, kDoLoad, kDoNewWorld, kDoSaveDelete, kDoScanSlots, kDoResetPlayer, kDoTitle, kDoScreenShot};

#define N_SAVES 3

void initLocalisation(void);
void initSprite(void);
void initWorld(void);
void initMap(void);
void initCargo(void);
void initChunk(void);
void initLocation(void);
void initPlayer(void);
void initiUI(void);
void initBuilding(void);
void initSound(void);
void initGame(void);
void setSave(uint8_t _save);
void scanSlots(void);
void doIO(enum kSaveLoadRequest _first, enum kSaveLoadRequest _andThen, enum kSaveLoadRequest _andFinally);
int gameLoop_ff(void* _data);
