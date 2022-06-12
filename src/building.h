#pragma once
#include "game.h"

// Building direction
enum kDir{SN, WE, NS, EW, kDirN};

enum kBuildingType {kNoBuilding, kConveyor, kPlant, kExtractor, kFactory, kNBuildingTypes};

///

enum kConvSubType{kBelt, kSplitI, kSplitL, kSplitT, kFilterL, kNConvSubTypes};

enum kPlantSubType{kCarrotPlant, kAppleTree, kNPlantSubTypes};

enum kExtractorSubType{kCropHarvester, kPump, kNExtractorSubTypes};

enum kFactorySubType{kMakeSmall, kNFactorySubTypes};

union kSubType {
   enum kConvSubType conveyor;
   enum kPlantSubType plant;
   enum kExtractorSubType extractor;
   enum kFactorySubType factory;
};

///

struct Building_t{
  // Persistent
  // Populated at RESET
  uint16_t m_index; // My location in the building array.

  // Populated at construct
  enum kBuildingType m_type;
  union kSubType m_subType;
  enum kDir m_dir; // Direction
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  uint16_t m_progress; // Counts towards the location's progress
  uint16_t m_mode; // For locations which alter between modes, such as a splitter
  uint8_t m_stored[MAX_STORE]; // For locations which store/produce. Location 0 is always for produced items

  // Transient
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_image[ZOOM_LEVELS]; // Index 0 not used
  struct Location_t* m_next[3]; // Where to send outputs, up to three (splitters)
  struct Location_t* m_location; // Building's location
  enum kDir m_nextDir[3];// Where to animate outputs, up to three (splitters)
  void (*m_updateFn)(struct Building_t*, uint8_t, uint8_t);
};

uint16_t getNBuildings(void);

uint16_t getNByType(enum kBuildingType _type);

struct Building_t* buildingManagerNewBuilding(enum kBuildingType _asType);

struct Building_t* buildingManagerGetByIndex(uint16_t _index);

void buildingManagerFreeBuilding(struct Building_t* _buildings);

bool newBuilding(struct Location_t* _loc, enum kDir _dir, enum kBuildingType _type, union kSubType _subType);

void initBuilding(void);

void resetBuilding(void);

void serialiseBuilding(struct json_encoder* je);

void deserialiseValueBuilding(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneBuilding(json_decoder* jd, const char* _name, json_value_type _type);