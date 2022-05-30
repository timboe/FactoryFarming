#include "player.h"
#include "sprite.h"
#include "chunk.h"
#include "render.h"
#include "input.h"

struct Player_t m_player;

int16_t m_offX, m_offY = 0; // Screen offsets

struct Chunk_t* m_currentChunk;
enum kChunkQuad m_quadrant = 0;

struct Location_t* m_currentLocation;
struct Location_t* m_lookingAt;

void setPlayerPosition(uint16_t _x, uint16_t _y);

void movePlayerPosition(float _goalX, float _goalY);

void updatePlayerPosition(void);

void playerSpriteSetup(uint8_t _zoom);

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
  free(collInfo);
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

  if (getPressed(0)) goalX -= 4 / zoom;
  if (getPressed(1)) goalX += 4 / zoom;
  if (getPressed(2)) goalY -= 4 / zoom;
  if (getPressed(3)) goalY += 4 / zoom;


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

  if (chunkX != m_currentChunk->m_x || chunkY != m_currentChunk->m_y || m_quadrant != quadrant) {
    m_currentChunk = getChunk(chunkX, chunkY); // TODO this can still go out-of-bounds (how?), ideally should be able to use getChunk_noCheck here
    m_quadrant = quadrant;
    pd->system->logToConsole("CHUNKCHANGE %u %u (%u %u)", chunkX, chunkY, subChunkX, subChunkY);
    updateRenderList();
  }

  struct Location_t* wasAt = m_currentLocation;
  m_currentLocation = getLocation(m_player.m_pix_x / TILE_PIX, m_player.m_pix_y / TILE_PIX);

  if (getPressed(0)) m_lookingAt = getLocation(m_currentLocation->m_x - 1, m_currentLocation->m_y);
  if (getPressed(1)) m_lookingAt = getLocation(m_currentLocation->m_x + 1, m_currentLocation->m_y);
  if (getPressed(2)) m_lookingAt = getLocation(m_currentLocation->m_x, m_currentLocation->m_y - 1);
  if (getPressed(3)) m_lookingAt = getLocation(m_currentLocation->m_x, m_currentLocation->m_y + 1);

  //pd->system->logToConsole("LA %i", (int) m_lookingAt);
  if (m_currentLocation && wasAt != m_currentLocation) {
    pd->sprite->moveTo(m_player.m_blueprint[zoom], m_currentLocation->m_pix_x * zoom, m_currentLocation->m_pix_y * zoom);
    //pd->system->logToConsole("LA %i %i", m_lookingAt->m_pix_x, m_lookingAt->m_pix_y);
  }

  return true;
}

void playerSpriteSetup(uint8_t _zoom) {
  m_player.m_sprite[_zoom] = pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX*_zoom, .height = TILE_PIX*_zoom};
  pd->sprite->setBounds(m_player.m_sprite[_zoom], bound);
  pd->sprite->setImage(m_player.m_sprite[_zoom], getSprite16(0, 1, _zoom), kBitmapUnflipped);
  pd->sprite->setCollideRect(m_player.m_sprite[_zoom], bound);

  m_player.m_blueprint[_zoom] = pd->sprite->newSprite();
  pd->sprite->setBounds(m_player.m_blueprint[_zoom], bound);
  pd->sprite->setImage(m_player.m_blueprint[_zoom], getSprite16(0, 0, _zoom), kBitmapUnflipped);
  pd->sprite->setZIndex(m_player.m_blueprint[_zoom], Z_INDEX_BLUEPRINT_A);
}

void initPlayer() {
  for (uint32_t zoom = 1; zoom < 5; ++zoom) {
    playerSpriteSetup(zoom);
  }

  setPlayerPosition(SCREEN_PIX_X/2, SCREEN_PIX_Y/2);

  m_currentChunk = getChunk_noCheck(0,0);
}