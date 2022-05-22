#pragma once
#include "game.h"

enum kLocationType {kEmpty, kConveyor};

enum kCargoType {kNoCargo, kApple};


struct Location_t{
  enum kLocationType m_type;
  LCDSprite* m_sprite;
  LCDSprite* m_cargo;
  struct Location_t* m_next;
  uint16_t m_x;
  uint16_t m_y;
  uint8_t m_progress;
};


void initSprite(void);
void deinitSprite(void);

void setWorld(LCDBitmap* _b, uint32_t _x, uint32_t _y);
void setWorldEdges(void);

struct Location_t* getLocation(int32_t _x, int32_t _y);

void animateConveyor(void);
bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir);
uint32_t getNConveyors(void);

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type);
uint32_t getNCargo(void);

void setRoobert11(void);

LCDSprite* getPlayer(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y);

