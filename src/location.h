#pragma once
#include "game.h"
#include "cargo.h"

enum kLocationType {kEmpty, kConveyor};

// Conveyor directions
enum kConvDir{SN, WE, NS, EW, kConvDirN};

enum kConvSubType{kBelt, kSplitI, kSplitL, kSplitT, kFilterL};

struct Location_t{
  enum kLocationType m_type;
  enum  kConvDir m_convDir; // Only used by conveyors
  enum  kConvSubType m_convSubType; // Only used by conveyors
  struct Chunk_t* m_chunk; // My chunk
  LCDSprite* m_sprite[5]; // Index 0 not used
  LCDBitmap* m_image[5]; // Index 0 not used
  struct Cargo_t* m_cargo;
  struct Location_t* m_next[3]; // Where to send outputs, up to three (splitters)
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_x; // In location coords
  uint16_t m_y;
  uint8_t m_progress; // Counts towards the location's progress
  uint8_t m_mode; // For locations which alter between modes, such as a splitter
  void (*m_updateFn)(struct Location_t*, uint8_t, uint8_t);
};

uint16_t getNConveyors(void);

struct Location_t* getLocation(int32_t _x, int32_t _y);

struct Location_t* getLocation_noCheck(int32_t _x, int32_t _y);

bool newConveyor(struct Location_t* _loc, enum kConvDir _dir);

void clearLocation(struct Location_t* _loc);

void initLocation(void);