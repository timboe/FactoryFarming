#include "game.h"
#include "generate.h"
#include "sprite.h"




void generate() {
  srand(pd->system->getSecondsSinceEpoch(NULL));

  uint8_t* tiles = malloc(WORLD_SCREENS_X * TILES_X * WORLD_SCREENS_Y * TILES_Y * sizeof(uint8_t));

  // Worldgen is very basic for now
  for (uint16_t i = 0; i < WORLD_SCREENS_X * TILES_X * WORLD_SCREENS_Y * TILES_Y; ++i) {
    tiles[i] = rand() % FLOOR_TILES;
  } 


  for (uint16_t y = 0; y < WORLD_SCREENS_Y; ++y) {
    for (uint16_t x = 0; x < WORLD_SCREENS_X; ++x) {
      LCDBitmap* bg = pd->graphics->newBitmap(SCREEN_X, SCREEN_Y, kColorClear);
      pd->graphics->pushContext(bg);
      for (uint16_t v = 0; v < TILES_Y; ++v) {
        for (uint16_t u = 0; u < TILES_X; ++u) {
          uint8_t t = tiles[ (WORLD_SCREENS_X * TILES_X)*((TILES_Y * y) + v) + ((TILES_X * x) + u) ];
          LCDBitmap* b = getSprite16(t, 0); // Top row
          pd->graphics->drawBitmap(b, u * TILE_SIZE, v * TILE_SIZE, kBitmapUnflipped);
        }
      }
      #ifdef DEV
      pd->graphics->drawRect(0, 0, SCREEN_X, SCREEN_Y, kColorBlack);
      static char text[16];
      snprintf(text, 16, "(%u,%u)", x, y);
      pd->graphics->drawText(text, 16, kASCIIEncoding, TILE_SIZE, TILE_SIZE);
      #endif
      pd->graphics->popContext();
      setWorld(bg, x, y);
    }
  }

  setWorldEdges();

  free(tiles);
}
