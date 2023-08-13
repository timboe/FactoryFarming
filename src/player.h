#pragma once
#include "game.h"
#include "building.h"
#include "cargo.h"

enum kUITutorialStage{
  kTutWelcomeBuySeeds, 
  kTutSeeObjective,
  kTutPlantCarrots, 
  kTutGetCarrots, 
  kTutSellCarrots, 
  kTutBuildHarvester, 
  kTutBuildConveyor, 
  kTutBuildQuarry, 
  kTutBuildVitamin, 
  //
  kTutFinishedOne, 
  kTutBreakOne, 
  //
  kTutNewPlots, 
  kTutExports, 
  kTutImports, 
  //
  kTutFinishedTwo, 
  kTutBreakTwo,
  // 
  kNTutorialStages
};

enum kToolType{kToolPickup, kToolInspect, kToolDestroy, kToolMap, kNToolTypes};
extern const uint16_t kToolUIIcon[];

#define PLAYER_ANIM_FRAMES 6
#define PLAYER_ANIM_DELAY 16

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};

struct Player_t{
  // Persistent
  uint8_t m_saveFormat;
  float m_pix_x; // Note: Centre
  float m_pix_y;
  int16_t m_camera_pix_x;
  int16_t m_camera_pix_y;
  uint32_t m_money;
  uint32_t m_moneyCumulative;
  uint32_t m_moneyHighWaterMark;
  uint32_t m_buildingsUnlockedTo; 
  uint32_t m_saveTime;
  uint32_t m_playTime;
  uint8_t m_tutorialProgress; // On each individual stage
  uint8_t m_paidTutorialMoney;
  uint16_t m_carryCargo[kNCargoType];
  uint16_t m_carryConveyor[kNConvSubTypes];
  uint16_t m_carryUtility[kNUtilitySubTypes];
  uint16_t m_carryPlant[kNPlantSubTypes];
  uint16_t m_carryExtractor[kNExtractorSubTypes];
  uint16_t m_carryFactory[kNFactorySubTypes];

  uint32_t m_soldCargo[kNCargoType];
  uint32_t m_importedCargo[kNCargoType];

  float m_exportPerWorld[WORLD_SAVE_SLOTS][kNCargoType];
  float m_soldPerWorld[WORLD_SAVE_SLOTS][kNCargoType];

  float m_sellPricePerWorld[WORLD_SAVE_SLOTS];

  uint16_t m_importConsumers[kNCargoType];

  // Settings. Persistent and NOT overwritten on reset
  uint8_t m_soundSettings;
  uint8_t m_musicVol;
  uint8_t m_autoUseConveyorBooster;
  uint8_t m_enableConveyorAnimation;
  uint8_t m_enableTutorial; // Also used to hold the tutorial stage
  uint8_t m_enableDebug;
  uint8_t m_enableAutosave;
  uint8_t m_enablePickupOnDestroy;
  uint8_t m_enableScreenShake;
  uint8_t m_enableExtractorOutlines;
  uint8_t m_enableSteps;
  uint8_t m_enableZoomWhenMove;
  uint8_t m_enableCrankConveyor;
  uint8_t m_enableCentreCamera;

  // Transient   
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDSprite* m_blueprint[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDSprite* m_blueprintRadius[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap1x1[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap3x3[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap5x5[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap7x7[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDBitmap* m_blueprintRadiusBitmap9x9[ZOOM_LEVELS]; // Index 0 not used - OWNED
  float m_vX;
  float m_vY;
  uint8_t m_infiniteMoney;
  uint16_t m_animID;
};

struct Player_t* getPlayer(void);

const char* toStringTool(enum kToolType _type);

const char* toStringToolInfo(enum kToolType _type);

float getTotalCargoExport(enum kCargoType _cargo);

float getThisWorldCargoExport(enum kCargoType _cargo);

uint16_t getCargoImportConsumers(enum kCargoType _cargo);

float getThisWorldCargoSales(void);

float getOtherWorldCargoSales(void);

void processOtherWorldCargoSales(int32_t _seconds);

bool modMoney(int32_t _amount);

void forceTorus(void);

uint32_t getTotalSoldCargo(void);

uint32_t getTotalImportedCargo(void);

struct Chunk_t* getCurrentChunk(void);

enum kChunkQuad getCurrentQuadrant(void);

struct Location_t* getPlayerLocation(void);

void setPlayerPosition(uint16_t _x, uint16_t _y, bool _updateCurrentLocation);

bool tutorialEnabled(void);

enum kUITutorialStage getTutorialStage(void);

void makeTutorialProgress(void);

uint8_t getTutorialProgress(void);

void nextTutorialStage(void);

void startPlotsTutorial(void);

void movePlayer(bool _forceUpdate);

void initPlayer(void);

void resetPlayer(void);

void setPlayerVisible(bool _visible);

void updateCameraWithZoom(uint8_t _prevZoom, uint8_t _newZoom);

void setDefaultPlayerSettings(void);

void serialisePlayer(struct json_encoder* je); 

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type);

void deserialiseArrayValuePlayer(json_decoder* jd, int _pos, json_value _value);