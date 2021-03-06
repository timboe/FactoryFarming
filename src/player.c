#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "render.h"
#include "input.h"
#include "io.h"
#include "ui.h"
#include "generate.h"

//                             {kToolPickup, kToolInspect, kToolDestroy, kNToolTypes};
const uint16_t kToolUIIcon[] = {SID(8,10),   SID(9,10),    SID(1,16)};

struct Player_t m_player;

int16_t m_offX, m_offY = 0; // Screen offsets

uint8_t m_facing = 0, m_wasFacing = 0, m_stepCounter = 0, m_animFrame = 0;

uint16_t m_deserialiseXPlayer = 0, m_deserialiseYPlayer = 0;
int16_t m_deserialiseArrayID = -1;

struct Chunk_t* m_currentChunk = NULL;

enum kChunkQuad m_quadrant = 0;

struct Location_t* m_currentLocation = NULL;

bool m_top = true;
bool m_left = true;
bool m_forceTorus = true;

void movePlayerPosition(float _goalX, float _goalY);

void updatePlayerPosition(void);

void playerSpriteSetup(void);

/// ///

const char* toStringTool(enum kToolType _type) {
  switch(_type) {
    case kToolPickup: return "Pickup Cargo Mode";
    case kToolInspect: return "Inspection Mode";
    case kToolDestroy: return "Demolition Mode";
    default: return "Tool???";
  }
}

const char* toStringToolInfo(enum kToolType _type) {
  switch(_type) {
    case kToolPickup: return "Get nearby cargo, empties buildings";
    case kToolInspect: return "See details about the current tile";
    case kToolDestroy: return "Destroys buildings, belts and cargo";
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

void nextTutorialStage() {
  m_player.m_tutorialProgress = 0;
  ++m_player.m_enableTutorial;
  if (m_player.m_enableTutorial == kTutBreakOne || m_player.m_enableTutorial == kTutBreakTwo) {
    m_player.m_enableTutorial = TUTORIAL_FINISHED;
    // TEMP - end of tutorial cash
    modMoney(100000);
    return;
  } else if (m_player.m_enableTutorial == kTutGetCarrots) {
    growAtAll(); // Force all plants to grow
  } else if (m_player.m_enableTutorial == kTutBuildHarvester) {
    modMoney( EDesc[kCropHarvesterSmall].unlock );
  } else if (m_player.m_enableTutorial == kTutBuildConveyor) {
    modMoney( CDesc[kBelt].unlock );
  } else if (m_player.m_enableTutorial == kTutBuildQuarry) {
    modMoney( EDesc[kChalkQuarry].unlock );
  } else if (m_player.m_enableTutorial == kTutBuildVitamin) {
    modMoney( FDesc[kVitiminFac].unlock );
  }

  showTutorialMsg(m_player.m_enableTutorial);
}

int16_t getOffX() {
  return m_offX;
}

int16_t getOffY() {
  return m_offY;
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
  uint8_t zoom = getZoom(); 
  pd->sprite->moveTo(m_player.m_sprite[zoom], _x * zoom, _y * zoom);
  updatePlayerPosition();
  pd->sprite->setZIndex(m_player.m_sprite[zoom], (int16_t)m_player.m_pix_y * zoom);
  if (_updateCurrentLocation) {
    m_currentLocation = getLocation(m_player.m_pix_x / TILE_PIX, m_player.m_pix_y / TILE_PIX);
    m_currentChunk = getChunk(m_player.m_pix_x / CHUNK_PIX_X, m_player.m_pix_y / CHUNK_PIX_Y);
  }
}

void movePlayerPosition(float _goalX, float _goalY) {
  int len;
  uint8_t zoom = getZoom();
  SpriteCollisionInfo* collInfo = pd->sprite->moveWithCollisions(m_player.m_sprite[zoom], _goalX * zoom, _goalY * zoom, &(m_player.m_pix_x), &(m_player.m_pix_y), &len);
  if (len) pd->system->realloc(collInfo, 0); // Free
  updatePlayerPosition();
  pd->sprite->setZIndex(m_player.m_sprite[zoom], (int16_t)m_player.m_pix_y * zoom);

}

void updatePlayerPosition() {
  uint8_t zoom = getZoom();
  pd->sprite->getPosition(m_player.m_sprite[zoom], &(m_player.m_pix_x), &(m_player.m_pix_y));
  m_player.m_pix_x = m_player.m_pix_x / zoom;
  m_player.m_pix_y = m_player.m_pix_y / zoom;
  //pd->system->logToConsole("P@ %f %f", m_player.m_pix_x , m_player.m_pix_y);
}

bool movePlayer() {

  uint8_t zoom = getZoom();
  //updatePlayerPosition();
  float goalX = m_player.m_pix_x;
  float goalY = m_player.m_pix_y;
  
  /*

  float diffX = 0;
  float diffY = 0;

  if (m_pressed[0]) diffX -= PLAYER_A;
  if (m_pressed[1]) diffX += PLAYER_A;
  if (m_pressed[2]) diffY -= PLAYER_A;
  if (m_pressed[3]) diffY += PLAYER_A;

  // Note floating point == 0 check, hopefully work in this case
  if (diffX && diffY) {
    diffX *= SQRT_HALF;
    diffY *= SQRT_HALF;
  }

  m_player.m_vX = (m_player.m_vX + diffX) * PLAYER_FRIC;
  m_player.m_vY = (m_player.m_vY + diffY) * PLAYER_FRIC;

  goalX += m_player.m_vX;
  goalY += m_player.m_vY;
  */

  // TODO proper movement penalty / bonus
  int16_t speed = 4;
  if (m_currentLocation != NULL && isWaterTile(m_currentLocation->m_x, m_currentLocation->m_y)) {
    speed = 2;
  }
  if (bPressed()) speed += 2;

  bool moving = false;
  if (getPressed(0)) { goalX -= speed / zoom; m_facing = 0; moving = true; } 
  if (getPressed(1)) { goalX += speed / zoom; m_facing = 1; moving = true; }
  if (getPressed(2)) { goalY -= speed / zoom; m_facing = 2; moving = true; }
  if (getPressed(3)) { goalY += speed / zoom; m_facing = 3; moving = true; }

  #define PLAYER_ANIM_FRAMES 6
  #define PLAYER_ANIM_DELAY 16

  if (moving) {
    if (++m_stepCounter * speed > PLAYER_ANIM_DELAY || m_facing != m_wasFacing) {
      m_animFrame = (m_animFrame + 1) % PLAYER_ANIM_FRAMES;
      m_stepCounter = 0;
      pd->sprite->setImage(m_player.m_sprite[zoom], getSprite18(m_animFrame, m_facing, zoom), kBitmapUnflipped);
    }
    m_wasFacing = m_facing;
  }


  //pd->system->logToConsole("GOAL %f %f CURRENT %f %f", goalX, goalY, m_player.m_x, m_player.m_y);

  movePlayerPosition(goalX, goalY);

  //if      ((m_offX + m_player.m_x) > ((SCREEN_PIX_X / m_zoom) * SCROLL_EDGE))          m_offX = ((SCREEN_PIX_X / m_zoom) * SCROLL_EDGE) - m_player.m_x;
  //else if ((m_offX + m_player.m_x) < ((SCREEN_PIX_X / m_zoom) * (1.0f - SCROLL_EDGE))) m_offX = ((SCREEN_PIX_X / m_zoom) * (1.0f - SCROLL_EDGE)) - m_player.m_x;

  //if      ((m_offY + m_player.m_y) > ((SCREEN_PIX_Y / m_zoom) * SCROLL_EDGE))          m_offY = ((SCREEN_PIX_Y / m_zoom) * SCROLL_EDGE) - m_player.m_y;
  //else if ((m_offY + m_player.m_y) < ((SCREEN_PIX_Y / m_zoom) * (1.0f - SCROLL_EDGE))) m_offY = ((SCREEN_PIX_Y / m_zoom) * (1.0f - SCROLL_EDGE)) - m_player.m_y;

  if (m_player.m_pix_x > TOT_WORLD_PIX_X) {
    setPlayerPosition(m_player.m_pix_x - TOT_WORLD_PIX_X, m_player.m_pix_y, /*update current location = */ false);
    m_offX += TOT_WORLD_PIX_X;
  } else if (m_player.m_pix_x < 0) {
    setPlayerPosition(m_player.m_pix_x + TOT_WORLD_PIX_X, m_player.m_pix_y, /*update current location = */ false);
    m_offX -= TOT_WORLD_PIX_X;
  }

  if (m_player.m_pix_y > TOT_WORLD_PIX_Y) {
    setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y - TOT_WORLD_PIX_Y, /*update current location = */ false);
    m_offY += TOT_WORLD_PIX_Y;
  } else if (m_player.m_pix_y < 0) {
    setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y + TOT_WORLD_PIX_Y, /*update current location = */ false);
    m_offY -= TOT_WORLD_PIX_Y;
  }

  //pd->system->logToConsole("OFF %f %f", m_offX, m_offY);

  //pd->system->logToConsole("CHX %f / %f = %f", m_player.m_x, (float)CHUNK_PIX_X, ((m_player.m_x)/((float)CHUNK_PIX_X)));


  m_offX = -(m_player.m_pix_x*zoom - (SCREEN_PIX_X/2));
  m_offY = -(m_player.m_pix_y*zoom - (SCREEN_PIX_Y/2));


  //pd->system->logToConsole("P@ %f %f", m_player.m_pix_x , m_player.m_pix_y);
  //pd->system->logToConsole("OFF %i %i", m_offX, m_offY);


  // Check chunk change
  uint16_t chunkX = m_player.m_pix_x / (CHUNK_PIX_X);
  uint16_t chunkY = m_player.m_pix_y / (CHUNK_PIX_Y);

  // Subchunk change
  uint8_t subChunkX = (int16_t)m_player.m_pix_x / (CHUNK_PIX_X/2) % 2;
  uint8_t subChunkY = (int16_t)m_player.m_pix_y / (CHUNK_PIX_Y/2) % 2;
  enum kChunkQuad quadrant = NW;
  if (subChunkX && subChunkY) {
    quadrant = SE;
  } else if (subChunkX) {
    quadrant = NE;
  } else if (subChunkY) {
    quadrant = SW;
  }

  bool chunkChange = (chunkX != m_currentChunk->m_x || chunkY != m_currentChunk->m_y);
  if (chunkChange || m_quadrant != quadrant) {
    m_currentChunk = getChunk(chunkX, chunkY); // TODO this can still go out-of-bounds (how?), ideally should be able to use getChunk_noCheck here
    m_quadrant = quadrant;
    if (zoom == 2 || chunkChange) { // When zoomed out, only need to call when actually changing chunks
      pd->system->logToConsole("CHUNKCHANGE %u %u (%u %u)", chunkX, chunkY, subChunkX, subChunkY);
      updateRenderList();
    }
  }

  struct Location_t* wasAt = m_currentLocation;
  m_currentLocation = getLocation(m_player.m_pix_x / TILE_PIX, m_player.m_pix_y / TILE_PIX);

  if (wasAt != m_currentLocation) {
    int32_t bpX = (TILE_PIX*m_currentLocation->m_x + TILE_PIX/2.0) * zoom, bpY = (TILE_PIX*m_currentLocation->m_y  + TILE_PIX/2.0) * zoom;
    pd->sprite->moveTo(m_player.m_blueprint[zoom], bpX, bpY);
    pd->sprite->moveTo(m_player.m_blueprintRadius[zoom], bpX, bpY);
  }

  // Torus splitting
  bool torusChanged = false;
  if (m_top && m_player.m_pix_y > TOT_WORLD_PIX_Y/2) {
    m_top = false;
    torusChanged = true;
  } else if (!m_top && m_player.m_pix_y <= TOT_WORLD_PIX_Y/2) {
    m_top = true;
    torusChanged = true;
  }

  if (m_left && m_player.m_pix_x > TOT_WORLD_PIX_X/2) {
    m_left = false;
    torusChanged = true;
  } else if (!m_left && m_player.m_pix_x <= TOT_WORLD_PIX_X/2) {
    m_left = true;
    torusChanged = true;
  }

  if (torusChanged || m_forceTorus) {
    chunkShiftTorus(m_top, m_left);
    m_forceTorus = false;
  }

  return true;
}

bool modMoney(int32_t _amount) {
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

// TODO this is named wrong in the docs
SpriteCollisionResponseType playerLCDSpriteCollisionFilterProc(LCDSprite* _player, LCDSprite* _other) {
  return kCollisionTypeSlide;
}

void playerSpriteSetup() {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    m_player.m_sprite[zoom] = pd->sprite->newSprite();
    PDRect pBound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = 18*zoom};
    PDRect cBound = {.x = (COLLISION_OFFSET_SMALL/2)*zoom, .y = (COLLISION_OFFSET_SMALL/2)*zoom, .width = (TILE_PIX - COLLISION_OFFSET_SMALL)*zoom, .height = (TILE_PIX - COLLISION_OFFSET_SMALL)*zoom};
    pd->sprite->setBounds(m_player.m_sprite[zoom], pBound);
    pd->sprite->setImage(m_player.m_sprite[zoom], getSprite18(0, 3, zoom), kBitmapUnflipped);
    pd->sprite->setCollideRect(m_player.m_sprite[zoom], cBound);
    pd->sprite->setCollisionResponseFunction(m_player.m_sprite[zoom], playerLCDSpriteCollisionFilterProc);

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
  }
}

void forceTorus() { m_forceTorus = true; }

void resetPlayer() {
  m_player.m_money = 0;
  m_player.m_moneyCumulative = 0;
  m_player.m_moneyHighWaterMark = 1;
  m_player.m_moneyHighWaterMarkMenu = 0;
  m_player.m_saveTime = pd->system->getSecondsSinceEpoch(NULL);
  m_player.m_playTime = 0;
  m_player.m_tutorialProgress = 0;
  memset(m_player.m_exportPerWorld, 0, sizeof(float)*WORLD_SAVE_SLOTS*kNCargoType);
  if (m_player.m_enableTutorial != TUTORIAL_DISABLED) m_player.m_enableTutorial = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) m_player.m_carryCargo[i] = 0;
  for (int32_t i = 0; i < kNConvSubTypes; ++i) m_player.m_carryConveyor[i] = 0;
  for (int32_t i = 0; i < kNUtilitySubTypes; ++i) m_player.m_carryUtility[i] = 0;
  for (int32_t i = 0; i < kNPlantSubTypes; ++i) m_player.m_carryPlant[i] = 0;
  for (int32_t i = 0; i < kNExtractorSubTypes; ++i) m_player.m_carryExtractor[i] = 0;
  for (int32_t i = 0; i < kNFactorySubTypes; ++i) m_player.m_carryFactory[i] = 0;
  for (int32_t i = 0; i < kNCargoType; ++i) m_player.m_importConsumers[i] = 0;
  setPlayerPosition(SCREEN_PIX_X/4, (3*SCREEN_PIX_Y)/4, /*update current location = */ true);
  m_currentChunk = getChunk_noCheck(0,0);
  modMoney(100000); // TEMP
}


void initPlayer() {
  playerSpriteSetup();
}

void serialisePlayer(struct json_encoder* je) {
  je->addTableMember(je, "player", 6);
  je->startTable(je);
  je->addTableMember(je, "sf", 2); // Save format
  je->writeInt(je, SAVE_FORMAT);
  je->addTableMember(je, "x", 1);
  je->writeInt(je, (int) m_player.m_pix_x);
  je->addTableMember(je, "y", 1);
  je->writeInt(je, (int) m_player.m_pix_y);
  je->addTableMember(je, "m", 1);
  je->writeInt(je, m_player.m_money);
  je->addTableMember(je, "mhwm", 4);
  je->writeInt(je, m_player.m_moneyHighWaterMark);
  je->addTableMember(je, "mhwmm", 5);
  je->writeInt(je, m_player.m_moneyHighWaterMarkMenu);
  je->addTableMember(je, "mc", 2);
  je->writeInt(je, m_player.m_moneyCumulative);
  je->addTableMember(je, "st", 2);
  je->writeInt(je, pd->system->getSecondsSinceEpoch(NULL));
  je->addTableMember(je, "pt", 2);
  je->writeInt(je, m_player.m_playTime);
  je->addTableMember(je, "tutstage", 8);
  je->writeInt(je, m_player.m_tutorialProgress);

  je->addTableMember(je, "slot", 4);
  je->writeInt(je, getSlot());

  // Settings - not reset new games
  je->addTableMember(je, "sets", 4);
  je->writeInt(je, m_player.m_soundSettings);
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

  for (int32_t w = 0; w < WORLD_SAVE_SLOTS; ++w) {
    char txt[12] = "";
    snprintf(txt, 12, "expw%02i", (int)w);
    je->addTableMember(je, txt, 6);
    je->startArray(je);
    for (int32_t i = 0; i < kNCargoType; ++i) {
      je->addArrayMember(je);
      je->writeDouble(je, m_player.m_exportPerWorld[w][i]);
    }
    je->endArray(je);
  }

  je->addTableMember(je, "impc", 4);
  je->startArray(je);
  for (int32_t i = 0; i < kNCargoType; ++i) {
    je->addArrayMember(je);
    je->writeInt(je, m_player.m_importConsumers[i]);
  }
  je->endArray(je);

  je->endTable(je);
}

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "sf") == 0) {
    m_player.m_saveFormat = json_intValue(_value);
  } else if (strcmp(_key, "x") == 0) {
    m_player.m_pix_x = (float) json_intValue(_value);
  } else if (strcmp(_key, "y") == 0) {
    m_player.m_pix_y = json_intValue(_value);
  } else if (strcmp(_key, "m") == 0) {
    m_player.m_money = json_intValue(_value);
  } else if (strcmp(_key, "mhwm") == 0) {
    m_player.m_moneyHighWaterMark = json_intValue(_value);
  } else if (strcmp(_key, "mhwmm") == 0) {
    m_player.m_moneyHighWaterMarkMenu = json_intValue(_value);
  } else if (strcmp(_key, "mc") == 0) {
    m_player.m_moneyCumulative = json_intValue(_value);
  } else if (strcmp(_key, "st") == 0) {
    m_player.m_saveTime = json_intValue(_value);
  } else if (strcmp(_key, "pt") == 0) {
    m_player.m_playTime = json_intValue(_value);
  } else if (strcmp(_key, "tutstage") == 0) {
    m_player.m_tutorialProgress = json_intValue(_value);
  } else if (strcmp(_key, "slot") == 0) {
    setSlot( json_intValue(_value) ); 
  } else if (strcmp(_key, "sets") == 0) {
    m_player.m_soundSettings = json_intValue(_value); 
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
  } else if (strcmp(_key, "expw00") == 0) {
    m_deserialiseArrayID = 7;
  } else if (strcmp(_key, "expw01") == 0) {
    m_deserialiseArrayID = 8;
  } else if (strcmp(_key, "expw02") == 0) {
    m_deserialiseArrayID = 9;
  } else if (strcmp(_key, "expw03") == 0) {
    m_deserialiseArrayID = 10;
  } else if (strcmp(_key, "expw04") == 0) {
    m_deserialiseArrayID = 11;
  } else if (strcmp(_key, "expw05") == 0) {
    m_deserialiseArrayID = 12;
  } else if (strcmp(_key, "expw06") == 0) {
    m_deserialiseArrayID = 13;
  } else if (strcmp(_key, "expw07") == 0) {
    m_deserialiseArrayID = 14;
  } else if (strcmp(_key, "impc") == 0) {
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

    case 6: m_player.m_exportPerWorld[0][i] = f; break;
    case 7: m_player.m_exportPerWorld[1][i] = f; break;
    case 8: m_player.m_exportPerWorld[2][i] = f; break;
    case 9: m_player.m_exportPerWorld[3][i] = f; break;
    case 10: m_player.m_exportPerWorld[4][i] = f; break;
    case 11: m_player.m_exportPerWorld[5][i] = f; break;
    case 12: m_player.m_exportPerWorld[6][i] = f; break;
    case 13: m_player.m_exportPerWorld[7][i] = f; break;

    case 14: m_player.m_importConsumers[i] = v; break;
  }
}

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type) {
  setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y, /*update current location = */ true);

  pd->system->logToConsole("-- Player decoded to (%i, %i), current location (%i, %i), money:%i", 
    (int32_t)m_player.m_pix_x, (int32_t)m_player.m_pix_y, m_currentLocation->m_x, m_currentLocation->m_y, m_player.m_money);

  m_deserialiseArrayID = -1;

  return NULL;
}
