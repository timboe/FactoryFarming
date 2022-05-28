#include "sprite.h"
#include "generate.h"


//struct LCDSprite* m_world[WORLD_CHUNKS_X][WORLD_CHUNKS_Y];
//LCDSprite* m_worldEdges[WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4]; // + 4 is corners

struct Chunk_t* m_chunks;
struct Chunk_t* m_edgeChunks;

#define CONV_START_Y 7


LCDBitmap* m_conveyorMasters[kConvDirN] = {NULL};

uint32_t m_nConveyors = 0, m_nCargo = 0;

struct Location_t* m_locations = NULL;

LCDBitmapTable* m_sheet16;

struct Player_t m_player;

LCDFont* m_fontRoobert24;
LCDFont* m_fontRoobert11;

LCDBitmap* loadImageAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDBitmap* _img = pd->graphics->loadBitmap(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading image at path '%s': %s", _path, _outErr);
  }
  return _img;
}

LCDBitmapTable* loadImageTableAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDBitmapTable* _table = pd->graphics->loadBitmapTable(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading image table at path '%s': %s", _path, _outErr);
  }
  return _table;
}

LCDFont* loadFontAtPath(const char* _path) {
  const char* _outErr = NULL;
  LCDFont* _f = pd->graphics->loadFont(_path, &_outErr);
  if (_outErr != NULL) {
    pd->system->error("Error loading font at path '%s': %s", _path, _outErr);
  }
  return _f;
}


LCDBitmap* getSprite16(uint32_t _x, uint32_t _y) {
  return pd->graphics->getTableBitmap(m_sheet16, (SHEET16_SIZE * _y) + _x);
}
 
struct Player_t* getPlayer() {
  return &m_player;
}

struct Chunk_t* getEdgeChunk(uint32_t _i) {
  return &(m_edgeChunks[_i]);
}

void animateConveyor() {
  static int8_t tick = 0;
  tick = (tick + 1) % 8;

  for (int32_t i = 0; i < kConvDirN; ++i) {
    pd->graphics->pushContext(m_conveyorMasters[i]);
    pd->graphics->drawBitmap(getSprite16(tick, CONV_START_Y + i), 0, 0, kBitmapUnflipped);
    pd->graphics->popContext();
  }

}

struct Location_t* getLocation(int32_t _x, int32_t _y) {
  if (_x < 0) _x += TOT_TILES_X;
  else if (_x >= TOT_TILES_X) _x -= TOT_TILES_X;
  if (_y < 0) _y += TOT_TILES_Y;
  else if (_y >= TOT_TILES_Y) _y -= TOT_TILES_Y;
  const uint32_t l = (TOT_TILES_X * _y) + _x;
  return &m_locations[l];
}

struct Chunk_t* getChunk(int32_t _x, int32_t _y) {
  if (_x < 0) _x += WORLD_CHUNKS_X;
  else if (_x >= WORLD_CHUNKS_X) _x -= WORLD_CHUNKS_X;
  if (_y < 0) _y += WORLD_CHUNKS_Y;
  else if (_y >= WORLD_CHUNKS_Y) _y -= WORLD_CHUNKS_Y;
  return getChunk_noCheck(_x, _y);
}

struct Chunk_t* getChunk_noCheck(const int32_t _x, const int32_t _y) {
  const uint32_t l = (WORLD_CHUNKS_X * _y) + _x;
  return &m_chunks[l];
}

struct Chunk_t* getChunk_fromLocation(struct Location_t* _loc) {
  //pd->system->logToConsole("ADD TO CHUNK: %i / %i = %i", (int)_loc->m_y, (int)TILES_PER_CHUNK_Y, (int)(_loc->m_y) / (int)TILES_PER_CHUNK_Y);
  return getChunk_noCheck(_loc->m_x / TILES_PER_CHUNK_X, _loc->m_y / TILES_PER_CHUNK_Y);
}



void conveyorUpdateFnSN(LCDSprite* conveyor) {
  struct Location_t* loc = (struct Location_t*) pd->sprite->getUserdata(conveyor);
  if (loc->m_cargo == NULL) return;
  if (loc->m_progress < TILE_PIX) {
    ++(loc->m_progress);
    pd->sprite->moveTo(loc->m_cargo, loc->m_x, loc->m_y - loc->m_progress);
  } else if (loc->m_next != NULL && loc->m_next->m_cargo == NULL) {
    loc->m_next->m_cargo = loc->m_cargo;
    loc->m_cargo = NULL;
    loc->m_next->m_progress = 0;
    loc->m_progress = 0;
  }
}

void conveyorUpdateFnNS(LCDSprite* conveyor) {
  struct Location_t* loc = (struct Location_t*) pd->sprite->getUserdata(conveyor);
  if (loc->m_cargo == NULL) return;
  //pd->system->logToConsole("NS %i", loc->m_progress);
  if (loc->m_progress < TILE_PIX) {
    ++(loc->m_progress);
    pd->sprite->moveTo(loc->m_cargo, loc->m_x, loc->m_y + loc->m_progress);
  } else if (loc->m_next != NULL && loc->m_next->m_cargo == NULL) {
    loc->m_next->m_cargo = loc->m_cargo;
    loc->m_cargo = NULL;
    loc->m_next->m_progress = 0;
    loc->m_progress = 0;
  }
}

void chunkAddLocation(struct Chunk_t* _chunk, struct Location_t* _loc) {
  _chunk->m_locations[ _chunk->m_nLocations ] = _loc;
  ++(_chunk->m_nLocations);
}


bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir) {
  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_type != kEmpty) return false;

  loc->m_sprite = pd->sprite->newSprite();
  loc->m_image = getSprite16(0, CONV_START_Y + _dir);
  loc->m_cargo = NULL;
  loc->m_type = kConveyor;
  loc->m_progress = 0;
  loc->m_x = _x;
  loc->m_y = _y;
  loc->m_pix_x = TILE_PIX*_x + TILE_PIX/2.0;
  loc->m_pix_y = TILE_PIX*_y + TILE_PIX/2.0;
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TILE_PIX};
  pd->sprite->setBounds(loc->m_sprite, bound);
  pd->sprite->setImage(loc->m_sprite, m_conveyorMasters[_dir], kBitmapUnflipped);
  pd->sprite->moveTo(loc->m_sprite, loc->m_pix_x, loc->m_pix_y);

  struct Location_t* above = getLocation(_x, _y - 1);
  struct Location_t* below = getLocation(_x, _y + 1);
  struct Location_t* left  = getLocation(_x - 1, _y);
  struct Location_t* right = getLocation(_x + 1, _y);
  switch (_dir) {
    case SN:;
      if (above->m_type == kConveyor) loc->m_next = above;
      if (below->m_type == kConveyor) below->m_next = loc;
      break;
    case NS:;
      if (above->m_type == kConveyor) above->m_next = loc;
      if (below->m_type == kConveyor) loc->m_next = below;
      break;
    case WE:;
      if (left->m_type  == kConveyor) left->m_next = loc;
      if (right->m_type == kConveyor) loc->m_next = right;
      break;
    case EW:;
      if (left->m_type  == kConveyor) loc->m_next = left;
      if (right->m_type == kConveyor) right->m_next = loc;
      break;
    case kConvDirN:;
  }

  struct Chunk_t* chunk = getChunk_fromLocation(loc);
  chunkAddLocation(chunk, loc);

  //pd->system->logToConsole("ADD TO CHUNK: %i %i with %i sprites", chunk->m_x, chunk->m_y, chunk->m_nLocations);

  // Bake into the background 
  renderChunkBackgroundImage(chunk);

  ++m_nConveyors;
  return true;
}

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type) {
  return false; // TODO

  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_cargo != NULL) return false;
  if (loc->m_type != kConveyor) return false;

  //pd->system->logToConsole("Set cargo, current %i", (int)loc->m_cargo);

  LCDBitmap* image = NULL;
  switch (_type) {
    case kApple: image = getSprite16(1,1); break;
    case kNoCargo:;
  }

  loc->m_cargo = pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TILE_PIX};
  pd->sprite->setBounds(loc->m_cargo, bound);
  pd->sprite->setImage(loc->m_cargo, image, kBitmapUnflipped);
  pd->sprite->moveTo(loc->m_cargo, loc->m_x, loc->m_y);
  pd->sprite->addSprite(loc->m_cargo);
  pd->sprite->setTag(loc->m_cargo, (uint8_t) _type);
  pd->sprite->setUpdatesEnabled(loc->m_cargo, 0);

  ++m_nCargo;
  return true;
}


uint32_t getNConveyors() {
  return m_nConveyors;
}

uint32_t getNCargo() {
  return m_nCargo;
}

void setRoobert11() {
  pd->graphics->setFont(m_fontRoobert11);
}

void initSprite() {
  m_sheet16 = loadImageTableAtPath("images/sheet16");

  //m_locations = calloc(TOT_TILES, sizeof(struct Location_t));
  m_locations = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Location_t));
  memset(m_locations, 0, TOT_TILES * sizeof(struct Location_t));

  m_chunks = pd->system->realloc(NULL, TOT_CHUNKS * sizeof(struct Chunk_t));
  memset(m_chunks, 0, TOT_CHUNKS * sizeof(struct Chunk_t));

  m_edgeChunks = pd->system->realloc(NULL, (WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4) * sizeof(struct Chunk_t));
  memset(m_edgeChunks, 0, (WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4) * sizeof(struct Chunk_t));

  for (int32_t i = 0; i < kConvDirN; ++i) {
    m_conveyorMasters[i] = getSprite16(0, CONV_START_Y + i);
  }

  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert11 = loadFontAtPath("fonts/Roobert-11-Bold");
  pd->graphics->setFont(m_fontRoobert24);

  m_player.m_sprite = pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TILE_PIX};
  pd->sprite->setBounds(m_player.m_sprite, bound);
  pd->sprite->setImage(m_player.m_sprite, getSprite16(0, 1), kBitmapUnflipped);
  pd->sprite->moveTo(m_player.m_sprite, SCREEN_PIX_X/2.0f, SCREEN_PIX_Y/2.0f);
  pd->sprite->getPosition(m_player.m_sprite, &(m_player.m_x), &(m_player.m_y));
  pd->sprite->addSprite(m_player.m_sprite);
  pd->sprite->setCollideRect(m_player.m_sprite, bound);



}

void deinitSprite() {
  pd->system->realloc(m_locations, 0);
  pd->system->realloc(m_chunks, 0);
  pd->system->realloc(m_edgeChunks, 0);
}
