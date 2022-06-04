#include "io.h"
#include "cargo.h"
#include "building.h"
#include "location.h"
#include "generate.h"


uint8_t m_slot = 0;

/// ///

void setSlot(uint8_t _slot) { m_slot = _slot; }

uint8_t getSlot() { return m_slot; }


void doWrite(void* _userdata, const char* _str, int _len) {
  pd->file->write((SDFile*)_userdata, _str, _len);
}


bool save() {
  json_encoder je;

  static char filePath[16];
  snprintf(filePath, 16, "slot%i.json", m_slot);
  SDFile* file = pd->file->open(filePath, kFileWrite);

  uint8_t pretty = 0;
  #ifdef DEBUG_MODE
  pretty = 1;
  #endif

  pd->json->initEncoder(&je, doWrite, file, pretty);
  je.startTable(&je);

  serialiseCargo(&je);
  serialiseBuilding(&je);
  serialiseLocation(&je);
  serialiseWorld(&je);


  je.endTable(&je);


  int status = pd->file->close(file);

  pd->system->logToConsole("save to %u, status %i", m_slot, status);
  return true;

}

bool load() {

  
  return true;
}

