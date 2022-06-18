#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "render.h"
#include "input.h"
#include "io.h"

struct Player_t m_player;

int16_t m_offX, m_offY = 0; // Screen offsets

uint8_t m_facing;

uint8_t m_lookingAtOffset;

bool m_updateLookingAt = false;

uint16_t m_deserialiseXPlayer = 0, m_deserialiseYPlayer = 0;
int16_t m_deserialiseArrayID = -1;

struct Chunk_t* m_currentChunk;

enum kChunkQuad m_quadrant = 0;

struct Location_t* m_currentLocation;

struct Location_t* m_lookingAt;

void setPlayerPosition(uint16_t _x, uint16_t _y);

void movePlayerPosition(float _goalX, float _goalY);

void updatePlayerPosition(void);

void playerSpriteSetup(void);

/// ///

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

struct Location_t* getPlayerLookingAtLocation() {
  return m_lookingAt;
}

void setPlayerLookingAtOffset(int8_t _offset) {
  m_lookingAtOffset = _offset;
  m_updateLookingAt = true;
}

void setPlayerPosition(uint16_t _x, uint16_t _y) {
  uint8_t zoom = getZoom(); 
  pd->sprite->moveTo(m_player.m_sprite[zoom], _x * zoom, _y * zoom);
  updatePlayerPosition();
  pd->sprite->setZIndex(m_player.m_sprite[zoom], (int16_t)m_player.m_pix_y * zoom);
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

  if (getPressed(0)) { goalX -= 4 / zoom; m_facing = 0; } 
  if (getPressed(1)) { goalX += 4 / zoom; m_facing = 1; }
  if (getPressed(2)) { goalY -= 4 / zoom; m_facing = 2; }
  if (getPressed(3)) { goalY += 4 / zoom; m_facing = 3; }


  //pd->system->logToConsole("GOAL %f %f CURRENT %f %f", goalX, goalY, m_player.m_x, m_player.m_y);

  movePlayerPosition(goalX, goalY);

  //if      ((m_offX + m_player.m_x) > ((SCREEN_PIX_X / m_zoom) * SCROLL_EDGE))          m_offX = ((SCREEN_PIX_X / m_zoom) * SCROLL_EDGE) - m_player.m_x;
  //else if ((m_offX + m_player.m_x) < ((SCREEN_PIX_X / m_zoom) * (1.0f - SCROLL_EDGE))) m_offX = ((SCREEN_PIX_X / m_zoom) * (1.0f - SCROLL_EDGE)) - m_player.m_x;

  //if      ((m_offY + m_player.m_y) > ((SCREEN_PIX_Y / m_zoom) * SCROLL_EDGE))          m_offY = ((SCREEN_PIX_Y / m_zoom) * SCROLL_EDGE) - m_player.m_y;
  //else if ((m_offY + m_player.m_y) < ((SCREEN_PIX_Y / m_zoom) * (1.0f - SCROLL_EDGE))) m_offY = ((SCREEN_PIX_Y / m_zoom) * (1.0f - SCROLL_EDGE)) - m_player.m_y;

  if (m_player.m_pix_x > TOT_WORLD_PIX_X) {
    setPlayerPosition(m_player.m_pix_x - TOT_WORLD_PIX_X, m_player.m_pix_y);
    m_offX += TOT_WORLD_PIX_X;
  } else if (m_player.m_pix_x < 0) {
    setPlayerPosition(m_player.m_pix_x + TOT_WORLD_PIX_X, m_player.m_pix_y);
    m_offX -= TOT_WORLD_PIX_X;
  }

  if (m_player.m_pix_y > TOT_WORLD_PIX_Y) {
    setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y - TOT_WORLD_PIX_Y);
    m_offY += TOT_WORLD_PIX_Y;
  } else if (m_player.m_pix_y < 0) {
    setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y + TOT_WORLD_PIX_Y);
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
  uint8_t subChunkX = (uint16_t)(m_player.m_pix_x / ((float)(CHUNK_PIX_X)/2.0f)) % 2;
  uint8_t subChunkY = (uint16_t)(m_player.m_pix_y / ((float)(CHUNK_PIX_Y)/2.0f)) % 2;
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

  switch (m_facing) {
    case 0: m_lookingAt = getLocation(m_currentLocation->m_x - m_lookingAtOffset, m_currentLocation->m_y); break;
    case 1: m_lookingAt = getLocation(m_currentLocation->m_x + m_lookingAtOffset, m_currentLocation->m_y); break;
    case 2: m_lookingAt = getLocation(m_currentLocation->m_x, m_currentLocation->m_y - m_lookingAtOffset); break;
    case 3: m_lookingAt = getLocation(m_currentLocation->m_x, m_currentLocation->m_y + m_lookingAtOffset); break;
  }

  //pd->system->logToConsole("LA %i", (int) m_lookingAt);
  if (m_lookingAt && (wasAt != m_currentLocation || m_updateLookingAt)) {
    m_updateLookingAt = false;
    int32_t bpX = (TILE_PIX*m_lookingAt->m_x + TILE_PIX/2.0) * zoom, bpY = (TILE_PIX*m_lookingAt->m_y  + TILE_PIX/2.0) * zoom;

    pd->sprite->moveTo(m_player.m_blueprint[zoom], bpX, bpY);
    pd->sprite->moveTo(m_player.m_blueprintRadius[zoom], bpX, bpY);

    //pd->system->logToConsole("LA %i %i, off %i", m_lookingAt->m_x, m_lookingAt->m_y, m_lookingAtOffset);
  }

  return true;
}

void playerSpriteSetup() {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    m_player.m_sprite[zoom] = pd->sprite->newSprite();
    PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*zoom, .height = TILE_PIX*zoom};
    pd->sprite->setBounds(m_player.m_sprite[zoom], bound);
    pd->sprite->setImage(m_player.m_sprite[zoom], getSprite16(8, 3, zoom), kBitmapUnflipped);
    pd->sprite->setCollideRect(m_player.m_sprite[zoom], bound);

    m_player.m_blueprint[zoom] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_player.m_blueprint[zoom], bound);
    pd->sprite->setImage(m_player.m_blueprint[zoom], getSprite16(0, 0, zoom), kBitmapUnflipped);
    pd->sprite->setZIndex(m_player.m_blueprint[zoom], Z_INDEX_BLUEPRINT);

    m_player.m_blueprintRadiusBitmap3x3[zoom] = pd->graphics->newBitmap(TILE_PIX*3*zoom, TILE_PIX*3*zoom, kColorClear);
    m_player.m_blueprintRadiusBitmap7x7[zoom] = pd->graphics->newBitmap(TILE_PIX*7*zoom, TILE_PIX*7*zoom, kColorClear);

    uint16_t start = 2*zoom, stop = (TILE_PIX*3 - 2)*zoom;
    pd->graphics->setLineCapStyle(kLineCapStyleRound);
    pd->graphics->pushContext(m_player.m_blueprintRadiusBitmap3x3[zoom]);
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

    m_player.m_blueprintRadius[zoom] = pd->sprite->newSprite();
    pd->sprite->setBounds(m_player.m_blueprintRadius[zoom], bound);
    pd->sprite->setImage(m_player.m_blueprintRadius[zoom], m_player.m_blueprintRadiusBitmap3x3[zoom], kBitmapUnflipped);
    pd->sprite->setZIndex(m_player.m_blueprintRadius[zoom], Z_INDEX_BLUEPRINT);
  }
}

void resetPlayer() {
  setPlayerPosition(SCREEN_PIX_X/2, SCREEN_PIX_Y/2);
  m_currentChunk = getChunk_noCheck(0,0);
}


void initPlayer() {
  playerSpriteSetup();
}

void serialisePlayer(struct json_encoder* je) {
  je->addTableMember(je, "player", 6);
  je->startTable(je);
  je->addTableMember(je, "x", 1);
  je->writeInt(je, (int) m_player.m_pix_x);
  je->addTableMember(je, "y", 1);
  je->writeInt(je, (int) m_player.m_pix_y);
  je->addTableMember(je, "m", 1);
  je->writeInt(je, m_player.m_money);
  je->addTableMember(je, "slot", 4);
  je->writeInt(je, getSlot());
  
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

  je->endTable(je);
}

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "x") == 0) {
    m_player.m_pix_x = (float) json_intValue(_value);
  } else if (strcmp(_key, "y") == 0) {
    m_player.m_pix_y = json_intValue(_value);
  } else if (strcmp(_key, "m") == 0) {
    m_player.m_money = json_intValue(_value);
  } else if (strcmp(_key, "slot") == 0) {
    setSlot( json_intValue(_value) ); 
    m_deserialiseArrayID = 0; // Note "one behind"
  } else if (strcmp(_key, "cargos") == 0) {
    m_deserialiseArrayID = 1;
  } else if (strcmp(_key, "convs") == 0) {
    m_deserialiseArrayID = 2;
  } else if (strcmp(_key, "plants") == 0) {
    m_deserialiseArrayID = 3;
  } else if (strcmp(_key, "xtrcts") == 0) {
    m_deserialiseArrayID = 4;
  } else if (strcmp(_key, "facts") == 0) {
    // noop
  } else if (strcmp(_key, "player") == 0) {
    jd->didDecodeSublist = deserialiseStructDonePlayer;
  } else {
    pd->system->error("PLAYER DECODE ISSUE, %s", _key);
  }
}

void deserialiseArrayValuePlayer(json_decoder* jd, int _pos, json_value _value) {
  int v = json_intValue(_value);
  int i = _pos - 1;
  switch (m_deserialiseArrayID) {
    case 0: m_player.m_carryCargo[i] = v; break;
    case 1: m_player.m_carryConveyor[i] = v; break;
    case 2: m_player.m_carryPlant[i] = v; break;
    case 3: m_player.m_carryExtractor[i] = v; break;
    case 4: m_player.m_carryFactory[i] = v; break;
  }
}

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type) {
  setPlayerPosition(m_player.m_pix_x, m_player.m_pix_y);
  m_currentLocation = getLocation(m_player.m_pix_x / TILE_PIX, m_player.m_pix_y / TILE_PIX);

  pd->system->logToConsole("-- Player decoded to (%i, %i), current location (%i, %i), money:%i", 
    (int32_t)m_player.m_pix_x, (int32_t)m_player.m_pix_y, m_currentLocation->m_x, m_currentLocation->m_y, m_player.m_money);

  m_deserialiseArrayID = -1;

  return NULL;
}
