#pragma once
#include "game.h"
#include "cargo.h"

enum kLocationType {kEmpty, kConveyor};

// Conveyor directions
enum kConvDir{SN, NS, EW, WE, kConvDirN};

struct Location_t{
  enum kLocationType m_type;
  enum  kConvDir m_convDir; // Only used by conveyors
  struct Chunk_t* m_chunk; // My chunk
  LCDSprite* m_sprite;
  LCDBitmap* m_image;
  struct Cargo_t* m_cargo;
  struct Location_t* m_next;
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In location coords
  uint16_t m_y;
  uint8_t m_progress; // Counts towards the location's progress
  void (*m_updateFn)(struct Location_t*, uint8_t);
};

uint16_t getNConveyors(void);

struct Location_t* getLocation(int32_t _x, int32_t _y);

bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir);

void initLocation(void);