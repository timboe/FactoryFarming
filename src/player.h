#pragma once
#include "game.h"
#include "building.h"
#include "cargo.h"

enum kUITutorialStage{kTutWelcomeBuySeeds, kTutPlantCarrots, kTutGetCarrots, kTutSellCarrots, kTutBuildHarvester, kTutBuildConveyor, kTutBuildQuarry, kTutBuildVitamin, kTutFinishedOne, kTutBreakOne, kTut2A, kTut2B, kTut2C, kTut2D, kTutFinishedTwo, kTutBreakTwo, kNTutorialStages};

enum kToolType{kToolPickup, kToolInspect, kToolDestroy, kNToolTypes};
extern const uint16_t kToolUIIcon[];

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};

struct Player_t{
  // Persistent
  uint8_t m_saveFormat;
  float m_pix_x; // Note: Centre
  float m_pix_y;
  uint32_t m_money;
  uint32_t m_moneyCumulative;
  uint32_t m_moneyHighWaterMark;
  uint32_t m_moneyHighWaterMarkMenu; // Indicates how many unlocks the player has been shown up to
  uint32_t m_saveTime;
  uint32_t m_playTime;
  uint8_t m_tutorialProgress; // On each individual stage
  uint16_t m_carryCargo[kNCargoType];
  uint16_t m_carryConveyor[kNConvSubTypes];
  uint16_t m_carryUtility[kNUtilitySubTypes];
  uint16_t m_carryPlant[kNPlantSubTypes];
  uint16_t m_carryExtractor[kNExtractorSubTypes];
  uint16_t m_carryFactory[kNFactorySubTypes];

  float m_exportPerWorld[WORLD_SAVE_SLOTS][kNCargoType];

  uint16_t m_importConsumers[kNCargoType];

  // Settings. Persistent and NOT overwritten on reset
  uint8_t m_soundSettings;
  uint8_t m_autoUseConveyorBooster;
  uint8_t m_enableConveyorAnimation;
  uint8_t m_enableTutorial;
  uint8_t m_enableDebug;
  uint8_t m_enableAutosave;
  uint8_t m_enablePickupOnDestroy;
  uint8_t m_enableScreenShake;

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
};

struct Player_t* getPlayer(void);

const char* toStringTool(enum kToolType _type);

const char* toStringToolInfo(enum kToolType _type);

float getTotalCargoExport(enum kCargoType _cargo);

float getThisWorldCargoExport(enum kCargoType _cargo);

uint16_t getCargoImportConsumers(enum kCargoType _cargo);

bool modMoney(int32_t _amount);

int16_t getOffX(void);

int16_t getOffY(void);

void forceTorus(void);

struct Chunk_t* getCurrentChunk(void);

enum kChunkQuad getCurrentQuadrant(void);

struct Location_t* getPlayerLocation(void);

bool tutorialEnabled(void);

enum kUITutorialStage getTutorialStage(void);

void makeTutorialProgress(void);

uint8_t getTutorialProgress(void);

void nextTutorialStage(void);

bool movePlayer(void);

void initPlayer(void);

void resetPlayer(void);

void serialisePlayer(struct json_encoder* je); 

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type);

void deserialiseArrayValuePlayer(json_decoder* jd, int _pos, json_value _value);