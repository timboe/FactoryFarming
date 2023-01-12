#pragma once
#include "game.h"
#include "cargo.h"
#include "generate.h"

///

#include "blists.h"

struct ConveyorDescriptor_t {
  enum kConvSubType subType;
  uint16_t unlock;
  uint32_t price;
  uint16_t UIIcon;
  uint8_t spriteX;
  uint8_t spriteY;
};
extern struct ConveyorDescriptor_t CDesc[];

struct PlantDescriptor_t {
  enum kPlantSubType subType;
  uint16_t unlock;
  uint32_t price;
  uint16_t sprite;
  uint16_t time;
  float multi;
  enum kGroundWetness wetness;
  enum kGroundType soil;
  enum kCargoType out;
};
extern struct PlantDescriptor_t PDesc[];

struct UtilityDescriptor_t {
  enum kUtilitySubType subType;
  uint16_t unlock;
  uint32_t price;
  uint16_t UIIcon;
  uint16_t sprite;
};
extern struct UtilityDescriptor_t UDesc[];

struct ExtractorDescriptor_t {
  enum kExtractorSubType subType;
  uint16_t unlock;
  uint32_t price;
  uint16_t UIIcon;
  uint16_t sprite;
  bool invert;
  enum kCargoType out;
};
extern struct ExtractorDescriptor_t EDesc[];

struct FactoryDescriptor_t {
  enum kFactorySubType subType;
  uint16_t unlock;
  uint32_t price;
  uint16_t time;
  uint16_t UIIcon;
  uint16_t sprite;
  float multi;
  bool invert;
  enum kCargoType out;
  enum kCargoType in1;
  enum kCargoType in2;
  enum kCargoType in3;
  enum kCargoType in4;
  enum kCargoType in5;
};
extern struct FactoryDescriptor_t FDesc[];

///

union kMode {
  uint16_t mode16;
  uint8_t mode8[2];
};

///

struct UnlockDescriptor_t {
  enum kBuildingType type;
  union kSubType subType;
  uint16_t fromSelling;
  enum kCargoType ofCargo;
};
extern struct UnlockDescriptor_t UnlockDecs[]; 

///

struct Building_t{
  // Persistent
  // Populated at RESET
  uint16_t m_index; // My location in the building array.

  // Populated at construct
  enum kBuildingType m_type;
  union kSubType m_subType;
  enum kDir m_dir; // Direction. For special: Non-SN is camouflaged.
  uint16_t m_pix_x; // Note: centre
  uint16_t m_pix_y;
  int16_t m_progress; // Counts towards the location's progress
  // m_mode. Either one uint16_t or two uint8_t
  // Conveyor (16): where next.
  // Conveyor Filter (16): my filter item
  // Plant (16): how many plants have grown
  // Well (16): The underlying tile type, before we overwrote it with water (replaced on destruct)
  // Rotavator (8,8): The ID of the S and SE tiles before they were changed to loamy
  // Import box (8,8): The ID [0] and capacity [1] of the 4th item slot: West. (N, E, S are stored in m_stored)
  // Factory (16): Production time in ticks
  union kMode m_mode; 
  // m_stored
  // For locations which store/produce fixed items, location 0 is always for produced items, 1-5 for inputs.
  // For locations which store variable items, locations 0,1,2 are the number of items, 3,4,5 are the item ID 
  // For rotavator locations 0-5 store the tile ID of the (respetivly) NW, N, NE, W, E, SW tiles 
  // For plants: locations 0, 1 are used for item animation.
  // For conveyors
  // - location 0 stores the conveyor's speed multiplier (1 or 2).
  // - location 1 stores the current cargo x SIGNED offset.
  // - location 2 stores the current cargo y SIGNED offset.
  // - location 3 is reserved for tunnels and causes the cargo to render off-screen.
  // - location 4 is marked when a cargo moves into the conveyor. It should not update again on the same tick
  uint8_t m_stored[MAX_STORE]; 

  // Transient
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_image[ZOOM_LEVELS]; // Index 0 not used
  struct Location_t* m_next[4]; // Where to send outputs
  struct Location_t* m_location; // Building's location
  enum kDir m_nextDir[4];// Where to animate outputs (conveyor only)
  uint8_t m_tickProcessed; // Which tick the building last processed
  void (*m_updateFn)(struct Building_t*, uint8_t, uint8_t, uint8_t);
};

///

uint16_t getNBuildings(void);

uint16_t getNByType(enum kBuildingType _type);

const char* toStringBuilding(enum kBuildingType _type, union kSubType _subType, bool _inworld);

struct Building_t* buildingManagerNewBuilding(enum kBuildingType _asType);

struct Building_t* buildingManagerGetByIndex(uint16_t _index);

bool buildingHasUpdateFunction(enum kBuildingType _type, union kSubType _subType);

void growAtAll(uint16_t _maxTicks);

bool isLargeBuilding(enum kBuildingType _type, union kSubType _subType);

bool isInvertedBuilding(struct Building_t* _building);

bool needs3x3Redraw(enum kBuildingType _type, union kSubType _subType);

bool producesOutputCargoOnAdjacentTile(enum kBuildingType _type, union kSubType _subType);

void buildingManagerFreeBuilding(struct Building_t* _buildings);

bool newBuilding(struct Location_t* _loc, enum kDir _dir, enum kBuildingType _type, union kSubType _subType);

void getBuildingNeighbors(struct Building_t* _building, int8_t _offset, struct Location_t** _above, struct Location_t** _below, struct Location_t** _left, struct Location_t** _right);

void initBuilding(void);

void resetBuilding(void);

void serialiseBuilding(struct json_encoder* je);

void deserialiseValueBuilding(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDoneBuilding(json_decoder* jd, const char* _name, json_value_type _type);

// Note: Under pdescriptors.c
void setPrices(void);