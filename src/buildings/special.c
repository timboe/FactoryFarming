#include "special.h"
#include "../location.h"
#include "../sprite.h"
#include "../generate.h"
#include "../cargo.h"
#include "../player.h"
#include "../io.h"

struct Building_t* m_sellBox = NULL;

struct Building_t* m_buyBox = NULL;

struct Building_t* m_warp = NULL;

struct Building_t* m_exportBox = NULL;

struct Building_t* m_importBox = NULL;

int32_t m_exportTimer = 0;
bool m_exportParity = false;

uint16_t m_exportItemCountA[kNCargoType];
uint16_t m_exportItemCountB[kNCargoType];

uint16_t m_exportItemValueA = 0;
uint16_t m_exportItemValueB = 0;

bool m_hasExported = false, m_hasImported = false; // tutorial only

#define ONE_MIN (60*TICKS_PER_SEC)
#define TWO_MIN (120*TICKS_PER_SEC)
#define IMPORT_SECONDS 10
#define IMPORT_SEC_IN_TICKS (IMPORT_SECONDS*TICKS_PER_SEC)

void sellBoxUpdateFn(struct Building_t* _building, uint8_t _tick);

void exportUpdateFn(struct Building_t* _building);

void importUpdateFn(struct Building_t* _building, uint8_t _tick);

bool nearbyConveyor(struct Building_t* _building);

/// ///

struct Building_t* getImportBox(void) {
  return m_importBox;
}

float directionToBuy() {
  //TODO - make this take into account the torus
  if (!m_buyBox) return 1000;
  struct Player_t* p = getPlayer();
  const int16_t dx1 = p->m_pix_x - m_buyBox->m_pix_x;
  const int16_t dy1 = p->m_pix_y - m_buyBox->m_pix_y;
  const int16_t dx2 = p->m_pix_x - m_buyBox->m_pix_x - TOT_WORLD_PIX_X;
  const int16_t dy2 = p->m_pix_y - m_buyBox->m_pix_y - TOT_WORLD_PIX_Y;
  const int16_t dx = fabs(dx1) < fabs(dx2) ? dx1 : dx2;
  const int16_t dy = fabs(dy1) < fabs(dy2) ? dy1 : dy2;
  return atan2(dx, dy);
}

int16_t distanceFromBuy() {
  if (!m_buyBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_buyBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_buyBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromSell() {
  if (!m_buyBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_sellBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_sellBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromWarp() {
  if (!m_warp) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_warp->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_warp->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromOut() {
  if (!m_exportBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_exportBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_exportBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

int16_t distanceFromIn() {
  if (!m_importBox) return 1000;
  struct Player_t* p = getPlayer();
  uint16_t dx = abs(p->m_pix_x - m_importBox->m_pix_x);
  uint16_t dy = abs(p->m_pix_y - m_importBox->m_pix_y);
  return (dx < dy ? dy : dx);
}

bool nearbyConveyor(struct Building_t* _building) {
  for (int32_t x = -2; x < 3; ++x) {
    for (int32_t y = -2; y < 3; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_building && loc->m_building->m_type == kConveyor) {
        return true;
      }
    }
  }
  return false;
}

void sellBoxUpdateFn(struct Building_t* _building, uint8_t _tick) {
  for (int32_t x = _building->m_location->m_x - 1; x < _building->m_location->m_x + 2; ++x) {
    for (int32_t y = _building->m_location->m_y - 1; y < _building->m_location->m_y + 2; ++y) {
      struct Location_t* loc = getLocation_noCheck(x, y); // Will never straddle the world boundary
      if (loc->m_cargo) {
        getPlayer()->m_soldCargo[ loc->m_cargo->m_type ]++;
        const uint16_t price = CargoDesc[loc->m_cargo->m_type].price;
        modMoney( price );

        if (m_exportTimer < ONE_MIN) {
          m_exportItemValueA += price;
        } else {
          if (m_exportParity) m_exportItemValueA += price;
          else                m_exportItemValueB += price;
        }

        // Tutorial
        const enum kUITutorialStage tut = getTutorialStage(); 
        if (tut == kTutBuildConveyor && loc->m_cargo->m_type == kCarrot && nearbyConveyor(_building)) {
          makeTutorialProgress();
        }
        // Tutorial 
        if (tut == kTutBuildVitamin && loc->m_cargo->m_type == kVitamin && nearbyConveyor(_building)) {
          makeTutorialProgress();
        }
        clearLocation(loc, /*cargo*/ true, /*building*/ false);
      }
    }
  }

  // Process external sales
  _building->m_progress += _tick;
  if (_building->m_progress >= IMPORT_SEC_IN_TICKS) {
    _building->m_progress -= IMPORT_SEC_IN_TICKS;
    const float totInputSales = getOtherWorldCargoSales() * IMPORT_SECONDS;
    modMoney( totInputSales ); // Rounds down
  } 


  // Process exporting of sales data (and exports data, though m_exportItemCountA/B are only incremented by the Exports update fn)
  m_exportTimer += _tick;
  if (m_exportTimer < TWO_MIN) return;

  updateExport();
  updateSales();
  m_exportTimer -= ONE_MIN;
  m_exportParity = !m_exportParity;
  if (m_exportParity)  {
    m_exportItemValueA = 0;
    memset(m_exportItemCountA, 0, sizeof(uint16_t)*kNCargoType);
  } else {
    m_exportItemValueB = 0;
    memset(m_exportItemCountB, 0, sizeof(uint16_t)*kNCargoType);
  }
}

void specialUpdateFn(struct Building_t* _building, uint8_t _tick, uint8_t _zoom) {
  switch (_building->m_subType.special) {
    case kSellBox:; return sellBoxUpdateFn(_building, _tick);
    case kExportBox:; return exportUpdateFn(_building);
    case kImportBox:; return importUpdateFn(_building, _tick);
    default: return;
  }
}

void assignNeighborsSpecial(struct Building_t* _building) {
}

bool isCamouflaged() {
  return m_warp && m_warp->m_dir != SN;
}

void unlockOtherWorlds() {
  m_warp->m_dir = SN;
  m_exportBox->m_dir = SN;
  m_importBox->m_dir = SN;

  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    m_warp->m_image[zoom] = getSprite48(0, 2, zoom);
    m_exportBox->m_image[zoom] = getSprite48(2, 1, zoom);
    m_importBox->m_image[zoom] = getSprite48(3, 1, zoom);
  }

  renderChunkBackgroundImageAround(m_warp->m_location->m_chunk);
  renderChunkBackgroundImageAround(m_exportBox->m_location->m_chunk);
  renderChunkBackgroundImageAround(m_importBox->m_location->m_chunk);

  startPlotsTutorial();
}

void buildingSetupSpecial(struct Building_t* _building) {
  for (uint32_t zoom = 1; zoom < ZOOM_LEVELS; ++zoom) {
    switch (_building->m_subType.special) {
      case kShop:;      _building->m_image[zoom] = getSprite48(1, 1, zoom); m_buyBox = _building; break;
      case kSellBox:;   _building->m_image[zoom] = getSprite48(0, 1, zoom); m_sellBox = _building; break;
      case kExportBox:; _building->m_image[zoom] = getSprite48(2, 1, zoom); m_exportBox = _building; break;
      case kImportBox:; _building->m_image[zoom] = getSprite48(3, 1, zoom); m_importBox = _building; break;
      case kWarp:;      _building->m_image[zoom] = getSprite48(0, 2, zoom); m_warp = _building; break;
      case kNSpecialSubTypes:;
    }

    // Start of the game?
    if (_building->m_dir != SN) {
      switch (_building->m_subType.special) {
        case kExportBox:; _building->m_image[zoom] = getSprite48(3, 2, zoom); break;
        case kImportBox:; _building->m_image[zoom] = getSprite48(2, 2, zoom); break;
        case kWarp:;      _building->m_image[zoom] = getSprite48(2, 2, zoom); break;
        default: break;
      }
    }

    PDRect bound = {.x = (COLLISION_OFFSET_BIG/2)*zoom, .y = (COLLISION_OFFSET_BIG/2)*zoom, .width = (EXTRACTOR_PIX-COLLISION_OFFSET_BIG)*zoom, .height = (EXTRACTOR_PIX-COLLISION_OFFSET_BIG)*zoom};
    if (_building->m_sprite[zoom] == NULL) _building->m_sprite[zoom] = pd->sprite->newSprite();
    pd->sprite->setCollideRect(_building->m_sprite[zoom], bound);
    pd->sprite->moveTo(_building->m_sprite[zoom], 
      (_building->m_pix_x + _building->m_location->m_pix_off_x - EXTRACTOR_PIX/2)*zoom, 
      (_building->m_pix_y + _building->m_location->m_pix_off_y - EXTRACTOR_PIX/2)*zoom);
  }

  for (int32_t x = _building->m_location->m_x - 1; x < _building->m_location->m_x + 2; ++x) {
    for (int32_t y = _building->m_location->m_y - 1; y < _building->m_location->m_y + 2; ++y) {
      clearLocation(getLocation(x, y), /*cargo*/ true, /*building*/ false);
    }
  }
}

bool hasExported() {
  return m_hasExported; // Tutorial
}

void exportUpdateFn(struct Building_t* _building) {
  if (_building->m_dir != SN) return;

  // Pickup items 
  const int32_t min = -1, max =2;
  // Picking up
  for (int32_t x = min; x < max; ++x) {
    for (int32_t y = min; y < max; ++y) {
      struct Location_t* loc = getLocation(_building->m_location->m_x + x, _building->m_location->m_y + y);
      if (loc->m_cargo) {
        // We do not allow export of any imports
        // Otherwise you can break the game too easily....
        const enum kCargoType ct = loc->m_cargo->m_type;
        if (ct == m_importBox->m_stored[3] || ct == m_importBox->m_stored[4] || ct == m_importBox->m_stored[5] || ct == m_importBox->m_mode.mode8[1]) {
          continue;
        }
        if (m_exportTimer < ONE_MIN) {
          m_exportItemCountA[loc->m_cargo->m_type]++;
        } else {
          if (m_exportParity) m_exportItemCountA[loc->m_cargo->m_type]++;
          else                m_exportItemCountB[loc->m_cargo->m_type]++;
        }
        clearLocation(loc, /*clearCargo*/ true, /*clearBuilding*/ false);
        m_hasExported = true;
      }
    }
  }
}

bool hasImported() {
  return m_hasImported; // Tutorial
}

void importUpdateFn(struct Building_t* _building, uint8_t _tick) {
  if (_building->m_dir != SN) return;

  struct Location_t* loc = NULL;

  // NORTH
  if (_building->m_stored[0]) {
    for (int32_t s = -1; s < 2; ++s) {
      loc = getLocation(_building->m_location->m_x + s, _building->m_location->m_y - 2);
      if (_building->m_stored[0] && loc->m_cargo == NULL) {
        newCargo(loc, _building->m_stored[3], _tick == NEAR_TICK_AMOUNT);
        --_building->m_stored[0];
      }
    }
  }

  // EAST
  if (_building->m_stored[1]) {
    for (int32_t s = -1; s < 2; ++s) {
      loc = getLocation(_building->m_location->m_x + 2, _building->m_location->m_y + s);
      if (_building->m_stored[1] && loc->m_cargo == NULL) {
        newCargo(loc, _building->m_stored[4], _tick == NEAR_TICK_AMOUNT);
        --_building->m_stored[1];
      }
    }
  }

  // SOUTH
  if (_building->m_stored[2]) {
    for (int32_t s = -1; s < 2; ++s) {
      loc = getLocation(_building->m_location->m_x + s, _building->m_location->m_y + 2);
      if (_building->m_stored[2] && loc->m_cargo == NULL) {
        newCargo(loc, _building->m_stored[5], _tick == NEAR_TICK_AMOUNT);
        --_building->m_stored[2];
      }
    }
  }

  // WEST - NOTE: Uses the m_mode slot as we are out of space in m_stored 
  if (_building->m_mode.mode8[0]) {
    for (int32_t s = -1; s < 2; ++s) {
      loc = getLocation(_building->m_location->m_x - 2, _building->m_location->m_y + s);
      if (_building->m_mode.mode8[0] && loc->m_cargo == NULL) {
        newCargo(loc, _building->m_mode.mode8[1], _tick == NEAR_TICK_AMOUNT);
        --_building->m_mode.mode8[0];
      }
    }
  }

  // Add new items
  _building->m_progress += _tick;
  if (_building->m_progress < IMPORT_SEC_IN_TICKS) return;
  _building->m_progress -= IMPORT_SEC_IN_TICKS;

  static float fractional[kNCargoType] = {0.0f}; // Keep track of partial cargos

  for (int32_t d = 0; d < 4; ++d) {

    enum kCargoType c = (d == 3 ? _building->m_mode.mode8[1] : _building->m_stored[(MAX_STORE/2) + d]);

    if (c == kNoCargo) {
      continue;
    }

    float totInput = getTotalCargoExport(c) * IMPORT_SECONDS;

    float input = totInput / getCargoImportConsumers(c);
    fractional[c] += input - (uint32_t)input;
    uint32_t input_int = (int32_t)input;

    getPlayer()->m_importedCargo[ c ]++;

    if (input_int) {
      m_hasImported = true;
      if (input_int + (d == 3 ? _building->m_mode.mode8[0] : _building->m_stored[d]) <= 255) {
        switch (d) {
          case 0: _building->m_stored[0] += input_int; break;
          case 1: _building->m_stored[1] += input_int; break;
          case 2: _building->m_stored[2] += input_int; break;
          case 3: _building->m_mode.mode8[0] += input_int; break;
        }
      } else {
        switch (d) {
          case 0: _building->m_stored[0] = 255; break;
          case 1: _building->m_stored[1] = 255; break;
          case 2: _building->m_stored[2] = 255; break;
          case 3: _building->m_mode.mode8[0] = 255; break;
        }
      }
    }

    // Accumulated an extra one?
    if (fractional[c] > 1.0f && (d == 3 ? _building->m_mode.mode8[0] : _building->m_stored[d]) < 255) {
      fractional[c] -= 1.0f;
      switch (d) {
        case 0: _building->m_stored[0]++; break;
        case 1: _building->m_stored[1]++; break;
        case 2: _building->m_stored[2]++; break;
        case 3: _building->m_mode.mode8[0]++; break;
      }
    }
  }

}

void updateSales() {
  struct Player_t* p = getPlayer();
  const uint8_t slot = getSlot();
 
  float collected = m_exportItemValueA + m_exportItemValueB;
  float av = collected / (m_exportTimer / TICKS_PER_SEC);
  p->m_sellPerWorld[slot] = av;
  //#ifdef DEV
  if (collected) pd->system->logToConsole("Integrated over %i s, the av sold value is %f /s", (m_exportTimer / TICKS_PER_SEC), (double)av);
  //#endif

}

void updateExport() {
  struct Player_t* p = getPlayer();
  const uint8_t slot = getSlot();

  for (int32_t c = 1; c < kNCargoType; ++c) {
    float collected = m_exportItemCountA[c] + m_exportItemCountB[c];
    float av = collected / (m_exportTimer / TICKS_PER_SEC);
    p->m_exportPerWorld[slot][c] = av;
    #ifdef DEV
    if (collected) pd->system->logToConsole("Integrated over %i s, the av exported %s is %f /s", (m_exportTimer / TICKS_PER_SEC), toStringCargoByType(c), (double)av);
    #endif
  }
}

void resetExportAndSales() {
  m_exportTimer = 0;
  m_exportParity = false;
  for (int32_t i = 0; i < kNCargoType; ++i) {
    m_exportItemCountA[i] = 0;
    m_exportItemCountB[i] = 0; 
  }
  m_exportItemValueA = 0;
  m_exportItemValueB = 0;
}

void drawUIInspectSpecial(struct Building_t* _building) {
  const enum kSpecialSubType sst = _building->m_subType.special;

  static char text[128];
  uint8_t y = 1;
  snprintf(text, 128, "%s", toStringBuilding(_building->m_type, _building->m_subType, false));
  pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*3, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  if (sst == kShop) {

    snprintf(text, 128, "Spend money to buy new Crops, Conveyors,");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Extractors, Factories, and Utility items.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (sst == kSellBox) {

    snprintf(text, 128, "Sell Cargo from your inventory, or use");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Conveyors to automate the sales.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Receive money for every sold item.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (sst == kWarp && _building->m_dir == SN) {

    snprintf(text, 128, "View available plots of land.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Buy new plots to expand your factory.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Move between your owned plots.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    
  } else if (sst == kExportBox && _building->m_dir == SN) {

    snprintf(text, 128, "Export Cargo for use in other plots.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Exports are averaged over 60s - 120s.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "You cannot Export any Cargo which is also being");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Imported into this plot.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else if (sst == kImportBox && _building->m_dir == SN) {

    snprintf(text, 128, "Import Cargo from other plots. You can select an");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "import for each of the 4 cardinal directions.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Imports occur every 10s, the amount imported");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "depends on the total number of importers.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  } else {

    snprintf(text, 128, "A large boulder blocks the ground here.");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, "Maybe it will become important later on?");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, " ");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);
    snprintf(text, 128, " ");
    pd->graphics->drawText(text, 128, kASCIIEncoding, TILE_PIX*2, TUT_Y_SPACING*(++y) - TUT_Y_SHFT);

  }

}