#include "io.h"

uint8_t m_slot = 0;

/// ///

void setSlot(uint8_t _slot) { m_slot = _slot; }

uint8_t getSlot() { return m_slot; }


void startArray(struct json_encoder* encoder) {

}

void addArrayMember(struct json_encoder* encoder) {

}

void endArray(struct json_encoder* encoder) {

}

void startTable(struct json_encoder* encoder) {

}

void addTableMember(struct json_encoder* encoder, const char* name, int len) {

}

void endTable(struct json_encoder* encoder) {

}

void writeNull(struct json_encoder* encoder) {

}

void writeFalse(struct json_encoder* encoder) {

}

void writeTrue(struct json_encoder* encoder) {

}

void writeInt(struct json_encoder* encoder, int num) {

}

void writeDouble(struct json_encoder* encoder, double num) {

}

void writeString(struct json_encoder* encoder, const char* str, int len) {

}

void write(void* userdata, const char* str, int len) {
  pd->file->write((SDFile*)userdata, str, len);
}


bool save() {
  json_encoder je = {
    .startArray = startArray,
    .addArrayMember = addArrayMember,
    .endArray = endArray,
    .startTable = startTable,
    .addTableMember = addTableMember,
    .endTable = endTable,
    .writeNull = writeNull,
    .writeFalse = writeFalse,
    .writeTrue = writeTrue,
    .writeInt = writeInt,
    .writeDouble = writeDouble,
    .writeString = writeString
  };

  static char filePath[16];
  snprintf(filePath, 16, "slot%i.json", m_slot);
  SDFile* file = pd->file->open(filePath, kFileWrite);

  pd->json->initEncoder(&je, write, file, /*pretty=*/1);

  // ???

  int status = pd->file->close(file);

  pd->system->logToConsole("save to %u, status", m_slot, status);
  return true;

}

bool load() {
  return true;
}

