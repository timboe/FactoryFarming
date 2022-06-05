#include "game.h"
#include "generate.h"
#include "sprite.h"
#include "chunk.h"
#include "location.h"

struct Tile_t* m_tiles = NULL;

uint16_t m_deserialiseIndexWorld = 0;

const int32_t SIZE_GENERATE = TOT_TILES * sizeof(struct Tile_t);
 
void generateSpriteSetup(struct Chunk_t* _chunk);

struct Tile_t* getTileInChunk(struct Chunk_t* _chunk, uint16_t _u, uint16_t _v);

struct Tile_t* getTile(uint16_t _x, uint16_t _y);

void addLake(uint16_t _startX, uint16_t _startY);

/// ///

struct Tile_t* getTileInChunk(struct Chunk_t* _chunk, uint16_t _u, uint16_t _v) {
  return &m_tiles[ (WORLD_CHUNKS_X * TILES_PER_CHUNK_X)*((TILES_PER_CHUNK_Y * _chunk->m_y) + _v) + ((TILES_PER_CHUNK_X * _chunk->m_x) + _u) ];
}

struct Tile_t* getTile(uint16_t _x, uint16_t _y) {
  return &m_tiles[ WORLD_CHUNKS_X*TILES_PER_CHUNK_X*_y + _x ];
}

#define LAKE_MIN 8
#define LAKE_MAX 16
void addLake(uint16_t _startX, uint16_t _startY) {
  const uint8_t w = rand() % (LAKE_MAX - LAKE_MIN) + LAKE_MIN; 
  const uint8_t h = rand() % (LAKE_MAX - LAKE_MIN) + LAKE_MIN; 
  uint8_t c_x[4] = {0};
  uint8_t c_y[4] = {0};
  for (uint8_t i = 0; i < 4; ++i) {
    if (rand() % 2) {
      c_x[i] = 2 + rand() % 2;
      c_y[i] = 2 + rand() % 2;
    }
  }
  // LR
  for (uint16_t x = _startX; x < _startX + w; ++x) {
    uint16_t y = _startY;
    if      (c_x[0] && (x-_startX) <     c_x[0])     y += c_y[0];
    else if (c_x[1] && (x-_startX) > w - c_x[1] - 1) y += c_y[1];

    getTile(x,y)->m_tile = getSprite16_idx(4,6);

    y = _startY + h;
    if      (c_x[3] && (x-_startX) <     c_x[3])     y -= c_y[3];
    else if (c_x[2] && (x-_startX) > w - c_x[2] - 1) y -= c_y[2];

    getTile(x,y)->m_tile = getSprite16_idx(4,6);
  }
  // TB
  for (uint16_t y = _startY; y < _startY + h; ++y) {
    uint16_t x = _startX;
    if      (c_y[0] && (y-_startY) <     c_y[0])     x += c_x[0];
    else if (c_y[1] && (y-_startY) > h - c_y[1] - 1) x += c_x[1];

    getTile(x,y)->m_tile = getSprite16_idx(4,6);

    x = _startX + w;
    if      (c_y[3] && (y-_startY) <     c_y[3])     x -= c_x[3];
    else if (c_y[2] && (y-_startY) > h - c_y[2] - 1) x -= c_x[2];

    getTile(x,y)->m_tile = getSprite16_idx(4,6);
  }
}

void renderChunkBackgroundImage(struct Chunk_t* _chunk) {
  pd->graphics->pushContext(_chunk->m_bkgImage[1]);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->fillRect(0, 0, CHUNK_PIX_X, CHUNK_PIX_Y, kColorWhite);
  for (uint16_t v = 0; v < TILES_PER_CHUNK_Y; ++v) {
    for (uint16_t u = 0; u < TILES_PER_CHUNK_X; ++u) {
      struct Tile_t* t = getTileInChunk(_chunk, u, v);
      LCDBitmap* b = getSprite16(t->m_tile, 0, 1); // Top row of sprite sheet
      pd->graphics->drawBitmap(b, u * TILE_PIX, v * TILE_PIX, kBitmapUnflipped);
    }
  }

  #ifdef DEV
  setRoobert24();
  pd->graphics->drawRect(0, 0, CHUNK_PIX_X, CHUNK_PIX_Y, kColorBlack);
  static char text[16];
  snprintf(text, 16, "(%u,%u)", _chunk->m_x, _chunk->m_y);
  pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_PIX, TILE_PIX);
  #endif

  // Shift from Sprite to Bitmap draw coords
  const int16_t off_x = (_chunk->m_x * CHUNK_PIX_X) + TILE_PIX/2;
  const int16_t off_y = (_chunk->m_y * CHUNK_PIX_Y) + TILE_PIX/2;

  // Render locations into the background image
  for (uint32_t i = 0; i < _chunk->m_nBuildings; ++i) {
    struct Building_t* building = _chunk->m_buildings[i];
    if (building->m_type != kNoBuilding && building->m_image[1]) {
      pd->graphics->drawBitmap(building->m_image[1], building->m_pix_x - off_x, building->m_pix_y - off_y, kBitmapUnflipped);
    }
  }

  pd->graphics->popContext();

  for (uint32_t zoom = 2; zoom < ZOOM_LEVELS; ++zoom) {
    pd->graphics->pushContext(_chunk->m_bkgImage[zoom]);
    pd->graphics->drawScaledBitmap(_chunk->m_bkgImage[1], 0, 0, (float)zoom, (float)zoom);
    pd->graphics->popContext();
  }

}

void generateSpriteSetup(struct Chunk_t* _chunk) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    if (_chunk->m_bkgImage[zoom] == NULL) _chunk->m_bkgImage[zoom] = pd->graphics->newBitmap(CHUNK_PIX_X*zoom, CHUNK_PIX_Y*zoom, kColorClear);
    // Not enough RAM for this :(
    //if (_chunk->m_bkgImage2[zoom] == NULL) _chunk->m_bkgImage2[zoom] = pd->graphics->newBitmap(CHUNK_PIX_X*zoom, CHUNK_PIX_Y*zoom, kColorClear);
    // ...
    if (_chunk->m_bkgSprite[zoom] == NULL) _chunk->m_bkgSprite[zoom] = pd->sprite->newSprite();
    PDRect bound = {.x = 0, .y = 0, .width = CHUNK_PIX_X*zoom, .height = CHUNK_PIX_Y*zoom};
    pd->sprite->setBounds(_chunk->m_bkgSprite[zoom], bound);
    pd->sprite->setImage(_chunk->m_bkgSprite[zoom], _chunk->m_bkgImage[zoom], kBitmapUnflipped);
    pd->sprite->moveTo(_chunk->m_bkgSprite[zoom], (CHUNK_PIX_X*_chunk->m_x + CHUNK_PIX_X/2.0)*zoom, (CHUNK_PIX_Y*_chunk->m_y + CHUNK_PIX_Y/2.0)*zoom);
    pd->sprite->setZIndex(_chunk->m_bkgSprite[zoom], -1);
  }
}

void setChunkBackgrounds() {
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      struct Chunk_t* chunk = getChunk_noCheck(x, y);
      generateSpriteSetup(chunk);
      renderChunkBackgroundImage(chunk);
    }
  }
}

void resetWorld() {
  memset(m_tiles, 0, SIZE_GENERATE);
  m_deserialiseIndexWorld = 0;
}

void initWorld() {
  m_tiles = pd->system->realloc(NULL, SIZE_GENERATE);
  pd->system->logToConsole("malloc: for world %i", SIZE_GENERATE/1024);
}

void serialiseWorld(struct json_encoder* je) {
  je->addTableMember(je, "world", 5);
  je->startArray(je);

  for (uint32_t i = 0; i < TOT_TILES; ++i) {
    je->addArrayMember(je);
    je->startTable(je);
    je->addTableMember(je, "tile", 4);
    je->writeInt(je, m_tiles[i].m_tile);
    je->endTable(je);
  }

  je->endArray(je);
}

void deserialiseValueWorld(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "tile") == 0) {
    m_tiles[m_deserialiseIndexWorld].m_tile = json_intValue(_value);
  } else {
    pd->system->error("WORLD DECODE ISSUE, %s", _key);
  }
}

void* deserialiseStructDoneWorld(json_decoder* jd, const char* _name, json_value_type _type) {
  ++m_deserialiseIndexWorld;
  return NULL;
}


void generate() {
  srand(pd->system->getSecondsSinceEpoch(NULL));
    
  // Worldgen is very basic for now
  for (uint16_t i = 0; i < TOT_TILES; ++i) {
    m_tiles[i].m_tile = rand() % FLOOR_TILES;
  } 

  //addLake(0,0);


}
