#include "game.h"
#include "generate.h"
#include "sprite.h"
#include "chunk.h"
#include "location.h"

struct Tile_t* m_tiles = NULL;

const int32_t SIZE_GENERATE = TOT_TILES * sizeof(struct Tile_t);

void setChunkBackgrounds(void);

void setChunkAssociations(void);

void setNonNeighborAssociationsFor(struct Chunk_t* _chunk);

void doNonNeighborAssociation(struct Chunk_t* _chunk, struct Chunk_t** _theNeighborList, struct Chunk_t** _theNonNeighborList, uint32_t _theListSize);
 
void generateSpriteSetup(struct Chunk_t* _chunk, uint8_t _zoom);

struct Tile_t* getTile(struct Chunk_t* _chunk, uint16_t _u, uint16_t _v);

/// ///

struct Tile_t* getTile(struct Chunk_t* _chunk, uint16_t _u, uint16_t _v) {
  return &m_tiles[ (WORLD_CHUNKS_X * TILES_PER_CHUNK_X)*((TILES_PER_CHUNK_Y * _chunk->m_y) + _v) + ((TILES_PER_CHUNK_X * _chunk->m_x) + _u) ];
}

void renderChunkBackgroundImage(struct Chunk_t* _chunk) {
  pd->graphics->pushContext(_chunk->m_bkgImage[1]);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->fillRect(0, 0, CHUNK_PIX_X, CHUNK_PIX_Y, kColorWhite);
  for (uint16_t v = 0; v < TILES_PER_CHUNK_Y; ++v) {
    for (uint16_t u = 0; u < TILES_PER_CHUNK_X; ++u) {
      struct Tile_t* t = getTile(_chunk, u, v);
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
  for (uint32_t i = 0; i < _chunk->m_nLocations; ++i) {
    struct Location_t* loc = _chunk->m_locations[i];
    if (loc->m_building && loc->m_building->m_type != kNoBuilding && loc->m_building->m_image) {
      pd->graphics->drawBitmap(loc->m_building->m_image[1], loc->m_building->m_pix_x - off_x, loc->m_building->m_pix_y - off_y, kBitmapUnflipped);
    }
  }

  pd->graphics->popContext();

  for (uint32_t zoom = 2; zoom < ZOOM_LEVELS; ++zoom) {
    pd->graphics->pushContext(_chunk->m_bkgImage[zoom]);
    pd->graphics->drawScaledBitmap(_chunk->m_bkgImage[1], 0, 0, (float)zoom, (float)zoom);
    pd->graphics->popContext();
  }

}


void doNonNeighborAssociation(struct Chunk_t* _chunk, struct Chunk_t** _theNeighborList, struct Chunk_t** _theNonNeighborList, uint32_t _theListSize) {
  uint32_t counter = 0;
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      struct Chunk_t* testChunk = getChunk_noCheck(x, y);

      bool found = (testChunk == _chunk);
      for (uint32_t i = 0; i < _theListSize; ++i) {
        if (_theNeighborList[i] == testChunk) {
          found = true;
          break;
        }
      }

      if (!found) {
        _theNonNeighborList[counter++] = testChunk;
      }
    }
  }
}

void setNonNeighborAssociationsFor(struct Chunk_t* _chunk) {
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsNE, _chunk->m_nonNeighborsNE, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsSE, _chunk->m_nonNeighborsSE, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsSW, _chunk->m_nonNeighborsSW, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsNW, _chunk->m_nonNeighborsNW, CHUNK_NEIGHBORS_CORNER);
  doNonNeighborAssociation(_chunk, _chunk->m_neighborsALL, _chunk->m_nonNeighborsALL, CHUNK_NEIGHBORS_ALL);
}

// Each chunk should know who its neighbors are for fast rendering
void setChunkAssociations(void) {
  // Set neighbor associations
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      struct Chunk_t* c = getChunk_noCheck(x, y);
      c->m_neighborsNE[0] = getChunk(x + 0, y - 1);
      c->m_neighborsNE[1] = getChunk(x + 1, y - 1);
      c->m_neighborsNE[2] = getChunk(x + 1, y + 0);

      c->m_neighborsSE[0] = getChunk(x + 1, y + 0);
      c->m_neighborsSE[1] = getChunk(x + 1, y + 1);
      c->m_neighborsSE[2] = getChunk(x + 0, y + 1);

      c->m_neighborsSW[0] = getChunk(x + 0, y + 1);
      c->m_neighborsSW[1] = getChunk(x - 1, y + 1);
      c->m_neighborsSW[2] = getChunk(x - 1, y + 0);

      c->m_neighborsNW[0] = getChunk(x - 1, y + 0);
      c->m_neighborsNW[1] = getChunk(x - 1, y - 1);
      c->m_neighborsNW[2] = getChunk(x + 0, y - 1);

      c->m_neighborsALL[0] = getChunk(x + 0, y - 1);
      c->m_neighborsALL[1] = getChunk(x + 1, y - 1);
      c->m_neighborsALL[2] = getChunk(x + 1, y + 0);
      c->m_neighborsALL[3] = getChunk(x + 1, y + 1);
      c->m_neighborsALL[4] = getChunk(x + 0, y + 1);
      c->m_neighborsALL[5] = getChunk(x - 1, y + 1);
      c->m_neighborsALL[6] = getChunk(x - 1, y + 0);
      c->m_neighborsALL[7] = getChunk(x - 1, y - 1);
    }
  }

  // Set non-neighbor associations
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      setNonNeighborAssociationsFor(getChunk_noCheck(x, y));
    }
  }
}

void generateSpriteSetup(struct Chunk_t* _chunk, uint8_t _zoom) {
  PDRect bound = {.x = 0, .y = 0, .width = CHUNK_PIX_X*_zoom, .height = CHUNK_PIX_Y*_zoom};
  pd->sprite->setBounds(_chunk->m_bkgSprite[_zoom], bound);
  pd->sprite->setImage(_chunk->m_bkgSprite[_zoom], _chunk->m_bkgImage[_zoom], kBitmapUnflipped);
  pd->sprite->moveTo(_chunk->m_bkgSprite[_zoom], (CHUNK_PIX_X*_chunk->m_x + CHUNK_PIX_X/2.0)*_zoom, (CHUNK_PIX_Y*_chunk->m_y + CHUNK_PIX_Y/2.0)*_zoom);
  pd->sprite->setZIndex(_chunk->m_bkgSprite[_zoom], -1);
}

void setChunkBackgrounds(void) {
  for (uint16_t y = 0; y < WORLD_CHUNKS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_CHUNKS_X; ++x) {
      struct Chunk_t* c = getChunk(x, y);
      if (c->m_bkgImage[1] == NULL) {
        for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
          c->m_bkgImage[zoom] = pd->graphics->newBitmap(CHUNK_PIX_X*zoom, CHUNK_PIX_Y*zoom, kColorClear);
        }
      }
      if (c->m_bkgSprite[1] == NULL) {
        for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
          c->m_bkgSprite[zoom] = pd->sprite->newSprite();
        }
      }

      renderChunkBackgroundImage(c);

      for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
        generateSpriteSetup(c, zoom);
      }
    }
  }
}

void resetWorld() {
  memset(m_tiles, 0, SIZE_GENERATE);
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


void generate() {
  srand(pd->system->getSecondsSinceEpoch(NULL));
    
  // Worldgen is very basic for now
  for (uint16_t i = 0; i < TOT_TILES; ++i) {
    m_tiles[i].m_tile = rand() % FLOOR_TILES;
  } 

  // The main playing grid
  setChunkBackgrounds();

  // The neighboring associations, including modulo behavior
  setChunkAssociations();

}
