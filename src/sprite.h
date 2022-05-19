#pragma once
#include "game.h"

struct Conveyor{
  LCDSprite* m_sprite;
  LCDSprite* m_cargo;
  struct Conveyor* m_next;
  uint8_t m_progress;
};


void initSprite(void);
void deinitSprite(void);

void setWorld(LCDBitmap* _b, uint32_t _x, uint32_t _y);
void setWorldEdges(void);

void animateConveyor(void);
void newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir);
uint32_t getNConveyors(void);

void setRoobert11(void);

LCDSprite* getPlayer(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y);

