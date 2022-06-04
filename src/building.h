#pragma once
#include "game.h"

// Conveyor directions
enum kConvDir{SN, WE, NS, EW, kConvDirN};

enum kConvSubType{kBelt, kSplitI, kSplitL, kSplitT, kFilterL};

enum kBuildingType {kNoBuilding, kConveyor};

struct Building_t{
  // Persistent
  uint16_t m_index; // My location in the building array.
  enum kBuildingType m_type;
  enum kConvDir m_convDir; // Only used by conveyors
  enum kConvSubType m_convSubType; // Only used by conveyors
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint8_t m_progress; // Counts towards the location's progress
  uint8_t m_mode; // For locations which alter between modes, such as a splitter

  // Transient
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_image[ZOOM_LEVELS]; // Index 0 not used
  struct Location_t* m_next[3]; // Where to send outputs, up to three (splitters)
  enum kConvDir m_nextDir[3];// Where to animate outputs, up to three (splitters)
  void (*m_updateFn)(struct Location_t*, uint8_t, uint8_t);
};

uint16_t getNBuildings(void);

uint16_t getNConveyors(void);

struct Building_t* buildingManagerNewBuilding(enum kBuildingType _asType);

void buildingManagerFreeBuilding(struct Building_t* _buildings);

bool newConveyor(struct Location_t* _loc, enum kConvDir _dir, enum kConvSubType _subType);

void initBuilding(void);

void resetBuilding(void);

void serialiseBuilding(struct json_encoder* je);