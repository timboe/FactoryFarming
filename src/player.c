#include <math.h>
#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "render.h"
#include "input.h"
#include "sound.h"
#include "io.h"
#include "ui.h"
#include "generate.h"

//                             {kToolPickup, kToolInspect, kToolDestroy, kToolMap, kNToolTypes};
const uint16_t kToolUIIcon[] = {SID(8,10),   SID(9,10),    SID(1,16),    SID(14,17)};

struct Player_t m_player;

int16_t m_offX, m_offY = 0; // Screen offsets

uint8_t m_facing = 0, m_wasFacing = 0, m_stepCounter = 0, m_animFrame = 0;

bool m_inWater = false, m_wasInWater = false;

uint16_t m_deserialiseXPlayer = 0, m_deserialiseYPlayer = 0;
int16_t m_deserialiseArrayID = -1;

struct Chunk_t* m_currentChunk = NULL;

enum kChunkQuad m_quadrant = 0;

struct Location_t* m_currentLocation = NULL;

bool m_topT = true;
bool m_leftT = true;
bool m_forceTorus = true;

void movePlayerPosition(float _goalX, float _goalY);

void updatePlayerPosition(uint8_t _zoom);

void playerSpriteSetup(void);

struct Chunk_t* computeCurrentChunk(void);

enum kChunkQuad computeCurrentQuadrant(void);

void checkTorus(void);

/// ///

const char* toStringTool(enum kToolType _type) {
  switch(_type) {
    case kToolPickup: return tr(kTRPick);
    case kToolInspect: return tr(kTRInspect);
    case kToolDestroy: return tr(kTRDestroy);
    case kToolMap: return tr(kTRMap);
    default: return "Tool???";
  }
}

const char* toStringToolInfo(enum kToolType _type) {
  switch(_type) {
    case kToolPickup: return tr(kTRPickHelp);
    case kToolInspect: return tr(kTRInspectHelp);
    case kToolDestroy: return tr(kTRDestroyHelp);
    case kToolMap: return tr(kTRMapHelp);
    default: return "Tool???";
  }
}

float getTotalCargoExport(enum kCargoType _cargo) {
  float ret = 0.0f;
  for (int32_t i = 0; i < WORLD_SAVE_SLOTS; ++i) {      
    ret += m_player.m_exportPerWorld[i][_cargo];
  }
  return ret;
}

float getThisWorldCargoExport(enum kCargoType _cargo) {
  return m_player.m_exportPerWorld[getSlot()][_cargo];
}

// This part computes the total value of sold cargo
float getOtherWorldCargoSales() {
  float ret = 0.0f;
  const int8_t s = getSlot();
  for (int32_t i = 0; i < WORLD_SAVE_SLOTS; ++i) {
    if (i == s) continue;
    ret += m_player.m_sellPricePerWorld[i];
  }
  return ret;
}

// This part computes the statistics part of cargo sold off-world
// We do this such that we can still work towards goals with the world not loaded
void processOtherWorldCargoSales(int32_t _seconds) {
  static float fractional[kNCargoType] = {0.0f}; // Keep track of partial cargo sales too
  for (int32_t i = 0; i < WORLD_SAVE_SLOTS; ++i) {
    if (i == getSlot()) continue;
    for (int32_t c = 1; c < kNCargoType; ++c) { // Start at 1 as 0=kNoCargo
      fractional[c] += (float)(m_player.m_soldPerWorld[i][c] * _seconds);
    }
  }
  // Accumulate integer statistics
  for (int32_t c = 1; c < kNCargoType; ++c) { // Start at 1 as 0=kNoCargo
    const int32_t soldInt = (int32_t) fractional[c];
    fractional[c] -= (float)soldInt;
    m_player.m_soldCargo[ c ] += soldInt;
    //if (soldInt) pd->system->logToConsole("-- Add bonus sold %s = %i", toStringCargoByType(c, true), soldInt);
  }  
}

float getThisWorldCargoSales() {
  return m_player.m_sellPricePerWorld[getSlot()];
}

uint16_t getCargoImportConsumers(enum kCargoType _cargo) {
  return m_player.m_importConsumers[_cargo];
}

enum kUITutorialStage getTutorialStage() {
  return (enum kUITutorialStage) m_player.m_enableTutorial;
}

bool tutorialEnabled() {
  return (m_player.m_tutorialProgress != TUTORIAL_DISABLED);
} 

void makeTutorialProgress() {
  ++m_player.m_tutorialProgress;
}

uint8_t getTutorialProgress() {
  return m_player.m_tutorialProgress;
}

void startPlotsTutorial() {
  if (!tutorialEnabled()) return;
  m_player.m_tutorialProgress = 0;
  m_player.m_enableTutorial = kTutBreakOne+1;
  showTutorialMsg(m_player.m_enableTutorial);
}

void nextTutorialStage() {
  if (m_player.m_enableTutorial != kTutSeeObjective) { // We let progress on this one task carry forward
    m_player.m_tutorialProgress = 0;
  }
  ++m_player.m_enableTutorial;
  int32_t toGive = 0;
  if (m_player.m_enableTutorial == kTutBreakOne || m_player.m_enableTutorial == kTutBreakTwo) {
    m_player.m_enableTutorial = TUTORIAL_FINISHED;
    return;
  } else if (m_player.m_enableTutorial == kTutGetCarrots) {
    growAtAll(TICKS_PER_SEC * 2); // Force all plants to grow
  } else if (m_player.m_enableTutorial == kTutBuildHarvester) {
    toGive = EDesc[kCropHarvesterSmall].price * 2;
  } else if (m_player.m_enableTutorial == kTutBuildConveyor) {
    toGive = CDesc[kBelt].price * 100;
  } else if (m_player.m_enableTutorial == kTutBuildQuarry) {
    toGive = EDesc[kChalkQuarry].price * 2;
  } else if (m_player.m_enableTutorial == kTutBuildVitamin) {
    toGive = FDesc[kVitiminFac].price * 2;
  }

  // If changing toGive items, change tutMoney also in settings
  if (toGive) {
    #ifdef DEV
    pd->system->logToConsole("Tut give player %i", toGive);
    #endif
    modMoney( toGive );
  }

  showTutorialMsg(m_player.m_enableTutorial);
}

struct Player_t* getPlayer() {
  return &m_player;
}

struct Chunk_t* getCurrentChunk() {
  return m_currentChunk;
}

enum kChunkQuad getCurrentQuadrant() {
  return m_quadrant;
}

struct Location_t* getPlayerLocation() {
  return m_currentLocation;
}

void setPlayerPosition(uint16_t _x, uint16_t _y, bool _updateCurrentLocation) {
  // keep track of camera offset and preserve it
  const int16_t cOffX = m_player.m_camera_pix_x - m_player.m_pix_x;
  const int16_t cOffY = m_player.m_camera_pix_y - m_player.m_pix_y; 
  pd->sprite->moveTo(m_player.m_sprite[1], _x, _y); // Note: Hard coded two zoom levels
  pd->sprite->moveTo(m_player.m_sprite[2], _x * 2, _y * 2);
  updatePlayerPosition(getZoom());
  // reset camera
  m_player.m_camera_pix_x = (int16_t)m_player.m_pix_x + cOffX;
  m_player.m_camera_pix_y = (int16_t)m_player.m_pix_y + cOffY;
  if (_updateCurrentLocation) {
    m_currentChunk = computeCurrentChunk();
    m_currentLocation = getLocation(m_player.m_pix_x / TILE_PIX, m_player.m_pix_y / TILE_PIX);
    // DON'T setTorus here (not 100% why, but it breaks building hitboxes)
    #ifdef DEV
    pd->system->logToConsole("CHUNKCHANGE (set) C:%i %i (%i)", m_currentChunk->m_x, m_currentChunk->m_y, m_quadrant);
    #endif
    if (!IOOperationInProgress()) {
      updateRenderList();
    }
  }
}

void movePlayerPosition(float _goalX, float _goalY) {
  int len;
  uint8_t zoom = getZoom();
  SpriteCollisionInfo* collInfo = pd->sprite->moveWithCollisions(m_player.m_sprite[zoom], _goalX * zoom, _goalY * zoom, &(m_player.m_pix_x), &(m_player.m_pix_y), &len);
  if (len) pd->system->realloc(collInfo, 0); // Free
  updatePlayerPosition(zoom);
}

void updatePlayerPosition(uint8_t _zoom) {
  pd->sprite->getPosition(m_player.m_sprite[_zoom], &(m_player.m_pix_x), &(m_player.m_pix_y));
  m_player.m_pix_x = m_player.m_pix_x / _zoom;
  m_player.m_pix_y = m_player.m_pix_y / _zoom;
  //pd->system->logToConsole("P@ %f %f", m_player.m_pix_x , m_player.m_pix_y);
}

void updateCameraWithZoom(uint8_t _prevZoom, uint8_t _newZoom) {
  const float scalingFactor = _prevZoom / (float)_newZoom;
  const float cOffX = (m_player.m_camera_pix_x - m_player.m_pix_x) * scalingFactor;
  const float cOffY = (m_player.m_camera_pix_y - m_player.m_pix_y) * scalingFactor; 
  m_player.m_camera_pix_x = m_player.m_pix_x + cOffX;
  m_player.m_camera_pix_y = m_player.m_pix_y + cOffY;
  // Set a consistant sprite frame too
  pd->sprite->setImage(m_player.m_sprite[_newZoom], getSprite18_byidx(m_player.m_animID, _newZoom), kBitmapUnflipped);
}

struct Chunk_t* computeCurrentChunk() {
  int16_t effX = m_player.m_camera_pix_x; // Get effective (wrapped one way) camera coordinate, not letting it go -ve
  int16_t effY = m_player.m_camera_pix_y; 
  if (m_player.m_camera_pix_x < 0) effX += TOT_WORLD_PIX_X;
  if (m_player.m_camera_pix_y < 0) effY += TOT_WORLD_PIX_Y;

  int16_t chunkX = (effX / (CHUNK_PIX_X)) % WORLD_CHUNKS_X;
  int16_t chunkY = (effY / (CHUNK_PIX_Y)) % WORLD_CHUNKS_Y;

  return getChunk(chunkX, chunkY);
}

enum kChunkQuad computeCurrentQuadrant() {
  int16_t effX = m_player.m_camera_pix_x; // Get effective (wrapped one way) camera coordinate, not letting it go -ve
  int16_t effY = m_player.m_camera_pix_y; 
  if (m_player.m_camera_pix_x < 0) effX += TOT_WORLD_PIX_X;
  if (m_player.m_camera_pix_y < 0) effY += TOT_WORLD_PIX_Y;

  int16_t subChunkX = effX / (CHUNK_PIX_X/2) % 2;
  int16_t subChunkY = effY / (CHUNK_PIX_Y/2) % 2;
  enum kChunkQuad quadrant = NW;
  if (subChunkX && subChunkY) {
    quadrant = SE;
  } else if (subChunkX) {
    quadrant = NE;
  } else if (subChunkY) {
    quadrant = SW;
  }

  return quadrant;
}

void checkTorus() {
  // Torus splitting
  bool torusChanged = false;
  if (m_topT && m_player.m_pix_y > TOT_WORLD_PIX_Y/2) {
    m_topT = false;
    torusChanged = true;
  } else if (!m_topT && m_player.m_pix_y <= TOT_WORLD_PIX_Y/2) {
    m_topT = true;
    torusChanged = true;
  }

  if (m_leftT && m_player.m_pix_x > TOT_WORLD_PIX_X/2) {
    m_leftT = false;
    torusChanged = true;
  } else if (!m_leftT && m_player.m_pix_x <= TOT_WORLD_PIX_X/2) {
    m_leftT = true;
    torusChanged = true;
  }

  if (torusChanged || m_forceTorus) {
    chunkShiftTorus(m_topT, m_leftT);
    m_forceTorus = false;
  }
}

void movePlayer(bool _forceUpdate) {

  // Note: This may set the zoom
  if (m_player.m_enableZoomWhenMove && !_forceUpdate) {
    static uint8_t delay = 0;
    if (getPressedAny()) {
      delay = 32;
      setZoom(2);
    } else if (delay > 0 && --delay == 0) {
      setZoom(1);
    }
  }

  uint8_t zoom = getZoom();

  // Do conveyor movement
  static uint8_t convNoise = 0;
  int8_t convMotion = getAndReduceFollowConveyor();
  if (m_player.m_enableCrankConveyor &&  convMotion > 0) { // Make this just "if (convMotion)" to allow backwards travel too
    if (m_currentLocation && m_currentLocation->m_building && m_currentLocation->m_building->m_type == kConveyor) {
      enum kDir direction;
      if (++convNoise % 8 == 0) {
        sfx(kSfxConveyor);
      }
      if (m_currentLocation->m_building->m_subType.conveyor >= kFilterL) {
        direction = m_currentLocation->m_building->m_nextDir[0];  
      } else {
        direction = m_currentLocation->m_building->m_nextDir[m_currentLocation->m_building->m_mode.mode16];
      }
      int8_t dist = -NEAR_TICK_AMOUNT * m_currentLocation->m_building->m_stored[0]; // Holds speed multiplier
      if (convMotion > 0) dist *= -2; // Speed boost when going forwards (also negates the above invesion)
      int16_t x = 0, y = 0;
      switch (direction) {
        case SN: x = m_player.m_pix_x; y = m_player.m_pix_y - dist; m_facing = 2; break;
        case NS: x = m_player.m_pix_x; y = m_player.m_pix_y + dist; m_facing = 3; break;
        case EW: x = m_player.m_pix_x - dist; y = m_player.m_pix_y; m_facing = 0; break;
        case WE: x = m_player.m_pix_x + dist; y = m_player.m_pix_y; m_facing = 1; break;
        case kDirN: break;
      }
      if (x < 0) {
        x += TOT_WORLD_PIX_X;
      } else if (x >= TOT_WORLD_PIX_X) {
        x -= TOT_WORLD_PIX_X;
      }
      if (y < 0) {
        y += TOT_WORLD_PIX_Y;
      } else if (y >= TOT_WORLD_PIX_Y) {
        y -= TOT_WORLD_PIX_Y;
      }
      setPlayerPosition(x, y, /*update current location = */ false);
      uint8_t i_n = direction == WE ? 5 : 0;
      m_player.m_animID = SPRITE18_ID(i_n, m_facing);
      pd->sprite->setImage(m_player.m_sprite[zoom], getSprite18_byidx(m_player.m_animID, zoom), kBitmapUnflipped);
    }
  }

  //updatePlayerPosition();
  float goalX = m_player.m_pix_x;
  float goalY = m_player.m_pix_y;

  float acc = PLAYER_A;
  float fric = PLAYER_FRIC;
  m_inWater = false;
  if (m_currentLocation != NULL) {
    if (isWaterTile(m_currentLocation->m_x, m_currentLocation->m_y)) {
      fric *= 0.5f;
      m_inWater = true;
    } else if (m_currentLocation->m_building && m_currentLocation->m_building->m_type == kUtility && m_currentLocation->m_building->m_subType.utility == kPath) {
      acc *= 1.3f;
    }
  }
  if (bPressed()) acc *= 1.5f;
  if (zoom == 2) acc *= 0.9f;

  float diffX = 0;
  float diffY = 0;

  if (getPressed(0)) diffX -= acc;
  if (getPressed(1)) diffX += acc;
  if (getPressed(2)) diffY -= acc;
  if (getPressed(3)) diffY += acc;

  // Note floating point == 0 check, hopefully work in this case
  if (diffX && diffY) {
    diffX *= SQRT_HALF;
    diffY *= SQRT_HALF;
  }

  m_player.m_vX = (m_player.m_vX + diffX) * fric;
  m_player.m_vY = (m_player.m_vY + diffY) * fric;

  static uint16_t movingTicksX = 0, movingTicksY = 0;
  if ((float)(fabs(m_player.m_vX) + fabs(m_player.m_vY)) > 0.1f) {
    if (m_player.m_vX) ++movingTicksX;
    if (m_player.m_vY) ++movingTicksY;
    if (fabs(m_player.m_vX) > fabs(m_player.m_vY)) {
      m_facing = (m_player.m_vX > 0 ? 1 : 0);
    } else {
      m_facing = (m_player.m_vY > 0 ? 3 : 2);
    }
  }
  if (!diffX) movingTicksX = 0;
  if (!diffY) movingTicksY = 0;

  goalX += m_player.m_vX;
  goalY += m_player.m_vY; 

  if (movingTicksX || movingTicksY || _forceUpdate) {
    if (++m_stepCounter * acc > PLAYER_ANIM_DELAY || m_facing != m_wasFacing || m_inWater != m_wasInWater) {
      m_animFrame = (m_animFrame + 1) % PLAYER_ANIM_FRAMES;
      m_stepCounter = 0;
      const uint16_t animY = m_inWater ? m_facing + 4 : m_facing;
      if (m_animFrame % 3 == 0 && m_player.m_enableSteps) sfx( (m_inWater ? kFootstepWater : kFootstepDefault) + rand() % N_FOOTSTEPS);
      m_player.m_animID = SPRITE18_ID(m_animFrame, animY);
      pd->sprite->setImage(m_player.m_sprite[zoom], getSprite18_byidx(m_player.m_animID, zoom), kBitmapUnflipped);
    }
    m_wasFacing = m_facing;
    m_wasInWater = m_inWater;
  }

  //pd->system->logToConsole("GOAL %f %f CURRENT %f %f", goalX, goalY, m_player.m_x, m_player.m_y);

  movePlayerPosition(goalX, goalY);

  // Don#t need to update position as the code below will pick up on this
  if (m_player.m_pix_x > TOT_WORLD_PIX_X) {
    setPlayerPosition(m_player.m_pix_x - TOT_WORLD_PIX_X, m_player.m_pix_y, /*update current location = */ false);
  } else if (m_player.m_pix_x < 0) {
    setPlayerPosition(m_player.m_pix_x + TOT_WORLD_PIX_X, m_player.m_pix_y, /*update current location = */ false);
  }

  if (m_player.m_pix_y > TOT_WORLD_PIX_Y) {
    setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y - TOT_WORLD_PIX_Y, /*update current location = */ false);
  } else if (m_player.m_pix_y < 0) {
    setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y + TOT_WORLD_PIX_Y, /*update current location = */ false);
  }

  if (m_player.m_enableCentreCamera) {

    m_player.m_camera_pix_x = m_player.m_pix_x;
    m_player.m_camera_pix_y = m_player.m_pix_y;

  } else {

    float shrinkFractionX = 1.0f - (movingTicksX / (float)(SCROLL_LOCATION_MODIFICATION_TIME));
    float shrinkFractionY = 1.0f - (movingTicksY / (float)(SCROLL_LOCATION_MODIFICATION_TIME));

    if (shrinkFractionX < 0.0f) shrinkFractionX = 0.0f;
    if (shrinkFractionY < 0.0f) shrinkFractionY = 0.0f;

    if (!diffY) {
      shrinkFractionY = 1.0f;
    }
    if (!diffX) {
      shrinkFractionX = 1.0f;
    }

    bool didScroll = false;

    const static int16_t MOVE_THRESHOLD_X = (SCREEN_PIX_X * SCROLL_EDGE) - (SCREEN_PIX_X/2); 
    const int16_t sideThreshold = (int16_t) roundf(MOVE_THRESHOLD_X * shrinkFractionX);
    if (m_player.m_pix_x - m_player.m_camera_pix_x > sideThreshold / zoom && diffX > 0) {
      m_player.m_camera_pix_x = m_player.m_pix_x - (sideThreshold / zoom);
      didScroll = true;
    } else if (m_player.m_pix_x - m_player.m_camera_pix_x < -(sideThreshold / zoom) && diffX < 0) {
      didScroll = true;
      m_player.m_camera_pix_x = m_player.m_pix_x + (sideThreshold / zoom);
    }
    if (!didScroll) {
      movingTicksX = 0;
    }

    didScroll = false;
    const static int16_t MOVE_THRESHOLD_Y = (SCREEN_PIX_Y * SCROLL_EDGE_TOP_BOT) - (SCREEN_PIX_Y/2);
    const int16_t topThreshold = (int16_t) roundf(MOVE_THRESHOLD_Y * shrinkFractionY);
    const static int16_t MOVE_THRESHOLD_Y_REDUCED = (SCREEN_PIX_Y * SCROLL_EDGE_BOT_TUT) - (SCREEN_PIX_Y/2);    
    #ifdef DEMO
    const int16_t bottomThreshold = (int16_t) roundf((getGameMode() == kInspectMode ? MOVE_THRESHOLD_Y_REDUCED : MOVE_THRESHOLD_Y) * shrinkFractionY);
    #else
    const int16_t bottomThreshold = (int16_t) roundf((m_player.m_enableTutorial < TUTORIAL_FINISHED || getGameMode() == kInspectMode ? MOVE_THRESHOLD_Y_REDUCED : MOVE_THRESHOLD_Y) * shrinkFractionY);
    #endif

    if (m_player.m_pix_y - m_player.m_camera_pix_y > bottomThreshold / zoom && diffY > 0) {
      didScroll = true;
      m_player.m_camera_pix_y = m_player.m_pix_y - (bottomThreshold / zoom); 
    } else if (m_player.m_pix_y - m_player.m_camera_pix_y < -(topThreshold / zoom) && diffY < 0) {
      didScroll = true;
      m_player.m_camera_pix_y = m_player.m_pix_y + (topThreshold / zoom);
    }
    if (!didScroll) {
      movingTicksY = 0;
    }

  }

  // Check chunk change
  struct Chunk_t* cameraChunk = computeCurrentChunk();
  enum kChunkQuad cameraQuad = computeCurrentQuadrant();

  checkTorus();

  bool chunkChange = (cameraChunk != m_currentChunk);
  if (chunkChange || cameraQuad != m_quadrant) {
    m_currentChunk = cameraChunk;
    m_quadrant = cameraQuad;
    if (zoom == 2 || chunkChange) { // When zoomed out, only need to call when actually changing chunks
      #ifdef DEV
      pd->system->logToConsole("CHUNKCHANGE (move) %u %u (%u)", m_currentChunk->m_x, m_currentChunk->m_y, m_quadrant);
      #endif
      updateRenderList();
    }
  }

  struct Location_t* wasAt = m_currentLocation;
  m_currentLocation = getLocation(m_player.m_pix_x / TILE_PIX, m_player.m_pix_y / TILE_PIX);

  if (wasAt != m_currentLocation || _forceUpdate) {
    int32_t bpX = (TILE_PIX*m_currentLocation->m_x + TILE_PIX/2.0) * zoom, bpY = (TILE_PIX*m_currentLocation->m_y  + TILE_PIX/2.0) * zoom;
    pd->sprite->moveTo(m_player.m_blueprint[zoom], bpX, bpY);
    pd->sprite->moveTo(m_player.m_blueprintRadius[zoom], bpX, bpY);
  }

  if (m_player.m_navHint) {
    int32_t bpX = m_player.m_pix_x * zoom, bpY = m_player.m_pix_y * zoom;
    #define HINT_D ((TILE_PIX*3)/2) 
    switch (m_player.m_navHintTopID) {
      case 0: case 8: bpY -= HINT_D*zoom; break;
      case 1: bpX += HINT_D*zoom; bpY -= HINT_D*zoom; break;
      case 2: bpX += HINT_D*zoom; break;
      case 3: bpX += HINT_D*zoom; bpY += HINT_D*zoom; break;
      case 4: bpY += HINT_D*zoom; break;
      case 5: bpX -= HINT_D*zoom; bpY += HINT_D*zoom; break;
      case 6: bpX -= HINT_D*zoom; break;
      case 7: bpX -= HINT_D*zoom; bpY -= HINT_D*zoom; break;
    }
    pd->sprite->moveTo(m_player.m_navGuideTop[zoom], bpX, bpY);
    pd->sprite->moveTo(m_player.m_navGuideBot[zoom], bpX, bpY);
  }

}

bool modMoney(int32_t _amount) {
  if (m_player.m_infiniteMoney) {
    return true;
  }
  if (_amount < 0 && (_amount * -1) > m_player.m_money) {
    return false;
  }
  m_player.m_money += _amount;
  if (m_player.m_money > m_player.m_moneyHighWaterMark) {
    m_player.m_moneyHighWaterMark = m_player.m_money;
  }
  if (_amount > 0) {
    m_player.m_moneyCumulative += _amount;
  }
  UIDirtyRight();
  return true;
}

uint32_t getTotalSoldCargo() {
  uint32_t t = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) {
    t += m_player.m_soldCargo[i];
  }
  return t;
}

uint32_t getTotalImportedCargo() {
  uint32_t t = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) {
    t += m_player.m_importedCargo[i];
  }
  return t;
}

SpriteCollisionResponseType playerLCDSpriteCollisionFilterProc(LCDSprite* _player, LCDSprite* _other) {
  return kCollisionTypeSlide;
}

void playerSpriteSetup() {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    m_player.m_sprite[zoom] = pd->sprite->newSprite();
    const PDRect pBound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = 18*zoom};
    const PDRect cBound = {.x = (COLLISION_OFFSET_SMALL/2)*zoom, .y = (COLLISION_OFFSET_SMALL/2)*zoom, .width = (TILE_PIX - COLLISION_OFFSET_SMALL)*zoom, .height = (TILE_PIX - COLLISION_OFFSET_SMALL)*zoom};
    pd->sprite->setBounds(m_player.m_sprite[zoom], pBound);
    pd->sprite->setImage(m_player.m_sprite[zoom], getSprite18(0, 3, zoom), kBitmapUnflipped);
    pd->sprite->setCollideRect(m_player.m_sprite[zoom], cBound);
    pd->sprite->setCollisionResponseFunction(m_player.m_sprite[zoom], playerLCDSpriteCollisionFilterProc);
    pd->sprite->setZIndex(m_player.m_sprite[zoom], Z_INDEX_PLAYER);

    m_player.m_blueprint[zoom] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_player.m_blueprint[zoom], pBound);
    pd->sprite->setImage(m_player.m_blueprint[zoom], getSprite16(0, 0, zoom), kBitmapUnflipped);
    pd->sprite->setZIndex(m_player.m_blueprint[zoom], Z_INDEX_BLUEPRINT);

    m_player.m_blueprintRadiusBitmap1x1[zoom] = pd->graphics->newBitmap(TILE_PIX*1*zoom, TILE_PIX*1*zoom, kColorClear);
    m_player.m_blueprintRadiusBitmap3x3[zoom] = pd->graphics->newBitmap(TILE_PIX*3*zoom, TILE_PIX*3*zoom, kColorClear);
    m_player.m_blueprintRadiusBitmap5x5[zoom] = pd->graphics->newBitmap(TILE_PIX*5*zoom, TILE_PIX*5*zoom, kColorClear);
    m_player.m_blueprintRadiusBitmap7x7[zoom] = pd->graphics->newBitmap(TILE_PIX*7*zoom, TILE_PIX*7*zoom, kColorClear);
    m_player.m_blueprintRadiusBitmap9x9[zoom] = pd->graphics->newBitmap(TILE_PIX*9*zoom, TILE_PIX*9*zoom, kColorClear);

    uint16_t start = 2*zoom, stop = (TILE_PIX*1 - 2)*zoom;

    pd->graphics->setLineCapStyle(kLineCapStyleRound);
    pd->graphics->pushContext(m_player.m_blueprintRadiusBitmap1x1[zoom]);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawLine(start, start, stop, start, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, start, start, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, stop, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(stop, start, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->popContext();

    stop = (TILE_PIX*3 - 2)*zoom;
    pd->graphics->setLineCapStyle(kLineCapStyleRound);
    pd->graphics->pushContext(m_player.m_blueprintRadiusBitmap3x3[zoom]);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawLine(start, start, stop, start, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, start, start, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, stop, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(stop, start, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->popContext();

    stop = (TILE_PIX*5 - 2)*zoom;
    pd->graphics->pushContext(m_player.m_blueprintRadiusBitmap5x5[zoom]);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawLine(start, start, stop, start, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, start, start, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, stop, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(stop, start, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->popContext();

    stop = (TILE_PIX*7 - 2)*zoom;
    pd->graphics->pushContext(m_player.m_blueprintRadiusBitmap7x7[zoom]);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawLine(start, start, stop, start, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, start, start, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, stop, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(stop, start, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->popContext();

    stop = (TILE_PIX*9 - 2)*zoom;
    pd->graphics->pushContext(m_player.m_blueprintRadiusBitmap9x9[zoom]);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->graphics->drawLine(start, start, stop, start, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, start, start, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(start, stop, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->drawLine(stop, start, stop, stop, 2*zoom, kColorBlack);
    pd->graphics->popContext();

    m_player.m_blueprintRadius[zoom] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_player.m_blueprintRadius[zoom], pBound);
    pd->sprite->setImage(m_player.m_blueprintRadius[zoom], m_player.m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped);
    pd->sprite->setZIndex(m_player.m_blueprintRadius[zoom], Z_INDEX_BLUEPRINT);

    const PDRect nBound = {.x = 0, .y = 0, .width = TILE_PIX*3*zoom, .height = TILE_PIX*3*zoom};
    m_player.m_navGuideTop[zoom] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_player.m_navGuideTop[zoom], nBound);
    pd->sprite->setImage(m_player.m_navGuideTop[zoom], getSpriteNavGuideTop(0, zoom), kBitmapUnflipped);
    pd->sprite->setZIndex(m_player.m_navGuideTop[zoom], Z_INDEX_UI_TTT);

    m_player.m_navGuideBot[zoom] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_player.m_navGuideBot[zoom], nBound);
    pd->sprite->setImage(m_player.m_navGuideBot[zoom], getSpriteNavGuideBot(0, zoom), kBitmapUnflipped);
    pd->sprite->setZIndex(m_player.m_navGuideBot[zoom], Z_INDEX_UI_TT);
  }
}

void forceTorus() { m_forceTorus = true; }

void resetPlayer() {
  m_player.m_money = STARTING_MONEY;
  m_player.m_moneyCumulative = 0;
  m_player.m_buildingsUnlockedTo = 0;
  m_player.m_saveTime = pd->system->getSecondsSinceEpoch(NULL);
  m_player.m_playTime = 0;
  m_player.m_tutorialProgress = 0; // Note: not tutorial _stage_ (this is in m_enableTutorial)
  m_player.m_infiniteMoney = false;
  m_player.m_animID = 0;
  m_player.m_pix_x = 0;
  m_player.m_pix_y = 0;
  if (m_player.m_enableTutorial != TUTORIAL_DISABLED) m_player.m_enableTutorial = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) m_player.m_carryCargo[i] = 0;
  for (int32_t i = 0; i < kNConvSubTypes; ++i) m_player.m_carryConveyor[i] = 0;
  for (int32_t i = 0; i < kNUtilitySubTypes; ++i) m_player.m_carryUtility[i] = 0;
  for (int32_t i = 0; i < kNPlantSubTypes; ++i) m_player.m_carryPlant[i] = 0;
  for (int32_t i = 0; i < kNExtractorSubTypes; ++i) m_player.m_carryExtractor[i] = 0;
  for (int32_t i = 0; i < kNFactorySubTypes; ++i) m_player.m_carryFactory[i] = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) m_player.m_soldCargo[i] = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) m_player.m_importedCargo[i] = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) m_player.m_importConsumers[i] = 0;
  for (int32_t i = 0; i < WORLD_SAVE_SLOTS; ++i) {
    m_player.m_sellPricePerWorld[i] = 0;
    for (int32_t c = 0; c < kNCargoType; ++c) {
      m_player.m_exportPerWorld[i][c] = 0;
      m_player.m_soldPerWorld[i][c] = 0;
    }
  }
  setPlayerPosition((TOT_WORLD_PIX_X/2) + SCREEN_PIX_X/4, (TOT_WORLD_PIX_Y/2) + (3*SCREEN_PIX_Y)/4, /*update current location = */ true);
  m_player.m_camera_pix_x = m_player.m_pix_x;
  m_player.m_camera_pix_y = m_player.m_pix_y;
  m_facing = 0;
  m_wasFacing = 0;
  m_stepCounter = 0;
  m_animFrame = 0;
  m_deserialiseXPlayer = 0;
  m_deserialiseYPlayer = 0;
  m_deserialiseArrayID = -1;
}

void setPlayerVisible(bool _visible) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    pd->sprite->setVisible(m_player.m_sprite[zoom], _visible);
    pd->sprite->setCollisionsEnabled(m_player.m_sprite[zoom], _visible);
  }
}

void setDefaultPlayerSettings() {
  m_player.m_soundSettings = 3;
  m_player.m_musicVol = 100;
  m_player.m_autoUseConveyorBooster = 0;
  m_player.m_enableConveyorAnimation = 0;
  m_player.m_enableTutorial = 0;
  m_player.m_enableDebug = 0;
  m_player.m_enableAutosave = 10;
  m_player.m_enablePickupOnDestroy = 1;
  m_player.m_enableScreenShake = 1;
  m_player.m_enableExtractorOutlines = 1;
  m_player.m_enableSteps = 1;
  m_player.m_enableZoomWhenMove = 0;
  m_player.m_enableCrankConveyor = 1; 
  m_player.m_enableCentreCamera = 0;
}

void initPlayer() {
  playerSpriteSetup();
}

void serialisePlayer(struct json_encoder* je) {
  je->addTableMember(je, "player", 6);
  je->startTable(je);
  je->addTableMember(je, "sf", 2); // Save format
  je->writeInt(je, CURRENT_SAVE_FORMAT);
  je->addTableMember(je, "x", 1);
  je->writeInt(je, (int) m_player.m_pix_x);
  je->addTableMember(je, "y", 1);
  je->writeInt(je, (int) m_player.m_pix_y);
  je->addTableMember(je, "m", 1);
  je->writeInt(je, m_player.m_money);
  je->addTableMember(je, "mhwm", 4);
  je->writeInt(je, m_player.m_moneyHighWaterMark);
  je->addTableMember(je, "but", 3);
  je->writeInt(je, m_player.m_buildingsUnlockedTo);
  je->addTableMember(je, "mc", 2);
  je->writeInt(je, m_player.m_moneyCumulative);
  je->addTableMember(je, "st", 2);
  je->writeInt(je, pd->system->getSecondsSinceEpoch(NULL));
  je->addTableMember(je, "pt", 2);
  je->writeInt(je, m_player.m_playTime);
  je->addTableMember(je, "tutstage", 8);
  je->writeInt(je, m_player.m_tutorialProgress);
  je->addTableMember(je, "tutpaid", 7);
  je->writeInt(je, m_player.m_paidTutorialMoney);

  je->addTableMember(je, "slot", 4);
  je->writeInt(je, getSlot());

  // Settings - not reset new games
  je->addTableMember(je, "sets", 4);
  je->writeInt(je, m_player.m_soundSettings);
  je->addTableMember(je, "setm", 4);
  je->writeInt(je, m_player.m_musicVol);
  je->addTableMember(je, "setb", 4);
  je->writeInt(je, m_player.m_autoUseConveyorBooster);
  je->addTableMember(je, "setc", 4);
  je->writeInt(je, m_player.m_enableConveyorAnimation); 
  je->addTableMember(je, "sett", 4);
  je->writeInt(je, m_player.m_enableTutorial);
  je->addTableMember(je, "setd", 4);
  je->writeInt(je, m_player.m_enableDebug);
  je->addTableMember(je, "seta", 4);
  je->writeInt(je, m_player.m_enableAutosave);
  je->addTableMember(je, "setp", 4);
  je->writeInt(je, m_player.m_enablePickupOnDestroy);
  je->addTableMember(je, "seth", 4);
  je->writeInt(je, m_player.m_enableScreenShake);
  je->addTableMember(je, "seto", 4);
  je->writeInt(je, m_player.m_enableExtractorOutlines);
  je->addTableMember(je, "sete", 4);
  je->writeInt(je, m_player.m_enableSteps);
  je->addTableMember(je, "setz", 4);
  je->writeInt(je, m_player.m_enableZoomWhenMove);
  je->addTableMember(je, "setr", 4);
  je->writeInt(je, m_player.m_enableCrankConveyor);
  je->addTableMember(je, "setn", 4);
  je->writeInt(je, m_player.m_enableCentreCamera);
  
  
  je->addTableMember(je, "cargos", 6);
  je->startArray(je);
  for (int32_t i = 0; i < kNCargoType; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_carryCargo[i]);
  }
  je->endArray(je);

  je->addTableMember(je, "convs", 5);
  je->startArray(je);
  for (int32_t i = 0; i < kNConvSubTypes; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_carryConveyor[i]);
  }
  je->endArray(je);

  je->addTableMember(je, "plants", 6);
  je->startArray(je);
  for (int32_t i = 0; i < kNPlantSubTypes; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_carryPlant[i]);
  }
  je->endArray(je);

  je->addTableMember(je, "util", 4);
  je->startArray(je);
  for (int32_t i = 0; i < kNUtilitySubTypes; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_carryUtility[i]);
  }
  je->endArray(je);

  je->addTableMember(je, "xtrcts", 6);
  je->startArray(je);
  for (int32_t i = 0; i < kNExtractorSubTypes; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_carryExtractor[i]);
  }
  je->endArray(je);


  je->addTableMember(je, "facts", 5);
  je->startArray(je);
  for (int32_t i = 0; i < kNFactorySubTypes; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_carryFactory[i]);
  }
  je->endArray(je);

  je->addTableMember(je, "scargo", 6);
  je->startArray(je);
  for (int32_t i = 0; i < kNCargoType; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_soldCargo[i]);
  }
  je->endArray(je);

  je->addTableMember(je, "icargo", 6);
  je->startArray(je);
  for (int32_t i = 0; i < kNCargoType; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_importedCargo[i]);
  }
  je->endArray(je);

  for (int32_t w = 0; w < WORLD_SAVE_SLOTS; ++w) {
    char txt[12] = "";
    snprintf(txt, 12, "expw%02i", (int)w);
    je->addTableMember(je, txt, 6);
    je->startArray(je);
    for (int32_t i = 0; i < kNCargoType; ++i) {
      je->addArrayMember(je);
      je->writeDouble(je, m_player.m_exportPerWorld[w][i]);
      #ifdef DEV // zzz v1.5 update checks
      if (m_player.m_exportPerWorld[w][i]) pd->system->logToConsole("SAVE: EXP PER WORLD %s, w=%i : %f", toStringCargoByType(i, /*plural=*/true), w, (double)m_player.m_exportPerWorld[w][i]);
      #endif
    }
    je->endArray(je);
  }

  je->addTableMember(je, "spw", 3);
  je->startArray(je);
  for (int32_t w = 0; w < WORLD_SAVE_SLOTS; ++w) {
    je->addArrayMember(je);
    je->writeDouble(je, m_player.m_sellPricePerWorld[w]);
    #ifdef DEV // zzz v1.5 update checks
    if (m_player.m_sellPricePerWorld[w]) pd->system->logToConsole("SAVE: SELL P.P. WORLD, w=%i : %f", w, (double)m_player.m_sellPricePerWorld[w]);
    #endif
  }
  je->endArray(je);

  je->addTableMember(je, "impc", 4);
  je->startArray(je);
  for (int32_t i = 0; i < kNCargoType; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_importConsumers[i]);
     #ifdef DEV// zzz v1.5 update checks
     if (m_player.m_importConsumers[i]) pd->system->logToConsole("SAVE: IMPORT CONSUMERS, c=%i : %i", i, m_player.m_importConsumers[i]);
     #endif
  }
  je->endArray(je);

  for (int32_t w = 0; w < WORLD_SAVE_SLOTS; ++w) {
    char txt[12] = "";
    snprintf(txt, 12, "scpw%02i", (int)w);
    je->addTableMember(je, txt, 6);
    je->startArray(je);
    for (int32_t i = 0; i < kNCargoType; ++i) {
      je->addArrayMember(je);
      je->writeDouble(je, m_player.m_soldPerWorld[w][i]);
      #ifdef DEV// zzz v1.5 update checks
      if (m_player.m_soldPerWorld[w][i]) pd->system->logToConsole("SAVE: SELL RATE %s, w=%i : %f", toStringCargoByType(i, /*plural=*/true), w, (double)m_player.m_soldPerWorld[w][i]);
      #endif
    }
    je->endArray(je);
  }

  je->endTable(je);
}

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "sf") == 0) {
    m_player.m_saveFormat = json_intValue(_value);
  } else if (strcmp(_key, "x") == 0) {
    m_player.m_pix_x = (float) json_intValue(_value);
    #ifdef DEV
    pd->system->logToConsole("LOAD: Player x=%i", (int)m_player.m_pix_x);
    #endif
  } else if (strcmp(_key, "y") == 0) {
    m_player.m_pix_y = json_intValue(_value);
    #ifdef DEV
    pd->system->logToConsole("LOAD: Player y=%i", (int)m_player.m_pix_y);
    #endif
  } else if (strcmp(_key, "m") == 0) {
    m_player.m_money = json_intValue(_value);
  } else if (strcmp(_key, "mhwm") == 0) {
    m_player.m_moneyHighWaterMark = json_intValue(_value);
  } else if (strcmp(_key, "but") == 0) {
    m_player.m_buildingsUnlockedTo = json_intValue(_value);
  } else if (strcmp(_key, "mc") == 0) {
    m_player.m_moneyCumulative = json_intValue(_value);
  } else if (strcmp(_key, "st") == 0) {
    m_player.m_saveTime = json_intValue(_value);
  } else if (strcmp(_key, "pt") == 0) {
    m_player.m_playTime = json_intValue(_value);
  } else if (strcmp(_key, "tutstage") == 0) {
    m_player.m_tutorialProgress = json_intValue(_value);
    #ifdef DEV
    pd->system->logToConsole("LOAD: tutstage=%i", m_player.m_tutorialProgress);
    #endif
  } else if (strcmp(_key, "tutpaid") == 0) { // New in v1.5
    m_player.m_paidTutorialMoney = json_intValue(_value);
    #ifdef DEV
    pd->system->logToConsole("LOAD: tutpaid=%i", m_player.m_paidTutorialMoney);
    #endif
  } else if (strcmp(_key, "slot") == 0) {
    setSlot( json_intValue(_value) ); 
    #ifdef DEV
    pd->system->logToConsole("LOAD: slot=%i", json_intValue(_value));
    #endif
  } else if (strcmp(_key, "sets") == 0) {
    m_player.m_soundSettings = json_intValue(_value); 
  } else if (strcmp(_key, "setm") == 0) {
    m_player.m_musicVol = json_intValue(_value); 
  } else if (strcmp(_key, "setb") == 0) {
    m_player.m_autoUseConveyorBooster = json_intValue(_value); 
  } else if (strcmp(_key, "setc") == 0) {
    m_player.m_enableConveyorAnimation = json_intValue(_value);
  } else if (strcmp(_key, "sett") == 0) {
    m_player.m_enableTutorial = json_intValue(_value);
  } else if (strcmp(_key, "setd") == 0) {
    m_player.m_enableDebug = json_intValue(_value); 
  } else if (strcmp(_key, "setp") == 0) {
    m_player.m_enablePickupOnDestroy = json_intValue(_value); 
  } else if (strcmp(_key, "seth") == 0) {
    m_player.m_enableScreenShake = json_intValue(_value); 
  } else if (strcmp(_key, "seto") == 0) {
    m_player.m_enableExtractorOutlines = json_intValue(_value); 
  } else if (strcmp(_key, "sete") == 0) {
    m_player.m_enableSteps = json_intValue(_value); 
  } else if (strcmp(_key, "setz") == 0) {
    m_player.m_enableZoomWhenMove = json_intValue(_value); 
  } else if (strcmp(_key, "setr") == 0) {
    m_player.m_enableCrankConveyor = json_intValue(_value); 
  } else if (strcmp(_key, "setn") == 0) {
    m_player.m_enableCentreCamera = json_intValue(_value); 
  } else if (strcmp(_key, "seta") == 0) {
    m_player.m_enableAutosave = json_intValue(_value); 
    m_deserialiseArrayID = 0; // Note "one behind"
  } else if (strcmp(_key, "cargos") == 0) {
    m_deserialiseArrayID = 1;
  } else if (strcmp(_key, "convs") == 0) {
    m_deserialiseArrayID = 2;
  } else if (strcmp(_key, "plants") == 0) {
    m_deserialiseArrayID = 3;
  } else if (strcmp(_key, "util") == 0) {
    m_deserialiseArrayID = 4;
  } else if (strcmp(_key, "xtrcts") == 0) {
    m_deserialiseArrayID = 5;
  } else if (strcmp(_key, "facts") == 0) {
    m_deserialiseArrayID = 6;
  } else if (strcmp(_key, "scargo") == 0) {
    m_deserialiseArrayID = 7;
  } else if (strcmp(_key, "icargo") == 0) {
    m_deserialiseArrayID = 8;

  } else if (strcmp(_key, "expw00") == 0) {
    m_deserialiseArrayID = 9;
  } else if (strcmp(_key, "expw01") == 0) {
    m_deserialiseArrayID = 10;
  } else if (strcmp(_key, "expw02") == 0) {
    m_deserialiseArrayID = 11;
  } else if (strcmp(_key, "expw03") == 0) {
    m_deserialiseArrayID = 12;
  } else if (strcmp(_key, "expw04") == 0) {
    m_deserialiseArrayID = 13;
  } else if (strcmp(_key, "expw05") == 0) {
    m_deserialiseArrayID = 14;
  } else if (strcmp(_key, "expw06") == 0) {
    m_deserialiseArrayID = 15;
  } else if (strcmp(_key, "expw07") == 0) {
    m_deserialiseArrayID = 16;
  } else if (strcmp(_key, "spw") == 0) {
    m_deserialiseArrayID = 17;
  } else if (strcmp(_key, "impc") == 0) {
    m_deserialiseArrayID = 18;

  // New in v1.5
  } else if (strcmp(_key, "scpw00") == 0) {
    m_deserialiseArrayID = 19;
  } else if (strcmp(_key, "scpw01") == 0) {
    m_deserialiseArrayID = 20;
  } else if (strcmp(_key, "scpw02") == 0) {
    m_deserialiseArrayID = 21;
  } else if (strcmp(_key, "scpw03") == 0) {
    m_deserialiseArrayID = 22;
  } else if (strcmp(_key, "scpw04") == 0) {
    m_deserialiseArrayID = 23;
  } else if (strcmp(_key, "scpw05") == 0) {
    m_deserialiseArrayID = 24;
  } else if (strcmp(_key, "scpw06") == 0) {
    m_deserialiseArrayID = 25;
  } else if (strcmp(_key, "scpw07") == 0) {
    // noop

  } else if (strcmp(_key, "player") == 0) {
    jd->didDecodeSublist = deserialiseStructDonePlayer;
  } else {
    pd->system->error("PLAYER DECODE ISSUE, %s", _key);
  }
}

void deserialiseArrayValuePlayer(json_decoder* jd, int _pos, json_value _value) {
  int v = json_intValue(_value);
  float f = json_floatValue(_value);
  int i = _pos - 1;
  switch (m_deserialiseArrayID) {
    case 0: m_player.m_carryCargo[i] = v; break;
    case 1: m_player.m_carryConveyor[i] = v; break;
    case 2: m_player.m_carryPlant[i] = v; break;
    case 3: m_player.m_carryUtility[i] = v; break;
    case 4: m_player.m_carryExtractor[i] = v; break;
    case 5: m_player.m_carryFactory[i] = v; break;

    case 6: m_player.m_soldCargo[i] = v; break;
    case 7: m_player.m_importedCargo[i] = v; break;

    case 8:  m_player.m_exportPerWorld[0][i] = f; break;
    case 9:  m_player.m_exportPerWorld[1][i] = f; break;
    case 10: m_player.m_exportPerWorld[2][i] = f; break;
    case 11: m_player.m_exportPerWorld[3][i] = f; break;
    case 12: m_player.m_exportPerWorld[4][i] = f; break;
    case 13: m_player.m_exportPerWorld[5][i] = f; break;
    case 14: m_player.m_exportPerWorld[6][i] = f; break;
    case 15: m_player.m_exportPerWorld[7][i] = f; break;

    case 16: m_player.m_sellPricePerWorld[i] = f; break;
    case 17: m_player.m_importConsumers[i] = v; break;

    case 18: m_player.m_soldPerWorld[0][i] = f; break;
    case 19: m_player.m_soldPerWorld[1][i] = f; break;
    case 20: m_player.m_soldPerWorld[2][i] = f; break;
    case 21: m_player.m_soldPerWorld[3][i] = f; break;
    case 22: m_player.m_soldPerWorld[4][i] = f; break;
    case 23: m_player.m_soldPerWorld[5][i] = f; break;
    case 24: m_player.m_soldPerWorld[6][i] = f; break;
    case 25: m_player.m_soldPerWorld[7][i] = f; break;

  }

  #ifdef DEV
  //zzz v1.5 update checks
  if (m_deserialiseArrayID >= 18 && m_deserialiseArrayID <=25 && f)
    pd->system->logToConsole("LOAD: SELL RATE %s, w=%i : %f", toStringCargoByType(i, /*plural=*/true), m_deserialiseArrayID-18, (double)f);

  if (m_deserialiseArrayID >= 8 && m_deserialiseArrayID <=15 && f)
    pd->system->logToConsole("LOAD: EXP RATE %s, w=%i : %f", toStringCargoByType(i, /*plural=*/true), m_deserialiseArrayID-8, (double)f);

  if (m_deserialiseArrayID == 16 && f)
    pd->system->logToConsole("LOAD: SELL P.P. WORLD, w=%i : %f", i, (double)f);

  if (m_deserialiseArrayID == 17 && v)
    pd->system->logToConsole("LOAD: IMPORT CONSUMERS, c=%i : %i", i, v); 
  #endif

}

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type) {
  setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y, /*update current location = */ true);
  m_player.m_camera_pix_x = m_player.m_pix_x;
  m_player.m_camera_pix_y = m_player.m_pix_y;

  #ifdef DEV
  pd->system->logToConsole("-- Player decoded to (%i, %i), current location (%i, %i), money:%i, unlock:%i, version:%i", 
    (int32_t)m_player.m_pix_x, (int32_t)m_player.m_pix_y, m_currentLocation->m_x, m_currentLocation->m_y, m_player.m_money, m_player.m_buildingsUnlockedTo, m_player.m_saveFormat);
  #endif

  #ifdef DEMO
  m_player.m_money = 0;
  m_player.m_moneyCumulative = 0;
  m_player.m_moneyHighWaterMark = 0;
  m_player.m_buildingsUnlockedTo = getCactusUnlock(); // cactus
  m_player.m_enableTutorial = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) if (m_player.m_carryCargo[i]) m_player.m_carryCargo[i] = rand() % 10;
  for (int32_t i = 0; i < kNConvSubTypes; ++i) if (m_player.m_carryConveyor[i]) m_player.m_carryConveyor[i] = rand() % 10;
  for (int32_t i = 0; i < kNUtilitySubTypes; ++i) if (m_player.m_carryUtility[i]) m_player.m_carryUtility[i] = (i >= kStorageBox ? 0 : rand() % 10);
  for (int32_t i = 0; i < kNPlantSubTypes; ++i) if (m_player.m_carryPlant[i]) m_player.m_carryPlant[i] = rand() % 10;
  for (int32_t i = 0; i < kNExtractorSubTypes; ++i) if (m_player.m_carryExtractor[i]) m_player.m_carryExtractor[i] = rand() % 10;
  for (int32_t i = 0; i < kNFactorySubTypes; ++i) if (m_player.m_carryFactory[i]) m_player.m_carryFactory[i] = rand() % 10;
  #endif

  // SCHEMA EVOLUTION - V4 to V5 (v1.0 to v1.1)
  // Might need to correct the building unlock progression due to new unlocks being added
  // Need to set defaults for new options parameters
  if (m_player.m_saveFormat == V1p0_SAVE_FORMAT) {
    m_player.m_saveFormat = V1p1_SAVE_FORMAT;
    const uint32_t prevUnlockedTo = m_player.m_buildingsUnlockedTo; 
    if (prevUnlockedTo >= 65) { // Dessert Factory from 1.0. Need to account for 2x Overflow, Rotavator & FacUp
      m_player.m_buildingsUnlockedTo += 4;
    } else if (prevUnlockedTo >= 46) { // Conveyor Grease from 1.0. Need to account for 2x Overflow & FacUp
      m_player.m_buildingsUnlockedTo += 3;
    } else if (prevUnlockedTo >= 26) { // Sign from 1.0. Need to account for 2x Overflow.
      m_player.m_buildingsUnlockedTo += 2;
    }
    m_player.m_enableCrankConveyor = 1; 
    m_player.m_enableCentreCamera = 0;
    pd->system->logToConsole("-- Performed player schema evolution from v%i to v%i, UnlockedTo:%i -> %i", V1p0_SAVE_FORMAT, V1p1_SAVE_FORMAT, prevUnlockedTo, m_player.m_buildingsUnlockedTo);
  }

  // SCHEMA EVOLUTION - V5 to V6 (v1.1 to v1.5)
  if (m_player.m_saveFormat == V1p1_SAVE_FORMAT) {
    m_player.m_saveFormat = V1p5_SAVE_FORMAT;
    // Just adding data - nothing to migrate
    pd->system->logToConsole("-- Performed player schema evolution from v%i to v%i", V1p1_SAVE_FORMAT, V1p5_SAVE_FORMAT);
  }

  m_deserialiseArrayID = -1;

  return NULL;
}
