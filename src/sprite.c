#include "sprite.h"
#include "generate.h"


//struct LCDSprite* m_world[WORLD_CHUNKS_X][WORLD_CHUNKS_Y];
//LCDSprite* m_worldEdges[WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4]; // + 4 is corners

struct Chunk_t* m_chunks;
struct Chunk_t* m_edgeChunks;

#define CONV_START_Y 7


LCDBitmap* m_conveyorMasters[kConvDirN] = {NULL};

uint16_t m_nConveyors = 0, m_nCargo = 0;

struct Location_t* m_locations = NULL;

struct Cargo_t* m_worldCargos;

LCDBitmapTable* m_sheet16;

struct Player_t m_player;

LCDFont* m_fontRoobert24;
LCDFont* m_fontRoobert10;

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

struct Cargo_t* cargoManagerNewCargo(void) {
  struct Cargo_t* theCargo = &(m_worldCargos[m_nCargo]);
  ++m_nCargo;
  return theCargo;
}

void cargoManagerFreeCargo(struct Cargo_t* _cargo) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TOT_TILES; ++i) {
    if (m_worldCargos[i].m_sprite == _cargo->m_sprite) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in freeCargo!");
  #endif
  for(uint32_t i = idx; i < TOT_TILES - 1; i++) {
    m_worldCargos[i] = m_worldCargos[i + 1];
  }
  --(m_nCargo);
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

  pd->graphics->setDrawMode(kDrawModeCopy);
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


#define min(a,b) (((a) < (b)) ? (a) : (b))

void conveyorUpdateFn(struct Location_t* _loc, uint8_t _tick) {
  if (_loc->m_cargo == NULL) return;
  if (_loc->m_progress < TILE_PIX) {
    _loc->m_progress = min(_loc->m_progress + _tick, TILE_PIX);
    switch (_loc->m_convDir) {
      case SN:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_x, _loc->m_y - _loc->m_progress); break;
      case NS:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_x, _loc->m_y + _loc->m_progress); break;
      case WE:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_x + _loc->m_progress, _loc->m_y); break;
      case EW:; pd->sprite->moveTo(_loc->m_cargo->m_sprite, _loc->m_x - _loc->m_progress, _loc->m_y); break;
      case kConvDirN:; break;
    }
  } else if (_loc->m_next != NULL && _loc->m_next->m_cargo == NULL) {
    struct Cargo_t* theCargo = _loc->m_cargo;
    _loc->m_next->m_cargo = theCargo;
    _loc->m_cargo = NULL;
    _loc->m_next->m_progress = 0;
    _loc->m_progress = 0;
    // Cargo moves between chunks?
    if (_loc->m_next->m_chunk != _loc->m_chunk) {
      chunkRemoveCargo(_loc->m_chunk, theCargo);
      chunkAddCargo(_loc->m_next->m_chunk, theCargo);
    }
  }
}

void chunkAddLocation(struct Chunk_t* _chunk, struct Location_t* _loc) {
  _chunk->m_locations[ _chunk->m_nLocations ] = _loc;
  ++(_chunk->m_nLocations);
}

void chunkRemoveLocation(struct Chunk_t* _chunk, struct Location_t* _loc) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_locations[i] == _loc) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveLocation!");
  #endif
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_locations[i] = _chunk->m_locations[i + 1];
  }
  --(_chunk->m_nLocations);
}

void chunkAddCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo) {
  _chunk->m_cargos[ _chunk->m_nCargos ] = _cargo;
  ++(_chunk->m_nCargos);
}

void chunkRemoveCargo(struct Chunk_t* _chunk, struct Cargo_t* _cargo) {
  int32_t idx = -1;
  for (uint32_t i = 0; i < TILES_PER_CHUNK; ++i) {
    if (_chunk->m_cargos[i] == _cargo) {
      idx = i;
      break;
    }
  }
  #ifdef DEV
  if (idx == -1) pd->system->error("-1 in chunkRemoveCargo!");
  #endif
  for(uint32_t i = idx; i < TILES_PER_CHUNK - 1; i++) {
    _chunk->m_cargos[i] = _chunk->m_cargos[i + 1];
  }
  --(_chunk->m_nCargos);
}


bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir) {
  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_type != kEmpty) return false;

  loc->m_x = _x;
  loc->m_y = _y;
  struct Chunk_t* chunk = getChunk_fromLocation(loc);

  loc->m_sprite = pd->sprite->newSprite();
  loc->m_image = getSprite16(0, CONV_START_Y + _dir);
  loc->m_cargo = NULL;
  loc->m_type = kConveyor;
  loc->m_chunk = chunk;
  loc->m_convDir = _dir;
  loc->m_updateFn = &conveyorUpdateFn;
  loc->m_progress = 0;

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

  chunkAddLocation(chunk, loc);

  //pd->system->logToConsole("ADD TO CHUNK: %i %i with %i sprites", chunk->m_x, chunk->m_y, chunk->m_nLocations);

  // Bake into the background 
  renderChunkBackgroundImage(chunk);
  updateRenderList();

  ++m_nConveyors;
  return true;
}

bool newCargo(uint32_t _x, uint32_t _y, enum kCargoType _type) {

  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_cargo != NULL) return false;
  if (loc->m_type != kConveyor) return false;

  //pd->system->logToConsole("Set cargo, current %i", (int)loc->m_cargo);

  LCDBitmap* image = NULL;
  switch (_type) {
    case kApple: image = getSprite16(1,1); break;
    case kNoCargo:;
  }

  struct Cargo_t* cargo = cargoManagerNewCargo();
  if (cargo->m_sprite == NULL) cargo->m_sprite = pd->sprite->newSprite();
  cargo->m_type = _type;

  PDRect bound = {.x = 0, .y = 0, .width = TILE_PIX, .height = TILE_PIX};
  pd->sprite->setBounds(cargo->m_sprite, bound);
  pd->sprite->setImage(cargo->m_sprite, image, kBitmapUnflipped);
  pd->sprite->moveTo(cargo->m_sprite, loc->m_x, loc->m_y);

  loc->m_cargo = cargo;

  updateRenderList();
  return true;
}


uint16_t getNConveyors() {
  return m_nConveyors;
}

uint16_t getNCargo() {
  return m_nCargo;
}

void setRoobert10() {
  pd->graphics->setFont(m_fontRoobert10);
}

void initSprite() {
  pd->graphics->setDrawMode(kDrawModeCopy);
  m_sheet16 = loadImageTableAtPath("images/sheet16");

  //m_locations = calloc(TOT_TILES, sizeof(struct Location_t));
  m_locations = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Location_t));
  memset(m_locations, 0, TOT_TILES * sizeof(struct Location_t));

  m_worldCargos = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Cargo_t));
  memset(m_worldCargos, 0, TOT_TILES * sizeof(struct Cargo_t));

  m_chunks = pd->system->realloc(NULL, TOT_CHUNKS * sizeof(struct Chunk_t));
  memset(m_chunks, 0, TOT_CHUNKS * sizeof(struct Chunk_t));

  m_edgeChunks = pd->system->realloc(NULL, (WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4) * sizeof(struct Chunk_t));
  memset(m_edgeChunks, 0, (WORLD_CHUNKS_X*2 + WORLD_CHUNKS_Y*2 + 4) * sizeof(struct Chunk_t));

  for (int32_t i = 0; i < kConvDirN; ++i) {
    m_conveyorMasters[i] = pd->graphics->copyBitmap(getSprite16(0, CONV_START_Y + i));
  }

  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert10 = loadFontAtPath("fonts/Roobert-10-Bold");
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
