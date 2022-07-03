#pragma once
#include "game.h"
#include "cargo.h"
#include "generate.h"

// All 48*48 should come after kExtractor
enum kBuildingType {kNoBuilding, kConveyor, kPlant, kUtility, kExtractor, kFactory, kSpecial, kNBuildingTypes};

///

enum kConvSubType{kBelt, kSplitL, kSplitI, kSplitT, kTunnelIn, kTunnelOut, kFilterL, kFilterI, kNConvSubTypes};
extern const uint32_t kConvUnlock[];
extern const uint16_t kConvPrice[];
extern const uint16_t kConvUIIcon[];

enum kPlantSubType{kCarrotPlant, kAppleTree, kWheatPlant, kP4, kP5, kP6, kP7, kP8, kP9, kP10, kP11, kP12, kNPlantSubTypes};
extern const uint32_t kPlantUnlock[];
extern const uint16_t kPlantPrice[];
extern const uint16_t kPlantUIIcon[];
extern const enum kGroundWetness kPlantWetness[];
extern const enum kGroundType kPlantSoil[];

enum kUtilitySubType{kWell, kStorageBox, kConveyorGrease, kNUtilitySubTypes};
extern const uint32_t kUtilityUnlock[];
extern const uint16_t kUtilityPrice[];
extern const uint16_t kUtilityUIIcon[];

enum kExtractorSubType{kCropHarvesterSmall, kPump, kChalkQuarry, kCropHarvesterLarge, kNExtractorSubTypes};
extern const uint32_t kExtractorUnlock[];
extern const uint16_t kExtractorPrice[];
extern const uint16_t kExtractorUIIcon[];
extern const uint16_t kExtractorSprite[];

enum kFactorySubType{kVitiminFac, kF2, kF3, kF4, kF5, kF6, kF7, kF8, kF9, kF10, kF11, kNFactorySubTypes};
extern const uint32_t kFactoryUnlock[];
extern const uint16_t kFactoryPrice[];
extern const uint16_t kFactoryTime[];
extern const uint16_t kFactorySprite[];
extern const enum kCargoType kFactoryOut[];
extern const enum kCargoType kFactoryIn1[];
extern const enum kCargoType kFactoryIn2[];
extern const enum kCargoType kFactoryIn3[];
extern const enum kCargoType kFactoryIn4[];
extern const enum kCargoType kFactoryIn5[];

enum kSpecialSubType{kShop, kSellBox, kExportBox, kImportBox, kWarp, kNSpecialSubTypes};

union kSubType {
   enum kConvSubType conveyor;
   enum kPlantSubType plant;
   enum kUtilitySubType utility;
   enum kExtractorSubType extractor;
   enum kFactorySubType factory;
   enum kSpecialSubType special; 
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
  int16_t m_progress; // Counts towards the location's progress
  // m_mode
  // Conveyor: where next.
  // Conveyor Filter: my filter item
  // Plant: how many plants have grown
  // Well: The underlying tile type, before we overwrote it with water (replaced on destruct)
  uint16_t m_mode; 
  // m_stored
  // For locations which store/produce fixed items, location 0 is always for produced items, 1-6 for inputs.
  // For locations which store variable items, locations 0-3 are the number of items, 3-6 are the item ID 
  // For plants: locations 0, 1 are used for item animation.
  // For conveyors, location 0 stores the conveyor's speed multiplier (1 or 2)
  uint8_t m_stored[MAX_STORE]; 

  // Transient
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_image[ZOOM_LEVELS]; // Index 0 not used
  struct Location_t* m_next[4]; // Where to send outputs
  struct Location_t* m_location; // Building's location
  enum kDir m_nextDir[4];// Where to animate outputs (conveyor only)
  void (*m_updateFn)(struct Building_t*, uint8_t, uint8_t);
};

uint16_t getNBuildings(void);

uint16_t getNByType(enum kBuildingType _type);

const char* toStringBuilding(enum kBuildingType _type, union kSubType _subType, bool _inworld);

struct Building_t* buildingManagerNewBuilding(enum kBuildingType _asType);

struct Building_t* buildingManagerGetByIndex(uint16_t _index);

void buildingManagerFreeBuilding(struct Building_t* _buildings);

bool newBuilding(struct Location_t* _loc, enum kDir _dir, enum kBuildingType _type, union kSubType _subType);

void getBuildingNeighbors(struct Building_t* _building, int8_t _offset, struct Location_t** _above, struct Location_t** _below, struct Location_t** _left, struct Location_t** _right);

void initBuilding(void);

void resetBuilding(void);

void serialiseBuilding(struct json_encoder* je);

void deserialiseValueBuilding(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneBuilding(json_decoder* jd, const char* _name, json_value_type _type);