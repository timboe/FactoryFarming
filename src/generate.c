#include "game.h"
#include "generate.h"
#include "sprite.h"


#define WORLD_SCREENS_X 4
#define WORLD_SCREENS_Y 4

#define FLOOR_TILES 8

LCDBitmap* m_world[WORLD_SCREENS_X][WORLD_SCREENS_Y];

void generate(PlaydateAPI* _pd) {
  srand(_pd->system->getSecondsSinceEpoch(NULL));

  uint8_t* tiles = malloc(WORLD_SCREENS_X * TILES_X * WORLD_SCREENS_Y * TILES_Y * sizeof(uint8_t));

  // Worldgen is very basic for now
  for (uint16_t i = 0; i < WORLD_SCREENS_X * TILES_X * WORLD_SCREENS_Y * TILES_Y; ++i) {
    tiles[i] = rand() % FLOOR_TILES;
  } 


  for (uint16_t y = 0; y < WORLD_SCREENS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_SCREENS_X; ++x) {
      m_world[x][y] = _pd->graphics->newBitmap(SCREEN_X, SCREEN_Y, kColorClear);
      _pd->graphics->pushContext(m_world[x][y]);
      for (uint16_t v = 0; v < TILES_Y; ++v) {
        for (uint16_t u = 0; u < TILES_X; ++u) {
          uint8_t t = tiles[ (WORLD_SCREENS_X * TILES_X)*((TILES_Y * y) + v) + ((TILES_X * x) + u) ];
          LCDBitmap* b = getSprite16(_pd, t, 0); // Top row
          _pd->graphics->drawBitmap(b, u * TILE_SIZE, v * TILE_SIZE, kBitmapUnflipped);
        }
      }
      #ifdef DEV
      _pd->graphics->drawRect(0, 0, SCREEN_X, SCREEN_Y, kColorBlack);
      static char text[16];
      snprintf(text, 16, "(%u,%u)", x, y);
      _pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_SIZE, TILE_SIZE);
      _pd->system->logToConsole("(%u,%u) %i", x, y, (int)m_world[x][y]);
      #endif
      _pd->graphics->popContext();
    }
  }

  free(tiles);
}

void get4Floor(LCDBitmap* floors[]) {
  floors[0] = m_world[0][0];
  floors[1] = m_world[1][0];
  floors[2] = m_world[0][1];
  floors[3] = m_world[1][1];
}