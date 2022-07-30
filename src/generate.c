#include "generate.h"
#include "sprite.h"
#include "location.h"
#include "ui.h"
#include "io.h"
#include "sprite.h"
#include "building.h"
#include "buildings/special.h"
#include "buildings/extractor.h"

#include <math.h>

//                             {kSiltWorld, kChalkWorld, kPeatWorld, kSandWorld, kClayWorld, kLoamWoarld, kWaterWorld, kEmptyWorld};
const uint16_t kWarpUIIcon[] = {SID(0,18),  SID(1,18),   SID(2,18),  SID(3,18),  SID(4,18),  SID(5,18),   SID(6,18),   SID(7,18)};
const uint16_t kWarpPrice[]  = {0,          100,         200,        300,        400,        500,         600,         700};

struct Tile_t* m_tiles = NULL;

uint16_t m_deserialiseIndexWorld = 0;

const int32_t SIZE_GENERATE = TOT_TILES * sizeof(struct Tile_t);
 
void generateSpriteSetup(struct Chunk_t* _chunk);

struct Tile_t* getTileInChunk(struct Chunk_t* _chunk, int32_t _u, int32_t _v);

bool addLake(int32_t _startX, int32_t _startY, int32_t _riverProb);

bool addRiver(int32_t _startX, int32_t _startY, enum kDir _dir, int32_t _lakeProb, bool _isolated);

void addSpawn(void);

void doLakesAndRivers(uint8_t _slot);

void doClutterObstacles(void);

float pointDist(int32_t _x, int32_t _y, int32_t _x1, int32_t _y1, int32_t _x2, int32_t _y2);

void doSea(void);


/// ///

enum kGroundType getWorldGround(uint8_t _slotNumber, uint8_t _groundCounter) {
  if (_slotNumber == kEmptyWorld) return kSiltyGround;
  else if (_slotNumber == kWaterWorld) return kClayGround;
  return (_slotNumber + _groundCounter) % kPavedGround; 
}

struct Tile_t* getTileInChunk(struct Chunk_t* _chunk, int32_t _u, int32_t _v) {
  return &m_tiles[ (WORLD_CHUNKS_X * TILES_PER_CHUNK_X)*((TILES_PER_CHUNK_Y * _chunk->m_y) + _v) + ((TILES_PER_CHUNK_X * _chunk->m_x) + _u) ];
}

struct Tile_t* getTile(int32_t _x, int32_t _y) {
  if (_x < 0) _x += TOT_TILES_X;
  else if (_x >= TOT_TILES_X) _x -= TOT_TILES_X;
  if (_y < 0) _y += TOT_TILES_Y;
  else if (_y >= TOT_TILES_Y) _y -= TOT_TILES_Y;
  return &m_tiles[ TOT_TILES_X*_y + _x ];
}

struct Tile_t* getTile_fromLocation(struct Location_t* _loc) {
  return getTile(_loc->m_x, _loc->m_y);
}

float pointDist(int32_t _x, int32_t _y, int32_t _x1, int32_t _y1, int32_t _x2, int32_t _y2) {

  int32_t A = _x - _x1;
  int32_t B = _y - _y1;
  int32_t C = _x2 - _x1;
  int32_t D = _y2 - _y1;

  float dot = A * C + B * D;
  int32_t len_sq = C * C + D * D;
  float param = -1;
  if (len_sq != 0) { //in case of 0 length line
    param = dot / len_sq;
  }

  int32_t xx, yy;

  if (param < 0) {
    xx = _x1;
    yy = _y1;
  } else if (param > 1) {
    xx = _x2;
    yy = _y2;
  } else {
    xx = _x1 + param * C;
    yy = _y1 + param * D;
  }

  int32_t dx = _x - xx;
  int32_t dy = _y - yy;
  return sqrt(dx * dx + dy * dy);
}

void renderChunkBackgroundImageAround(struct Chunk_t* _chunk) {
  for (int32_t x = _chunk->m_x - 1; x < _chunk->m_x + 2; ++x) {
    for (int32_t y = _chunk->m_y - 1; y < _chunk->m_y + 2; ++y) {
      struct Chunk_t* toRedraw = getChunk(x,y);
      //pd->system->logToConsole("REDRAW %i %i", toRedraw->m_x, toRedraw->m_y);
      renderChunkBackgroundImage(toRedraw);
    }
  }
}

uint8_t getNearbyBackground(struct Chunk_t* _chunk, uint16_t _u, uint16_t _v) {
  uint8_t t = getTileInChunk(_chunk, _u - 1, _v)->m_tile;
  if (t < TOT_FLOOR_TILES) return t;
  t = getTileInChunk(_chunk, _u + 1, _v)->m_tile;
  if (t < TOT_FLOOR_TILES) return t;
  t = getTileInChunk(_chunk, _u, _v - 1)->m_tile;
  if (t < TOT_FLOOR_TILES) return t;
  t = getTileInChunk(_chunk, _u, _v + 1)->m_tile;
  if (t < TOT_FLOOR_TILES) return t;
  return 0;
}

void renderChunkBackgroundImage(struct Chunk_t* _chunk) {
  pd->graphics->pushContext(_chunk->m_bkgImage[1]);
  pd->graphics->setDrawMode(kDrawModeCopy);
  pd->graphics->fillRect(0, 0, CHUNK_PIX_X, CHUNK_PIX_Y, kColorWhite);

  // Draw background
  for (uint16_t v = 0; v < TILES_PER_CHUNK_Y; ++v) {
    for (uint16_t u = 0; u < TILES_PER_CHUNK_X; ++u) {
      struct Tile_t* t = getTileInChunk(_chunk, u, v);
      LCDBitmapFlip flip = kBitmapUnflipped;
      if (isGroundTile(t)) {
        flip = (u+v) % 2 ? kBitmapUnflipped : kBitmapFlippedX;
      } else {
        // For other tiles, draw them on top of a background tile
        uint16_t nearby = getNearbyBackground(_chunk, u, v);
        pd->graphics->drawBitmap(getSprite16_byidx(nearby, 1), 
          u * TILE_PIX, v * TILE_PIX, kBitmapUnflipped);
      }
      pd->graphics->drawBitmap(getSprite16_byidx(t->m_tile, 1), 
        u * TILE_PIX, v * TILE_PIX, flip);
    }
  }

  if (getPlayer()->m_enableDebug) {
    setRoobert24();
    pd->graphics->drawRect(0, 0, CHUNK_PIX_X, CHUNK_PIX_Y, kColorBlack);
    static char text[16];
    snprintf(text, 16, "(%u,%u)", _chunk->m_x, _chunk->m_y);
    pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_PIX, TILE_PIX);
  }

  // Shift from Sprite to Bitmap draw coords
  const int16_t off16_x = (_chunk->m_x * CHUNK_PIX_X) + TILE_PIX/2;
  const int16_t off16_y = (_chunk->m_y * CHUNK_PIX_Y) + TILE_PIX/2;

  const int16_t off48_x = (_chunk->m_x * CHUNK_PIX_X) + 3*TILE_PIX/2;
  const int16_t off48_y = (_chunk->m_y * CHUNK_PIX_Y) + 3*TILE_PIX/2;

  // Render farmland
  for (uint32_t i = 0; i < _chunk->m_nBuildings; ++i) {
    struct Building_t* building = _chunk->m_buildings[i];
    if (building->m_type == kPlant) {
      if (building->m_mode.mode16 >= 2 * N_CROPS_BEFORE_FARMLAND) { // Draw farm land
        pd->graphics->drawBitmap(getSprite16(7,12,1), building->m_pix_x - off16_x, building->m_pix_y - off16_y, kBitmapUnflipped);
      } else if (building->m_mode.mode16 >= N_CROPS_BEFORE_FARMLAND) { // Draw farm land
        pd->graphics->drawBitmap(getSprite16(6,12,1), building->m_pix_x - off16_x, building->m_pix_y - off16_y, kBitmapUnflipped);
      } 
    }
  }

  // Render wetness
  for (uint16_t v = 0; v < TILES_PER_CHUNK_Y; ++v) {
    for (uint16_t u = 0; u < TILES_PER_CHUNK_X; ++u) {
      struct Tile_t* t = getTileInChunk(_chunk, u, v);
      if (t->m_wetness >= 0 && t->m_wetness < 8) { // If wet, but not actually water
        for (int32_t w = 0; w < (8 - t->m_wetness); w += 2) {
          pd->graphics->drawBitmap(getSprite16(12 + (w/2 + v*u)%4, 16, 1), u * TILE_PIX, v * TILE_PIX, kBitmapUnflipped);
        }
      }
    }
  }

  // Render locations 
  for (uint32_t i = 0; i < _chunk->m_nBuildings; ++i) {
    struct Building_t* building = _chunk->m_buildings[i];
    if (building->m_type != kNoBuilding && building->m_image[1]) {
      if (building->m_type >= kExtractor) {
        const bool invert = (building->m_type == kExtractor && building->m_subType.extractor == kCropHarvesterLarge);
        const bool flip = (building->m_type == kSpecial && building->m_subType.special == kImportBox && building->m_dir != SN);
        pd->graphics->setDrawMode(invert ? kDrawModeInverted : kDrawModeCopy);
        pd->graphics->drawBitmap(building->m_image[1], building->m_pix_x - off48_x, building->m_pix_y - off48_y, flip ? kBitmapFlippedX : kBitmapUnflipped);
        pd->graphics->setDrawMode(kDrawModeCopy);
        // If factory or farm - draw also what is produced
        if (building->m_type == kFactory) {
          pd->graphics->drawBitmap(getSprite16_byidx(getUIIcon(kUICatFactory, building->m_subType.factory), 1), 
            building->m_pix_x - off16_x, building->m_pix_y - off16_y, kBitmapUnflipped);
        } else if (building->m_type == kExtractor) {
          pd->graphics->drawBitmap(getSprite16_byidx(CargoDesc[ EDesc[building->m_subType.extractor].out ].UIIcon, 1), 
            building->m_pix_x - off16_x, building->m_pix_y - off16_y, kBitmapUnflipped);
        }
      } else {
        // Fast conveyors get drawn inverted. Stored[0] is used to hold the speed
        const bool invert = (building->m_type == kConveyor && building->m_stored[0] >= 2);
        pd->graphics->setDrawMode(invert ? kDrawModeInverted : kDrawModeCopy);
        pd->graphics->drawBitmap(building->m_image[1], building->m_pix_x - off16_x, building->m_pix_y - off16_y, kBitmapUnflipped);
        pd->graphics->setDrawMode(kDrawModeCopy);
      }
    }
  }

  // Render locations from nearby chunks, in case they overlap
  for (int32_t x = -1; x < 2; ++x) {
    for (int32_t y = -1; y < 2; ++y) {
      if (!x && !y) continue;
      struct Chunk_t* otherChunk = getChunk(_chunk->m_x + x, _chunk->m_y + y);
      int32_t chunkOffX = (otherChunk->m_x * CHUNK_PIX_X) + (3*TILE_PIX/2) - (CHUNK_PIX_X * x);
      int32_t chunkOffY = (otherChunk->m_y * CHUNK_PIX_Y) + (3*TILE_PIX/2) - (CHUNK_PIX_Y * y);

      for (uint32_t i = 0; i < otherChunk->m_nBuildings; ++i) {
        struct Building_t* building = otherChunk->m_buildings[i];
        if (building->m_type >= kExtractor && building->m_image[1]) {
          const bool invert = (building->m_type == kExtractor && building->m_subType.extractor == kCropHarvesterLarge);
          pd->graphics->setDrawMode(invert ? kDrawModeInverted : kDrawModeCopy);
          pd->graphics->drawBitmap(building->m_image[1], building->m_pix_x - chunkOffX, building->m_pix_y - chunkOffY, kBitmapUnflipped);
          pd->graphics->setDrawMode(kDrawModeCopy);
        }
      }
    }
  }

  pd->graphics->popContext();

  for (uint32_t zoom = 2; zoom < ZOOM_LEVELS; ++zoom) {
    pd->graphics->pushContext(_chunk->m_bkgImage[zoom]);
    pd->graphics->drawScaledBitmap(_chunk->m_bkgImage[1], 0, 0, (float)zoom, (float)zoom);
    pd->graphics->popContext();
  }

}

#define SPAWN_START_X 6
#define SPAWN_Y TILES_PER_CHUNK_Y
#define SPAWN_END_X 42
#define SPAWN_RADIUS 4
void addSpawn() {
  for (int32_t x = 0; x < SPAWN_END_X+SPAWN_RADIUS; ++x) {
    for (int32_t y = 0; y < SPAWN_Y+SPAWN_RADIUS; ++y) {
      if (pointDist(x, y, SPAWN_START_X, SPAWN_Y, SPAWN_END_X, SPAWN_Y) < SPAWN_RADIUS) {
        getTile(x, y)->m_tile = SPRITE16_ID(0, 3) + rand() % 4;
      }
    }
  }
}

#define BIOME_RADIUS 16
#define BIOME_POINTS_TOO_CLOSE (TOT_TILES_X/2 - 3*BIOME_RADIUS)
void addBiome(int32_t _offX, int32_t _offY, uint16_t _imgStart) {
  bool good = false;
  int32_t x1, y1, x2, y2;
  while (!good) {
    x1 = BIOME_RADIUS + rand() % (TOT_TILES_X/2 - 2*BIOME_RADIUS);
    y1 = BIOME_RADIUS + rand() % (TOT_TILES_Y/2 - 2*BIOME_RADIUS);
    x2 = BIOME_RADIUS + rand() % (TOT_TILES_X/2 - 2*BIOME_RADIUS);
    y2 = BIOME_RADIUS + rand() % (TOT_TILES_Y/2 - 2*BIOME_RADIUS);
    if (sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) ) > BIOME_POINTS_TOO_CLOSE) break;
  }
  for (int32_t x = _offX; x < TOT_TILES_X/2 + _offX; ++x) {
    for (int32_t y = _offY; y < TOT_TILES_Y/2 + _offY; ++y) {
      if (pointDist(x, y, x1 + _offX, y1 + _offY, x2 + _offX, y2 + _offY) < BIOME_RADIUS) {
        getTile(x, y)->m_tile = _imgStart + (rand() % FLOOR_VARIETIES);
      }
    }
  }
}

bool isGroundTile(struct Tile_t* _tile) {
  return _tile->m_tile < TOT_FLOOR_TILES;
}

bool isGroundTypeTile(int32_t _x, int32_t _y, enum kGroundType _ground) {
  const uint16_t t = getTile(_x, _y)->m_tile;
  return (t >= _ground * FLOOR_VARIETIES && t < (_ground + 1) * FLOOR_VARIETIES);
}

bool isWaterTile(int32_t _x, int32_t _y) {
  const uint16_t t = getTile(_x, _y)->m_tile;
  if (t < TOT_FLOOR_TILES) return false;
  if (t >= SPRITE16_ID(0, 5)  && t < SPRITE16_ID(0, 7)) return true;
  if (t >= SPRITE16_ID(4, 11) && t < SPRITE16_ID(8, 11)) return true;
  if (t >= SPRITE16_ID(4, 12) && t < SPRITE16_ID(6, 12)) return true;
  if (t >= SPRITE16_ID(4, 13) && t < SPRITE16_ID(8, 13)) return true;
  if (t >= SPRITE16_ID(4, 14) && t < SPRITE16_ID(8, 14)) return true;
  return false;
}

uint8_t distanceFromWater(int32_t _x, int32_t _y) {
  uint8_t r = 0;
  if (isWaterTile(_x, _y)) return 0;
  while (++r < 16) {
    for (int32_t x = _x - r; x < _x + r + 1; ++x) {
      if (isWaterTile(x, _y - r)) return r;
      if (isWaterTile(x, _y + r)) return r;
    }
    for (int32_t y = _y - r; y < _y + r + 1; ++y) {
      if (isWaterTile(_x + r, y)) return r;
      if (isWaterTile(_x - r, y)) return r;
    }
  }
  return 255;
}

void doWetness(bool _forTitles) {
  const uint16_t maxY = (_forTitles ? TILES_PER_CHUNK_Y * 3 : TOT_TILES_Y);
  const uint16_t maxX = (_forTitles ? TILES_PER_CHUNK_X * 3 : TOT_TILES_X);
  for (int32_t x = 0; x < maxX; ++x) {
    for (int32_t y = 0; y < maxY; ++y) {
      getTile(x, y)->m_wetness = distanceFromWater(x,y);
    }
  }
}

enum kGroundWetness getWetness(uint8_t _wetness) {
  if (_wetness == 0) {
    return kWater;
  } else if (_wetness < 4) {
    return kWet;
  } else if (_wetness < 8) {
    return kMoist;
  } else {
    return kDry;
  }
}

enum kGroundType getGroundType(uint8_t _tile) {
  if (_tile < FLOOR_VARIETIES) {
    return kSiltyGround;
  } else if (_tile < FLOOR_VARIETIES*2) {
    return kChalkyGround;
  } else if (_tile < FLOOR_VARIETIES*3) {
    return kPeatyGround;
  } else if (_tile < FLOOR_VARIETIES*4) {
    return kSandyGround;
  } else if (_tile < FLOOR_VARIETIES*5) {
    return kClayGround;
  } else if (_tile < FLOOR_VARIETIES*6) {
    return kLoamyGround;
  } else if (_tile < FLOOR_VARIETIES*7) {
    return kPavedGround;
  } else if (_tile >= SID(12,13) && _tile <= SID(15,13)) {
    return kObstructedGround;
  } else if (_tile < 192) {
    return kLake;
  } else if (_tile < 232) {
    return kRiver;
  } else {
    return kNGroundTypes;
  }
}

const char* toStringWetness(enum kGroundWetness _wetness) {
  switch (_wetness) {
    case kWater: return "Water";
    case kWet: return "Wet";
    case kMoist: return "Moist";
    default: return "Dry";
  }
}

const char* toStringSoil(enum kGroundType _type) {
  switch (_type) {
    case kSiltyGround: return "Silty Soil";
    case kChalkyGround: return "Chalky Soil";
    case kPeatyGround: return "Peaty Soil";
    case kSandyGround: return "Sandy Soil";
    case kClayGround: return "Clay Soil";
    case kLoamyGround: return "Loamy Soil";
    case kPavedGround: return "Paved Ground";
    case kObstructedGround: return "Obstructed Ground";
    case kLake: return "Lake";
    case kRiver: return "River";
    case kNGroundTypes: return "UNKNOWN Soil";
  }
  return "UNKNOWN Soil";
}

const char* getWorldName(enum kWorldType _type, bool _mask) {
  switch (_type) {
    case kSiltWorld: return _mask ? "??? ?????????" : "The Grasslands"; // Main: Silt
    case kChalkWorld: return _mask ? "??? ????? ???????" : "The Stony Plateau"; // Main: Chalk
    case kPeatWorld: return _mask ? "??? ????? ??????" : "The Boggy Hollow"; // Main: Peat
    case kSandWorld: return _mask ? "??? ??????" : "The Desert"; // Main: Sand
    case kClayWorld: return _mask ? "??? ???????" :"The Estuary"; // Main: Clay
    case kLoamWoarld: return _mask ? "??? ??????? ?????" :"The Fertile Hills"; // Main: Loaamy
    case kWaterWorld: return _mask ? "??? ??? ?????" :"The Mud Flats"; // Special, Main: Water
    case kEmptyWorld: return _mask ? "??? ??????" :"The Plains"; // Special, all Silty - Empty
  }
  return "UNKNOWN World";
}

void doClutterObstacles() {
  for (int32_t x = 0; x < TOT_TILES_X; ++x) {
    for (int32_t y = 0; y < TOT_TILES_Y; ++y) {
      if (rand() % 1000 >= CLUTTER_CHANCE) {
        continue;
      }
      struct Tile_t* t = getTile(x,y);
      if (t->m_tile >= TOT_FLOOR_TILES) {
        continue;
      }
      if (getGroundType(t->m_tile) == kSandyGround) {
        t->m_tile = rand() % 2 ? SPRITE16_ID(14, 13) : SPRITE16_ID(15, 13);
      } else {
        t->m_tile = rand() % 2 ? SPRITE16_ID(12, 13) : SPRITE16_ID(13, 13);
      }
    }
  }
}

#define LAKE_MIN 10
// was 18
#define LAKE_MAX 14
#define RIVER_MIN 8
// was 32
#define RIVER_MAX 16
bool addRiver(int32_t _startX, int32_t _startY, enum kDir _dir, int32_t _lakeProb, bool _isolated) {

  if (getTile(_startX,_startY)->m_tile > TOT_FLOOR_TILES) return true; // Reject 

  if (_isolated) {
    getTile(_startX,_startY)->m_tile = SPRITE16_ID(6+(_dir == NS ? 0 : 1), 14);
  } else {
    getTile(_startX,_startY)->m_tile = SPRITE16_ID(5+(_dir == NS ? 1 : 0), 11);
  }

  int32_t lenA = RIVER_MIN + rand() % (RIVER_MAX - RIVER_MIN);
  bool switchA = rand() % 2;
  bool switchDir = rand() % 2;
  int32_t lenB = RIVER_MIN + rand() % (RIVER_MAX - RIVER_MIN);
  bool switchB = switchA && rand() % 2;
  int32_t lenC = RIVER_MIN + rand() % (RIVER_MAX - RIVER_MIN);

  bool endInLake = !_isolated && rand() % 2;

  struct Tile_t* t;

  if (_dir == WE) {
    for (int32_t x = _startX + 1; x < _startX + lenA; ++x) {
      t = getTile(x, _startY);
      if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
      t->m_tile = SPRITE16_ID(5, 12);
    }
    if (switchA) getTile(_startX + lenA, _startY)->m_tile = SPRITE16_ID(6 + (switchDir ? 0 : 1), 13);
    else if (endInLake) {
      bool lakeGood = addLake(_startX + lenA, _startY - LAKE_MIN/2, _lakeProb);
      getTile(_startX + lenA , _startY)->m_tile = SPRITE16_ID(7, 11);
      return lakeGood;
    } else getTile(_startX + lenA, _startY)->m_tile = SPRITE16_ID(5, 14);
  } else if (_dir == NS) {
    for (int32_t y = _startY + 1; y < _startY + lenA; ++y) {
      t = getTile(_startX, y);
      if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
      t->m_tile = SPRITE16_ID(4, 12);
    }
    if (switchA) getTile(_startX, _startY + lenA)->m_tile = SPRITE16_ID(4 + (switchDir ? 0 : 3), 13);
    else if (endInLake) {
      bool lakeGood = addLake(_startX - LAKE_MIN/2, _startY + lenA, _lakeProb);
      getTile(_startX, _startY + lenA)->m_tile = SPRITE16_ID(4, 11);
      return lakeGood;
    } else getTile(_startX, _startY + lenA)->m_tile = SPRITE16_ID(4, 14);
  }

  if (switchA && _dir == WE) {
    if (switchDir) {
      for (int32_t y = _startY + 1; y < _startY + lenB; ++y) {
        t = getTile(_startX + lenA, y);
        if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
        t->m_tile = SPRITE16_ID(4, 12);
      }
      if (switchB) getTile(_startX + lenA, _startY + lenB)->m_tile = SPRITE16_ID(4, 13);
      else getTile(_startX + lenA, _startY + lenB)->m_tile = SPRITE16_ID(4, 14);
    } else {
      for (int32_t y = _startY - 1; y > _startY - lenB; --y) {
        t = getTile(_startX + lenA, y);
        if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
        t->m_tile = SPRITE16_ID(4, 12);
      } 
      if (switchB) getTile(_startX + lenA, _startY - lenB)->m_tile = SPRITE16_ID(5, 13);
      else getTile(_startX + lenA, _startY - lenB)->m_tile = SPRITE16_ID(6, 14);
    }
  } else if (switchA && _dir == NS) {
    if (switchDir) {
      for (int32_t x = _startX + 1; x < _startX + lenB; ++x) {
        t = getTile(x, _startY + lenA);
        if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
        t->m_tile = SPRITE16_ID(5, 12);
      }
      if (switchB) getTile(_startX + lenB, _startY + lenA)->m_tile = SPRITE16_ID(6, 13);
      else getTile(_startX + lenB, _startY + lenA)->m_tile = SPRITE16_ID(5, 14);
    } else {
      for (int32_t x = _startX - 1; x > _startX - lenB; --x) {
        t = getTile(x, _startY + lenA);
        if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
        t->m_tile = SPRITE16_ID(5, 12);
      } 
      if (switchB) getTile(_startX - lenB, _startY + lenA)->m_tile = SPRITE16_ID(5, 13);
      else getTile(_startX - lenB, _startY + lenA)->m_tile = SPRITE16_ID(7, 14);
    }
  }

  int32_t signedLenB = lenB * (switchDir ? 1 : -1);
  if (switchB && _dir == WE) {
    for (int32_t x = _startX + lenA + 1; x < _startX + lenA + lenC; ++x) {
      t = getTile(x, _startY + signedLenB);
      if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
      t->m_tile = SPRITE16_ID(5, 12);
    }
    if (endInLake) {
      bool lakeGood = addLake(_startX + lenA + lenC, _startY + signedLenB - LAKE_MIN/2, _lakeProb);
      getTile(_startX + lenA + lenC, _startY + signedLenB)->m_tile = SPRITE16_ID(7, 11);
      return lakeGood;
    } else getTile(_startX + lenA + lenC, _startY + signedLenB)->m_tile = SPRITE16_ID(5, 14);
  } else if (switchB && _dir == NS) {
    for (int32_t y = _startY + lenA + 1; y < _startY + lenA + lenC; ++y) {
      t = getTile(_startX + signedLenB, y);
      if (t->m_tile > TOT_FLOOR_TILES) return true; // Reject 
      t->m_tile = SPRITE16_ID(4, 12);
    }
    if (endInLake) {
      bool lakeGood = addLake(_startX + signedLenB - LAKE_MIN/2, _startY + lenA + lenC, _lakeProb); 
      getTile(_startX +  signedLenB, _startY + lenA + lenC)->m_tile = SPRITE16_ID(4, 11);
      return lakeGood;
    } else getTile(_startX + signedLenB, _startY + lenA + lenC)->m_tile = SPRITE16_ID(4, 14);
  }

  return false;

}

bool addLake(int32_t _startX, int32_t _startY, int32_t _riverProb) {
  uint8_t w = rand() % (LAKE_MAX - LAKE_MIN) + LAKE_MIN; 
  uint8_t h = rand() % (LAKE_MAX - LAKE_MIN) + LAKE_MIN; 
  uint8_t c_x[4] = {0};
  uint8_t c_y[4] = {0};
  for (uint8_t i = 0; i < 4; ++i) {
    if (rand() % 2) {
      c_x[i] = 2 + rand() % 2;
      c_y[i] = 2 + rand() % 2;
    }
  }

  // Check
  for (int32_t x = _startX; x < _startX + w; ++x) {
    if (getTile(x,_startY)->m_tile > TOT_FLOOR_TILES) return true; // Reject
    if (getTile(x,_startY+h)->m_tile > TOT_FLOOR_TILES) return true; // Reject
  }
  for (int32_t y = _startY; y < _startY + h + 1; ++y) {
    if (getTile(_startX,y)->m_tile > TOT_FLOOR_TILES) return true; // Reject
    if (getTile(_startX+w,y)->m_tile > TOT_FLOOR_TILES) return true; // Reject
  }

  uint16_t tex;
  // LR
  for (int32_t x = _startX; x < _startX + w; ++x) {
    int32_t y = _startY;
    tex = SPRITE16_ID(4,5);
    if      (x == _startX) tex = SPRITE16_ID(0,5); // Corner
    else if (c_x[0] && (x-_startX) == c_x[0]) tex = SPRITE16_ID(0,6); // Inner corner
    else if (c_x[1] && (x-_startX) == w - c_x[1]) tex = SPRITE16_ID(1,6); // Inner corner

    if      (c_x[0] && (x-_startX) <     c_x[0] + 1) y += c_y[0];
    else if (c_x[1] && (x-_startX) > w - c_x[1] - 1) y += c_y[1];

    getTile(x,y)->m_tile = tex;

    y = _startY + h;
    tex = SPRITE16_ID(6,5);
    if (x == _startX) tex = SPRITE16_ID(3,5);
    else if (c_x[3] && (x-_startX) == c_x[3]) tex = SPRITE16_ID(3,6); // Inner corner
    else if (c_x[2] && (x-_startX) == w - c_x[2]) tex = SPRITE16_ID(2,6); // Inner corner


    if      (c_x[3] && (x-_startX) <     c_x[3] + 1) y -= c_y[3];
    else if (c_x[2] && (x-_startX) > w - c_x[2] - 1) y -= c_y[2];

    getTile(x,y)->m_tile = tex;
  }
  // TB
  for (int32_t y = _startY; y < _startY + h + 1; ++y) {
    int32_t x = _startX;
    tex = SPRITE16_ID(7,5);
    if      (y == _startY) tex = SPRITE16_ID(0,5); // Corner
    else if (c_y[0] && (y-_startY) == c_y[0])     tex = SPRITE16_ID(0,5);
    else if (c_y[3] && (y-_startY) == h - c_y[3]) tex = SPRITE16_ID(3,5);
    else if (y == _startY+h)                      tex = SPRITE16_ID(3,5);

    if      (c_y[0] && (y-_startY) <     c_y[0] ) x += c_x[0];
    else if (c_y[3] && (y-_startY) > h - c_y[3] ) x += c_x[3];

    getTile(x,y)->m_tile = tex;

    x = _startX + w;
    tex = SPRITE16_ID(5,5);
    if      (y == _startY) tex = SPRITE16_ID(1,5); // Corner
    else if (c_y[1] && (y-_startY) == c_y[1]) tex = SPRITE16_ID(1,5);
    else if (c_y[2] && (y-_startY) == h - c_y[2]) tex = SPRITE16_ID(2,5);
    else if (y == _startY+h)          tex = SPRITE16_ID(2,5);

    if      (c_y[1] && (y-_startY) <     c_y[1]) x -= c_x[1];
    else if (c_y[2] && (y-_startY) > h - c_y[2]) x -= c_x[2];

    getTile(x,y)->m_tile = tex;
  }

  // (Literal) Flood fill
  for (int32_t x = _startX + 1; x < _startX + w; ++x) {
    bool fill = false;
    for (int32_t y = _startY; y < _startY + h; ++y) {
      struct Tile_t* t = getTile(x,y);
      if (fill && t->m_tile > TOT_FLOOR_TILES) break;
      if (fill) t->m_tile = SPRITE16_ID(4 + rand() % 4, 6);
      if (!fill && t->m_tile > TOT_FLOOR_TILES && getTile(x,y+1)->m_tile <= TOT_FLOOR_TILES) fill = true;
    }
  }

  bool toAddRiver = rand() < _riverProb;
  bool riverWE = rand() % 2;
  if (toAddRiver) {
    if (riverWE) return addRiver(_startX + w, _startY + h/2, WE, /*lake prob=*/ _riverProb/2, /*isolated=*/ false);
    else return addRiver(_startX + w/2, _startY + h, NS, /*lake prob=*/ _riverProb/2, /*isolated=*/ false);
  }

  return false;
}

#define SEASTART (TILES_PER_CHUNK_Y*2)
#define SEAEND (TILES_PER_CHUNK_Y*5)
void doSea() {
  struct Tile_t* t = NULL;
  for (int32_t x = 0; x < TOT_TILES_X; ++x) {
    getTile(x, SEASTART-1)->m_tile = SPRITE16_ID(4, 5);
    if (rand() % TILES_PER_CHUNK_X/2 == 0) {
      addRiver(x, SEAEND, NS, /*lake prob=*/0, /*isolated=*/ true);
      getTile(x, SEAEND)->m_tile = SPRITE16_ID(6, 11);
    } else {
      getTile(x, SEAEND)->m_tile = SPRITE16_ID(6, 5);
    }
    for (int32_t y = SEASTART; y < SEAEND; ++y) {
      getTile(x, y)->m_tile = SPRITE16_ID(4 + rand() % 4, 6);
    }
  }
}

void doLakesAndRivers(uint8_t _slot) {
  struct Tile_t* backup = pd->system->realloc(NULL, SIZE_GENERATE);
  //int8_t addToRegion[7] = {1,1,1,1,1,1,1};
  //if (rand() % 2 == 0) addToRegion[rand() % 7] = 0;
  //if (rand() % 4 == 0) addToRegion[rand() % 7] = 0;

  uint8_t lakesToTry = 5;
  if (_slot == kPeatWorld) {
    lakesToTry = 32;
  } 

  for (uint8_t i = 0; i < lakesToTry; ++i) {
    //if (!addToRegion[i]) continue;
    for (int32_t try = 0; try < 5; ++try) {
      int32_t x = rand() % (TOT_TILES_X/2);
      int32_t y = rand() % (TOT_TILES_Y/2);
      switch (i) {
        case 1: x += TOT_TILES_X/2; break;
        case 2: y += TOT_TILES_Y/2; break;
        case 3: x += TOT_TILES_X/2; y += TOT_TILES_Y/2; break;
        case 4: x += TOT_TILES_X/4; y += TOT_TILES_Y/4; break;
        case 5: case 6: x = rand() % TOT_TILES_X; y = rand() % TOT_TILES_Y; break;
      }
      memcpy(backup, m_tiles, SIZE_GENERATE);
      bool reject = false;
      if (_slot == kPeatWorld) {
        bool riverWE = rand() % 2;
        if (riverWE) reject = addRiver(x, y, WE, 0, true);
        else reject = addRiver(x, y, NS, 0, true);
      } else {
        reject = addLake(x, y, RAND_MAX/2);
      }
      if (reject) {
        pd->system->logToConsole("generate: rejected %i lake system at %i,%i (try %i)", i, x, y, try);
        memcpy(m_tiles, backup, SIZE_GENERATE);
      } else {
        pd->system->logToConsole("generate: added %i lake system at %i,%i", i, x, y);
        break;
      }
    }
  }
  pd->system->realloc(backup, 0); // Free
  
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

void setChunkBackgrounds(bool _forTitles) {
  const uint16_t maxY = (_forTitles ? 3 : WORLD_CHUNKS_Y);
  const uint16_t maxX = (_forTitles ? 3 : WORLD_CHUNKS_X);
  for (uint16_t y = 0; y < maxY; ++y) {
    for (uint16_t x = 0; x < maxX; ++x) {
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


bool tileIsObstacle(struct Tile_t* _tile) {
  // With water as impassible
  //if (_tile->m_tile >= SPRITE16_ID(4,6) && _tile->m_tile <= SPRITE16_ID(7,6)) return false; // Water (only the border is obstacle)
  //if (_tile->m_tile >= SPRITE16_ID(8,16) && _tile->m_tile <= SPRITE16_ID(16,16)) return false; // Entry area tiles 
  //return _tile->m_tile > TOT_FLOOR_TILES;
  return _tile->m_tile >= SPRITE16_ID(12, 13) &&  _tile->m_tile <= SPRITE16_ID(15, 13);
}

void addObstacles() {
  for (uint32_t x = 0; x < TOT_TILES_X; ++x) {
    for (uint32_t y = 0; y < TOT_TILES_Y; ++y) {
      struct Tile_t* tile = getTile(x, y);
      if (!tileIsObstacle(tile)) {
        continue;
      }

      LCDSprite* obstacle_x1 = pd->sprite->newSprite();
      PDRect bound_x1 = {.x = (COLLISION_OFFSET_SMALL/2), .y = (COLLISION_OFFSET_SMALL/2), .width = TILE_PIX - COLLISION_OFFSET_SMALL, .height = TILE_PIX - COLLISION_OFFSET_SMALL};
      pd->sprite->setCollideRect(obstacle_x1, bound_x1);
      pd->sprite->moveTo(obstacle_x1, x * TILE_PIX, y * TILE_PIX);


      LCDSprite* obstacle_x2 = pd->sprite->newSprite();
      PDRect bound_x2 = {.x = COLLISION_OFFSET_SMALL, .y = COLLISION_OFFSET_SMALL, .width = (TILE_PIX-COLLISION_OFFSET_SMALL)*2, .height = (TILE_PIX-COLLISION_OFFSET_SMALL)*2};
      pd->sprite->setCollideRect(obstacle_x2, bound_x2);
      pd->sprite->moveTo(obstacle_x2, (x * TILE_PIX)*2, (y * TILE_PIX)*2);


      chunkAddObstacle(getLocation_noCheck(x, y)->m_chunk, obstacle_x1, obstacle_x2);
    }
  }
}

void generate(uint32_t _actionProgress) {

  const uint8_t slot = getSlot();
  uint8_t floorMain = getWorldGround(slot, 0);

  if (_actionProgress == 1) {

    srand(pd->system->getSecondsSinceEpoch(NULL));

  } else if (_actionProgress == 2) {

      
    // Worldgen is very basic for now
    for (uint16_t i = 0; i < TOT_TILES; ++i) {
      m_tiles[i].m_tile = (FLOOR_VARIETIES * floorMain) + rand() % FLOOR_VARIETIES;
    } 

  } else if (_actionProgress == 3) {

    addSpawn();

  } else if (_actionProgress == 4) {

    if (slot != kEmptyWorld) {
      addBiome(TOT_TILES_X/2, 0, FLOOR_VARIETIES*getWorldGround(slot, 1));
      addBiome(0, TOT_TILES_Y/2, FLOOR_VARIETIES*getWorldGround(slot, 2));
    }

  } else if (_actionProgress == 5) {

    if (slot == kWaterWorld) {
      doSea();
    }
    if (slot != kEmptyWorld && slot != kSandWorld) {
      doLakesAndRivers(slot);
    }

  } else if (_actionProgress == 6) {

    if (slot != kEmptyWorld) {
      doClutterObstacles();
    }

  } else if (_actionProgress == 7) {

    #define STARTX (TILES_PER_CHUNK_X/2)

    // By setting EW, we camouflage these buildings in the early game in slot 0
    const enum kDir camo = (slot == 0 ? EW : SN);
    newBuilding(getLocation_noCheck(STARTX + 0, TILES_PER_CHUNK_Y), SN, kSpecial, (union kSubType) {.special = kShop} );
    newBuilding(getLocation_noCheck(STARTX + 9, TILES_PER_CHUNK_Y), SN, kSpecial, (union kSubType) {.special = kSellBox} );
    newBuilding(getLocation_noCheck(STARTX + 18, TILES_PER_CHUNK_Y), camo, kSpecial, (union kSubType) {.special = kWarp} );
    newBuilding(getLocation_noCheck(STARTX + 27, TILES_PER_CHUNK_Y), camo, kSpecial, (union kSubType) {.special = kExportBox} );
    newBuilding(getLocation_noCheck(STARTX + 36, TILES_PER_CHUNK_Y), camo, kSpecial, (union kSubType) {.special = kImportBox} );

  } else if (_actionProgress == 8) {

    doWetness(/*for titles = */ false);

    // Finished
    float f; for (int32_t i = 0; i < 10000; ++i) for (int32_t j = 0; j < 10000; ++j) { f*=i*j; }
    pd->system->logToConsole("Generated %s",  getWorldName(slot, /*mask*/ false));

  }
}

void generateTitle() {

  const uint8_t slot = getSlot();
  uint8_t floorMain = getWorldGround(slot, 0);

  srand(0);

  for (uint16_t x = 0; x < TILES_PER_CHUNK_X*3; ++x) {
    for (uint16_t y = 0; y < TILES_PER_CHUNK_Y*3; ++y) {
      getTile(x,y)->m_tile = rand() % FLOOR_VARIETIES;
    } 
  }

  doWetness(/*for titles = */ true);

}
