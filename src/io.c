#include "io.h"
#include "cargo.h"
#include "building.h"
#include "location.h"
#include "generate.h"
#include "player.h"
#include "buildings/special.h"

bool m_foundSaveData = false;

uint8_t m_slot = 0;

uint8_t m_scanSlot = 0;

char m_filePath[32];

uint8_t m_worldVersions[WORLD_SAVE_SLOTS];

bool m_worldExists[WORLD_SAVE_SLOTS];

int doRead(void* _userdata, uint8_t* _buf, int _bufsize);

void doWrite(void* _userdata, const char* _str, int _len);

void decodeError(json_decoder* jd, const char* _error, int _linenum);

void willDecodeSublist(json_decoder* jd, const char* _name, json_value_type _type);

void scanDidDecode(json_decoder* jd, const char* _key, json_value _value);

int scanShouldDecodeTableValueForKey(json_decoder* jd, const char* _key);

/// ///

bool hasSaveData() { return m_foundSaveData; }

bool hasWorld(uint8_t _slot) { return m_worldExists[_slot]; }

int doRead(void* _userdata, uint8_t* _buf, int _bufsize) {
  return pd->file->read((SDFile*)_userdata, _buf, _bufsize);
}

void doWrite(void* _userdata, const char* _str, int _len) {
  pd->file->write((SDFile*)_userdata, _str, _len);
}

void decodeError(json_decoder* jd, const char* _error, int _linenum) {
  pd->system->logToConsole("decode error line %i: %s", _linenum, _error);
}

void setSlot(uint8_t _slot) { m_slot = _slot; }

uint8_t getSlot() { return m_slot; }

///

void hardReset() {
  for (uint16_t ss = 0; ss < WORLD_SAVE_SLOTS; ++ss) {
    snprintf(m_filePath, 16, "world%u.json", (unsigned)ss);
    pd->file->unlink(m_filePath, 0);
  }
  pd->file->unlink("player.json", 0);
}

///

void scanSlots() {
  for (m_scanSlot = 0; m_scanSlot < WORLD_SAVE_SLOTS; ++m_scanSlot) {
    snprintf(m_filePath, 16, "world%u.json", (unsigned)m_scanSlot);
    SDFile* file = pd->file->open(m_filePath, kFileRead|kFileReadData);
    if (!file) {
      pd->system->logToConsole("Scan world: Slot:%u, No Save", m_scanSlot);
      m_worldExists[m_scanSlot] = false;
      continue;
    }

    json_decoder jd = {
      .decodeError = decodeError,
      .didDecodeTableValue = scanDidDecode,
      .shouldDecodeTableValueForKey = scanShouldDecodeTableValueForKey
    };

    pd->json->decode(&jd, (json_reader){ .read = doRead, .userdata = file }, NULL);
    pd->file->close(file);

    pd->system->logToConsole("Scan world: Slot:%u, Version:%i", (unsigned)m_scanSlot, m_worldVersions[m_scanSlot]);
    m_worldExists[m_scanSlot] = true;
    m_foundSaveData = true;
  }

  // Filter
  for (uint16_t ss = 0; ss < WORLD_SAVE_SLOTS; ++ss) {
    if (m_worldExists[ss]) {
      if (m_worldVersions[ss] == 0) {
        pd->system->logToConsole("Scan world: OLD WORLD DETECTED! Version 0. ACTION: Delete everything and start again");
        hardReset();
        m_foundSaveData = false;
      }
    }
  }

  {
    SDFile* file = pd->file->open("player.json", kFileRead|kFileReadData);
    if (!file) {
      m_foundSaveData = false;
      pd->system->logToConsole("Scan world: No Playe data found!");
    }
  }

  pd->system->logToConsole("Scan world: overall result:%s", m_foundSaveData ? "CAN-BE-LOADED" : "CANNOT-LOAD");
}



int scanShouldDecodeTableValueForKey(json_decoder* jd, const char* _key) {
  return (strcmp(_key, "sf") == 0);
}

void scanDidDecode(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "sf") == 0) {
    m_worldVersions[m_scanSlot] = json_intValue(_value);
  } else {
    pd->system->error("scanDidDecode DECODE ISSUE, %s", _key);
  }
}


///

bool save() {
  uint8_t pretty = 0;
  #ifdef DEV
  pretty = 1;
  #endif

  pd->system->logToConsole("START save player");

  // Should get the latest export averages, guaranteed to be between 60 and 120s worth of data
  updateExport();

  json_encoder je_p;

  snprintf(m_filePath, 32, "TMP_player.json");
  SDFile* file_p = pd->file->open(m_filePath, kFileWrite);

  pd->json->initEncoder(&je_p, doWrite, file_p, pretty);

  je_p.startTable(&je_p);
  serialisePlayer(&je_p);
  je_p.endTable(&je_p);

  int status = pd->file->close(file_p);

  /// ///

  pd->system->logToConsole("START save world");

  json_encoder je;

  snprintf(m_filePath, 32, "TMP_world%i.json", m_slot);
  SDFile* file = pd->file->open(m_filePath, kFileWrite);

  pd->json->initEncoder(&je, doWrite, file, pretty);

  je.startTable(&je);

  je.addTableMember(&je, "sf", 2);
  je.writeInt(&je, SAVE_FORMAT);

  serialiseCargo(&je);
  serialiseBuilding(&je);
  serialiseLocation(&je);
  serialiseWorld(&je);

  je.endTable(&je);

  status &= pd->file->close(file);

  //float f; for (int32_t i = 0; i < 10000; ++i) for (int32_t j = 0; j < 100000; ++j) { f*=i*j; }

  pd->system->logToConsole("save to %u, status %i", m_slot, status);

  // Finish by moving into location
  char filePathFinal[32];

  snprintf(m_filePath, 32, "TMP_player.json");
  snprintf(filePathFinal, 32, "player.json");
  pd->file->rename(m_filePath, filePathFinal);

  snprintf(m_filePath, 32, "TMP_world%i.json", m_slot);
  snprintf(filePathFinal, 32, "world%i.json", m_slot);
  pd->file->rename(m_filePath, filePathFinal);

  return true;
}

///

bool load(int32_t _forceSlot) {

  pd->system->logToConsole("START load player");

  json_decoder jd_p = {
    .decodeError = decodeError,
    .didDecodeTableValue = didDecodeTableValuePlayer,
    .didDecodeArrayValue = deserialiseArrayValuePlayer
  };

  snprintf(m_filePath, 16, "player.json");
  SDFile* file_p = pd->file->open(m_filePath, kFileRead|kFileReadData);

  pd->json->decode(&jd_p, (json_reader){ .read = doRead, .userdata = file_p }, NULL);

  pd->file->close(file_p);

  // We have now loaded the correct slot number for this player-save.
  // But we might want to be loading into a different world 
  if (_forceSlot != -1) {
    setSlot(_forceSlot);
  }

  pd->system->logToConsole("START load from slot %i", m_slot);

  json_decoder jd = {
    .decodeError = decodeError,
    .willDecodeSublist = willDecodeSublist
  };

  snprintf(m_filePath, 16, "world%i.json", m_slot);
  SDFile* file = pd->file->open(m_filePath, kFileRead|kFileReadData);

  pd->json->decode(&jd, (json_reader){ .read = doRead, .userdata = file }, NULL);

  pd->file->close(file);

  pd->system->logToConsole("STOP load from slot %i", m_slot);
  
  return true;
}

void willDecodeSublist(json_decoder* jd, const char* _name, json_value_type _type) {

  static char truncated[6];
  strncpy(truncated, _name, 5);
  truncated[5] = '\0';

  if (strcmp(truncated, "sf") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = NULL;
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
