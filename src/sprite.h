#pragma once
#include "game.h"

enum kLocationType {kEmpty, kConveyor};

enum kCargoType {kNoCargo, kApple};

struct Cargo_t{
  LCDSprite* m_sprite;
};


struct Location_t{
  enum kLocationType m_type;
  LCDSprite* m_sprite;
  LCDBitmap* m_image;
  LCDSprite* m_cargo;
  struct Location_t* m_next;
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In location coords
  uint16_t m_y;
  uint8_t m_progress;
};


struct Chunk_t{
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In chunk coords
  uint16_t m_y;

  LCDSprite* m_bkgSprite;
  LCDBitmap* m_bkgImage;

  struct Chunk_t* m_neighboursNE[3];
  struct Chunk_t* m_neighboursSE[3];
  struct Chunk_t* m_neighboursSW[3];
  struct Chunk_t* m_neighboursNW[3];
  struct Chunk_t* m_neighboursALL[8];

  uint16_t m_nLocations;
  struct Location_t* m_locations[TILES_PER_CHUNK_X * TILES_PER_CHUNK_Y];

  uint16_t m_nCargos;
  struct Cargo_t* m_cargos[TILES_PER_CHUNK_X * TILES_PER_CHUNK_Y];
};


struct Player_t{
  LCDSprite* m_sprite;
  float m_x;
  float m_y;
  float m_vX;
  float m_vY;
};



void initSprite(void);
void deinitSprite(void);

struct Location_t* getLocation(int32_t _x, int32_t _y);

struct Chunk_t* getChunk(int32_t _x, int32_t _y);

struct Chunk_t* getChunk_noCheck(const int32_t _x, const int32_t _y);

struct Chunk_t* getEdgeChunk(uint32_t _i);

void animateConveyor(void);
bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir);
uint32_t getNConveyors(void);

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type);
uint32_t getNCargo(void);

void setRoobert11(void);

struct Player_t* getPlayer(void);

LCDBitmap* getSprite16(uint32_t _x, uint32_t _y);

