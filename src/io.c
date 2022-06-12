#include "io.h"
#include "cargo.h"
#include "building.h"
#include "location.h"
#include "generate.h"
#include "player.h"

uint8_t m_slot = 0;

/// ///

void setSlot(uint8_t _slot) { m_slot = _slot; }

uint8_t getSlot() { return m_slot; }

void doWrite(void* _userdata, const char* _str, int _len) {
  pd->file->write((SDFile*)_userdata, _str, _len);
}

void decodeError(json_decoder* jd, const char* _error, int _linenum) {
  pd->system->logToConsole("decode error line %i: %s", _linenum, _error);
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

  serialisePlayer(&je);
  serialiseCargo(&je);
  serialiseBuilding(&je);
  serialiseLocation(&je);
  serialiseWorld(&je);

  je.endTable(&je);

  int status = pd->file->close(file);

  pd->system->logToConsole("save to %u, status %i", m_slot, status);
  return true;

}

void willDecodeSublist(json_decoder* jd, const char* _name, json_value_type _type) {

  static char truncated[6];
  strncpy(truncated, _name, 5);
  truncated[5] = '\0';

  if (strcmp(_name, "player") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = didDecodeTableValuePlayer;
    jd->didDecodeSublist = deserialiseStructDonePlayer;
  } else if (strcmp(truncated, "cargo") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueCargo;
    jd->didDecodeSublist = deserialiseStructDoneCargo;
  } else if (strcmp(truncated, "build") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueBuilding;
    jd->didDecodeSublist = deserialiseStructDoneBuilding;
  } else if (strcmp(truncated, "locat") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueLocation;
    jd->didDecodeSublist = deserialiseStructDoneLocation;
  } else if (strcmp(truncated, "world") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueWorld;
    jd->didDecodeSublist = deserialiseStructDoneWorld;
  } else {
    jd->didDecodeTableValue = NULL;
  }
  
}

int doRead(void* _userdata, uint8_t* _buf, int _bufsize) {
  return pd->file->read((SDFile*)_userdata, _buf, _bufsize);
}


bool load() {

  pd->system->logToConsole("START load from slot %i", m_slot);

  json_decoder jd = {
    .decodeError = decodeError,
    .willDecodeSublist = willDecodeSublist
  };

  static char filePath[16];
  snprintf(filePath, 16, "slot%i.json", m_slot);
  SDFile* file = pd->file->open(filePath, kFileRead|kFileReadData);

  pd->json->decode(&jd, (json_reader){ .read = doRead, .userdata = file }, NULL);

  pd->file->close(file);

  pd->system->logToConsole("STOP from slot %i", m_slot);
  
  return true;
}
