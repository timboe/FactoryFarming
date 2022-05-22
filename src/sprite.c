#include "sprite.h"


struct LCDSprite* m_world[WORLD_SCREENS_X][WORLD_SCREENS_Y];
LCDSprite* m_worldEdges[WORLD_SCREENS_X*2 + WORLD_SCREENS_Y*2 + 4]; // + 4 is corners

LCDBitmap* m_conveyorMasters[kConvDirN] = {NULL};

uint32_t m_nConveyors = 0, m_nCargo = 0;

struct Location_t* m_worldLocations = NULL;

LCDBitmapTable* m_sheet16;

LCDSprite* m_player;

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

void setWorld(LCDBitmap* _b, uint32_t _x, uint32_t _y) {
  m_world[_x][_y] = pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = SCREEN_X, .height = SCREEN_Y};
  pd->sprite->setBounds(m_world[_x][_y], bound);
  pd->sprite->setImage(m_world[_x][_y], _b, kBitmapUnflipped);
  pd->sprite->moveTo(m_world[_x][_y], SCREEN_X*_x + SCREEN_X/2.0f, SCREEN_Y*_y + SCREEN_Y/2.0f);
  pd->sprite->addSprite(m_world[_x][_y]);
  pd->sprite->setZIndex(m_world[_x][_y], -1);

}

void setWorldEdges() {
  uint32_t c = 0;
  PDRect bound = {.x = 0, .y = 0, .width = SCREEN_X, .height = SCREEN_Y};

  for (uint32_t x = 0; x < WORLD_SCREENS_X; ++x) {
    m_worldEdges[c] = pd->sprite->newSprite(); // Top
    pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[x][WORLD_SCREENS_Y-1]), kBitmapUnflipped);
    pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*x + SCREEN_X/2.0f, SCREEN_Y*-1 + SCREEN_Y/2.0f);
    pd->sprite->addSprite(m_worldEdges[c]);
    pd->sprite->setZIndex(m_worldEdges[c], -1);
    ++c;
    m_worldEdges[c] = pd->sprite->newSprite(); // Bottom
    pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[x][0]), kBitmapUnflipped);
    pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*x + SCREEN_X/2.0f, SCREEN_Y*WORLD_SCREENS_Y + SCREEN_Y/2.0f);
    pd->sprite->addSprite(m_worldEdges[c]);
    pd->sprite->setZIndex(m_worldEdges[c], -1);
    ++c;
  }

  for (uint32_t y = 0; y < WORLD_SCREENS_Y; ++y) {
    m_worldEdges[c] = pd->sprite->newSprite(); // Left
    pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[WORLD_SCREENS_X-1][y]), kBitmapUnflipped);
    pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*-1 + SCREEN_X/2.0f, SCREEN_Y*y + SCREEN_Y/2.0f);
    pd->sprite->addSprite(m_worldEdges[c]);
    pd->sprite->setZIndex(m_worldEdges[c], -1);
    ++c;
    m_worldEdges[c] = pd->sprite->newSprite(); // Bottom
    pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[0][y]), kBitmapUnflipped);
    pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*WORLD_SCREENS_X + SCREEN_X/2.0f, SCREEN_Y*y + SCREEN_Y/2.0f);
    pd->sprite->addSprite(m_worldEdges[c]);
    pd->sprite->setZIndex(m_worldEdges[c], -1);
    ++c;
  }

  m_worldEdges[c] = pd->sprite->newSprite(); // Top Left
  pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[WORLD_SCREENS_X-1][WORLD_SCREENS_Y-1]), kBitmapUnflipped);
  pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*-1 + SCREEN_X/2.0f, SCREEN_Y*-1 + SCREEN_Y/2.0f);
  pd->sprite->addSprite(m_worldEdges[c]);
  pd->sprite->setZIndex(m_worldEdges[c], -1);
  ++c;

  m_worldEdges[c] = pd->sprite->newSprite(); // Top Right
  pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[0][WORLD_SCREENS_Y-1]), kBitmapUnflipped);
  pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*WORLD_SCREENS_X + SCREEN_X/2.0f, SCREEN_Y*-1 + SCREEN_Y/2.0f);
  pd->sprite->addSprite(m_worldEdges[c]);
  pd->sprite->setZIndex(m_worldEdges[c], -1);
  ++c;

  m_worldEdges[c] = pd->sprite->newSprite(); // Bottom Right
  pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[0][0]), kBitmapUnflipped);
  pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*WORLD_SCREENS_X + SCREEN_X/2.0f, SCREEN_Y*WORLD_SCREENS_Y + SCREEN_Y/2.0f);
  pd->sprite->addSprite(m_worldEdges[c]);
  pd->sprite->setZIndex(m_worldEdges[c], -1);
  ++c;

  m_worldEdges[c] = pd->sprite->newSprite(); // Bottom Left
  pd->sprite->setImage(m_worldEdges[c], pd->sprite->getImage(m_world[WORLD_SCREENS_X-1][0]), kBitmapUnflipped);
  pd->sprite->moveTo(m_worldEdges[c], SCREEN_X*-1 + SCREEN_X/2.0f, SCREEN_Y*WORLD_SCREENS_Y + SCREEN_Y/2.0f);
  pd->sprite->addSprite(m_worldEdges[c]);
  pd->sprite->setZIndex(m_worldEdges[c], -1);
  ++c;

}


LCDBitmap* getSprite16(uint32_t _x, uint32_t _y) {
  return pd->graphics->getTableBitmap(m_sheet16, (SHEET16_SIZE * _y) + _x);
}
 
LCDSprite* getPlayer() {
  return m_player;
}

void animateConveyor() {
  int8_t tick = getFrameCount() % 8;

  
  pd->graphics->pushContext(m_conveyorMasters[SN]);
  pd->graphics->drawBitmap(getSprite16(tick, 7), 0, 0, kBitmapUnflipped);
  pd->graphics->popContext();

  pd->graphics->pushContext(m_conveyorMasters[NS]);
  pd->graphics->drawBitmap(getSprite16(tick, 8), 0, 0, kBitmapUnflipped);
  pd->graphics->popContext();


  //for (uint32_t i = 0; i < m_nConveyors; ++i) {
  //  pd->sprite->setImage(m_conveyors[i], getSprite16(tick, 7), kBitmapUnflipped);
  //}

}

struct Location_t* getLocation(int32_t _x, int32_t _y) {
  if (_x < 0) _x += TOT_TILES_X;
  else if (_x >= TOT_TILES_X) _x -= TOT_TILES_X;
  if (_y < 0) _y += TOT_TILES_Y;
  else if (_y >= TOT_TILES_Y) _y -= TOT_TILES_Y;
  const uint32_t l = (TOT_TILES_X * _y) + _x;
  return &m_worldLocations[l];
}


void conveyorUpdateFnSN(LCDSprite* conveyor) {
  struct Location_t* loc = (struct Location_t*) pd->sprite->getUserdata(conveyor);
  if (loc->m_cargo == NULL) return;
  if (loc->m_progress < TILE_SIZE) {
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
  if (loc->m_progress < TILE_SIZE) {
    ++(loc->m_progress);
    pd->sprite->moveTo(loc->m_cargo, loc->m_x, loc->m_y + loc->m_progress);
  } else if (loc->m_next != NULL && loc->m_next->m_cargo == NULL) {
    loc->m_next->m_cargo = loc->m_cargo;
    loc->m_cargo = NULL;
    loc->m_next->m_progress = 0;
    loc->m_progress = 0;
  }
}

bool newConveyor(uint32_t _x, uint32_t _y, enum kConvDir _dir) {
  struct Location_t* loc = getLocation(_x, _y);
  if (loc->m_type != kEmpty) return false;

  loc->m_sprite = pd->sprite->newSprite();
  loc->m_cargo = NULL;
  loc->m_type = kConveyor;
  loc->m_progress = 0;
  loc->m_x = TILE_SIZE*_x + TILE_SIZE/2.0;
  loc->m_y = TILE_SIZE*_y + TILE_SIZE/2.0;
  PDRect bound = {.x = 0, .y = 0, .width = TILE_SIZE, .height = TILE_SIZE};
  pd->sprite->setBounds(loc->m_sprite, bound);
  pd->sprite->setImage(loc->m_sprite, m_conveyorMasters[_dir], kBitmapUnflipped);
  pd->sprite->moveTo(loc->m_sprite, loc->m_x, loc->m_y);
  pd->sprite->addSprite(loc->m_sprite);
  pd->sprite->setUserdata(loc->m_sprite, (void*) loc);
  pd->sprite->setUpdatesEnabled(loc->m_sprite, 1);

  struct Location_t* above;
  struct Location_t* below;
  struct Location_t* left;
  struct Location_t* right;
  switch (_dir) {
    case SN:;
      above = getLocation(_x, _y - 1);
      below = getLocation(_x, _y + 1);
      if (above->m_type == kConveyor) loc->m_next = above;
      if (below->m_type == kConveyor) below->m_next = loc;
      pd->sprite->setUpdateFunction(loc->m_sprite, conveyorUpdateFnSN);
      break;
    case NS:;
      above = getLocation(_x, _y - 1);
      below = getLocation(_x, _y + 1);
      if (above->m_type == kConveyor) above->m_next = loc;
      if (below->m_type == kConveyor) loc->m_next = below;
      pd->sprite->setUpdateFunction(loc->m_sprite, conveyorUpdateFnNS);
      break;
  }

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
  }

  loc->m_cargo = pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = TILE_SIZE, .height = TILE_SIZE};
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

  m_conveyorMasters[SN] = getSprite16(0, 7);
  m_conveyorMasters[NS] = getSprite16(0, 8);


  m_fontRoobert24 = loadFontAtPath("fonts/Roobert-24-Medium");
  m_fontRoobert11 = loadFontAtPath("fonts/Roobert-11-Bold");
  pd->graphics->setFont(m_fontRoobert24);

  m_player = pd->sprite->newSprite();
  PDRect bound = {.x = 0, .y = 0, .width = TILE_SIZE, .height = TILE_SIZE};
  pd->sprite->setBounds(m_player, bound);
  pd->sprite->setImage(m_player, getSprite16(0, 1), kBitmapUnflipped);
  pd->sprite->moveTo(m_player, SCREEN_X/2.0f, SCREEN_Y/2.0f);
  pd->sprite->addSprite(m_player);
  pd->sprite->setCollideRect(m_player, bound);

  //m_worldLocations = calloc(TOT_TILES, sizeof(struct Location_t));
  m_worldLocations = pd->system->realloc(NULL, TOT_TILES * sizeof(struct Location_t));
  memset(m_worldLocations, 0, TOT_TILES * sizeof(struct Location_t));
}

void deinitSprite() {
  pd->system->realloc(m_worldLocations, 0);
}
