#include "io.h"
#include "ui.h"
#include "cargo.h"
#include "render.h"
#include "building.h"
#include "input.h"
#include "location.h"
#include "generate.h"
#include "player.h"
#include "sound.h"
#include "buildings/special.h"

uint8_t m_save = 0;

uint8_t m_slot = 0;

uint8_t m_scanSlot = 0;

char m_filePath[32];

uint8_t m_worldVersions[N_SAVES][WORLD_SAVE_SLOTS] = {0};

bool m_worldExists[N_SAVES][WORLD_SAVE_SLOTS] = {false};

bool m_foundSaveData[N_SAVES] = {false};

enum kSaveLoadRequest m_doFirst = kDoNothing, m_andThen = kDoNothing;

bool m_secondStage = false;

int8_t m_forceSlot = -1;

uint32_t m_actionProgress = 0;

SDFile* m_file;

int doRead(void* _userdata, uint8_t* _buf, int _bufsize);

void doWrite(void* _userdata, const char* _str, int _len);

bool doSave(bool _synchronous);

bool doLoad(void);

bool doNewWorld(void);

bool doSaveDelete(void);

bool doTitle(void);

void decodeError(json_decoder* jd, const char* _error, int _linenum);

void willDecodeSublist(json_decoder* jd, const char* _name, json_value_type _type);

void scanDidDecode(json_decoder* jd, const char* _key, json_value _value);

int scanShouldDecodeTableValueForKey(json_decoder* jd, const char* _key);

json_encoder m_je;

json_decoder m_jd_p = {
  .decodeError = decodeError,
  .didDecodeTableValue = didDecodeTableValuePlayer,
  .didDecodeArrayValue = deserialiseArrayValuePlayer
};

json_decoder m_jd = {
  .decodeError = decodeError,
  .willDecodeSublist = willDecodeSublist
};


/// ///

void doIO(enum kSaveLoadRequest _first, enum kSaveLoadRequest _andThen) {
  m_doFirst = _first;
  m_andThen = _andThen;
  m_secondStage = false;
  m_actionProgress = 0;
  pauseMusic();
  #ifdef DEV
  pd->system->logToConsole("IO: Requested");
  #endif
}

bool IOOperationInProgress() {
  return m_doFirst != kDoNothing;
}

enum kSaveLoadRequest currentIOAction() {
  return (m_secondStage ? m_andThen : m_doFirst);
}

void enactIO() {

  bool finished = false;
  switch (currentIOAction()) {
    case kDoNothing: finished = true; break;
    case kDoSave: finished = doSave(/*synchronous = */ false); break;
    case kDoLoad: finished = doLoad(); break;
    case kDoNewWorld: finished = doNewWorld(); break;
    case kDoTitle: finished = doTitle(); break;
    case kDoSaveDelete: finished = doSaveDelete(); break;
    case kDoScanSlots: finished = true; scanSlots(); break;
    case kDoResetPlayer: finished = true; resetPlayer(); break;
  }

  if (finished) {
    if (!m_secondStage) {
      #ifdef DEV
      pd->system->logToConsole("IO: Stage 1 Finished");
      #endif
      m_secondStage = true;
      m_actionProgress = 0;
    } else {
      // Specials...
      #ifdef DEV
      pd->system->logToConsole("IO: Stage 2 Finished");
      #endif
      m_secondStage = false;
      m_actionProgress = 0;
      m_doFirst = kDoNothing;
      m_andThen = kDoNothing;
      m_forceSlot = -1;
      updateRenderList();
      resumeMusic();
    }
  }
}

bool hasSaveData(uint8_t _save) { return m_foundSaveData[_save]; }

void setSave(uint8_t _save) { m_save = _save; }

bool hasWorld(uint8_t _slot) { return m_worldExists[m_save][_slot]; }

int doRead(void* _userdata, uint8_t* _buf, int _bufsize) {
  return pd->file->read((SDFile*)_userdata, _buf, _bufsize);
}

void doWrite(void* _userdata, const char* _str, int _len) {
  pd->file->write((SDFile*)_userdata, _str, _len);
}

void decodeError(json_decoder* jd, const char* _error, int _linenum) {
  pd->system->logToConsole("decode error line %i: %s", _linenum, _error);
  doIO(kDoTitle, /*and then*/ kDoNothing);
}

void setSlot(uint8_t _slot) { m_slot = _slot; }

uint8_t getSlot() { return m_slot; }

void setForceSlot(int8_t _slot) { m_forceSlot = _slot; }

///

bool doTitle() {

  reset(true); // including player
  generateTitle();
  setChunkBackgrounds(/*for title = */ true);
  setGameMode(kTitles);
  unZoom();
  setPlayerPosition(14*TILE_PIX, 9*TILE_PIX, /*update current location = */ true);

  populateMenuTitle();
  updateMusic(/*isTitle=*/true);

  return true;
}

///

bool doNewWorld() {
  #ifdef DEV
  pd->system->logToConsole("NEW WORLD: Progress %i", m_actionProgress);
  #endif
  pd->sprite->addSprite(getGenSprite());

  // Create and go to new world!
  if (m_actionProgress == 0) {

    reset(false); // Don't reset the player!
    if (m_forceSlot == -1) pd->system->error("doNewWorld: must set slot override!");
    setSlot(m_forceSlot);

  } else if (m_actionProgress < 9) {
    
    generate(m_actionProgress);

  } else if (m_actionProgress == 9) {

    addObstacles();

  } else if (m_actionProgress == 10) {

    setChunkBackgrounds(/*for title = */ false);

    setGameMode(kWanderMode);

    populateMenuGame(); // Save, Load, Menu

    if (getSlot() == 0) {
      // Starting a new game, set the default settings
      setDefaultPlayerSettings();
      showTutorialMsg(getTutorialStage());
      // TEMP
      //unlockOtherWorlds();
    }

    setPlotCursorToWorld(getSlot());

    // Finished
    // Need to save now, so issue another doIO command from here (TODO, make the IO procedure allow up to three things to be chained instead?)
    doIO(kDoSave, kDoNothing);
    return false; // Hence then have to return false otherwise we'd complete the current doIO...
  }

  ++m_actionProgress;
  return false;
}

///

bool doSaveDelete() {
  #ifdef DEV
  pd->system->logToConsole("SAVE DELTE Save:%i", m_save);
  #endif

  // Hard file system reset
  for (uint16_t ss = 0; ss < WORLD_SAVE_SLOTS; ++ss) {
    snprintf(m_filePath, 32, "world_%i_%i.json", m_save, ss);
    pd->file->unlink(m_filePath, 0);
  }
  snprintf(m_filePath, 32, "player_%i.json", m_save);
  pd->file->unlink(m_filePath, 0);

  // Finished
  return true;
}

///

void scanSlots() {
  m_foundSaveData[m_save] = false;
  for (m_scanSlot = 0; m_scanSlot < WORLD_SAVE_SLOTS; ++m_scanSlot) {
    snprintf(m_filePath, 32, "world_%i_%i.json", m_save, m_scanSlot);
    SDFile* file = pd->file->open(m_filePath, kFileRead|kFileReadData);
    if (!file) {
      #ifdef DEV
      pd->system->logToConsole("Scan world: Save:%i, Slot:%i, No Save", m_save, m_scanSlot);
      #endif
      m_worldExists[m_save][m_scanSlot] = false;
      continue;
    }

    json_decoder jd = {
      .decodeError = decodeError,
      .didDecodeTableValue = scanDidDecode,
      .shouldDecodeTableValueForKey = scanShouldDecodeTableValueForKey
    };

    pd->json->decode(&jd, (json_reader){ .read = doRead, .userdata = file }, NULL);
    pd->file->close(file);

    #ifdef DEV
    pd->system->logToConsole("Scan world: Save:%i, Slot:%i, Version:%i", m_save, m_scanSlot, m_worldVersions[m_save][m_scanSlot]);
    #endif
    m_worldExists[m_save][m_scanSlot] = true;
    m_foundSaveData[m_save] = true;
  }

  // Filter
  for (uint16_t ss = 0; ss < WORLD_SAVE_SLOTS; ++ss) {
    if (m_worldExists[m_save][ss]) {
      if (m_worldVersions[m_save][ss] == 0) {
        #ifdef DEV
        pd->system->logToConsole("Scan world: OLD WORLD DETECTED! Version 0. ACTION: Delete everything and start again");
        #endif
        m_foundSaveData[m_save] = false;
      }
    }
  }

  {
    snprintf(m_filePath, 32, "player_%i.json", m_save);
    SDFile* file = pd->file->open(m_filePath, kFileRead|kFileReadData);
    if (!file) {
      m_foundSaveData[m_save] = false;
      #ifdef DEV
      pd->system->logToConsole("Scan world: No Player data found!");
      #endif
    }
  }

  #ifdef DEV
  pd->system->logToConsole("Scan world: overall result for Save:%i - %s", m_save, m_foundSaveData[m_save] ? "CAN-BE-LOADED" : "CANNOT-LOAD");
  #endif
}

int scanShouldDecodeTableValueForKey(json_decoder* jd, const char* _key) {
  return (strcmp(_key, "sf") == 0);
}

void scanDidDecode(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "sf") == 0) {
    m_worldVersions[m_save][m_scanSlot] = json_intValue(_value);
  } else {
    pd->system->error("scanDidDecode DECODE ISSUE, %s", _key);
  }
}


///

void synchronousSave() {
  m_actionProgress = 0;
  while (doSave(/*synchronous = */ true) == false) {
    // noop
  }
}

bool doSave(bool _synchronous) {
  uint8_t pretty = 0;
  #ifdef DEV
  pretty = 1;
  #endif

  if (!_synchronous) pd->sprite->addSprite(getSaveSprite());
  #ifdef DEV
  pd->system->logToConsole("SAVE: Save:%i, Sync:%i, Progress %i", m_save, _synchronous, m_actionProgress);
  #endif

  if (m_actionProgress == 0) {

    // Should get the latest export averages, guaranteed to be between 60 and 120s worth of data
    updateExport();
    updateSales();

    snprintf(m_filePath, 32, "TMP_player_%i.json", m_save);
    m_file = pd->file->open(m_filePath, kFileWrite);

    pd->json->initEncoder(&m_je, doWrite, m_file, pretty);

  } else if (m_actionProgress == 1) {

    m_je.startTable(&m_je);
    serialisePlayer(&m_je);
    m_je.endTable(&m_je);

    int status = pd->file->close(m_file);

  } else if (m_actionProgress == 2) {

    snprintf(m_filePath, 32, "TMP_world_%i_%i.json", m_save, m_slot);
    SDFile* file = pd->file->open(m_filePath, kFileWrite);

    pd->json->initEncoder(&m_je, doWrite, m_file, pretty);

    m_je.startTable(&m_je);

    m_je.addTableMember(&m_je, "sf", 2);
    m_je.writeInt(&m_je, SAVE_FORMAT);

  } else if (m_actionProgress == 3) {
    serialiseCargo(&m_je);
  } else if (m_actionProgress == 4) {
    serialiseBuilding(&m_je);
  } else if (m_actionProgress == 5) {
    serialiseLocation(&m_je);
  } else if (m_actionProgress == 6) {
    serialiseWorld(&m_je);
  } else if (m_actionProgress == 7) {
    m_je.endTable(&m_je);

    int status = pd->file->close(m_file);
    m_file = NULL;

    #ifdef DEV
    pd->system->logToConsole("SAVE: Saved to Save:%i, Slot %u, save status %i", m_save, m_slot, status);
    #endif

    // Finish by moving into location
    char filePathFinal[32];

    snprintf(m_filePath, 32, "TMP_player_%i.json", m_save);
    snprintf(filePathFinal, 32, "player_%i.json", m_save);
    pd->file->rename(m_filePath, filePathFinal);

    snprintf(m_filePath, 32, "TMP_world_%i_%i.json", m_save, m_slot);
    snprintf(filePathFinal, 32, "world_%i_%i.json", m_save, m_slot);
    pd->file->rename(m_filePath, filePathFinal);

  } else if (m_actionProgress == 8) {
    
    scanSlots();

    // Finished
    #ifdef SLOW_LOAD
    float f; for (int32_t i = 0; i < 10000; ++i) for (int32_t j = 0; j < 10000; ++j) { f*=i*j; }
    #endif

    return true;
  }

  ++m_actionProgress;
  return false;
}

///

bool doLoad() {

  pd->sprite->addSprite(getLoadSprite());
  #ifdef DEV
  pd->system->logToConsole("LOAD: Save:%i, Progress %i", m_save, m_actionProgress);
  #endif

  if (m_actionProgress == 0) {

    // Clear in preparation for load (including player, if we haven't been given a slot override)
    const bool resetThePlayerToo = (m_forceSlot == -1);
    reset(resetThePlayerToo);

    snprintf(m_filePath, 32, "player_%i.json", m_save);
    m_file = pd->file->open(m_filePath, kFileRead|kFileReadData);

    pd->json->decode(&m_jd_p, (json_reader){ .read = doRead, .userdata = m_file }, NULL);

    pd->file->close(m_file);
    m_file = NULL;

  } else if (m_actionProgress == 1) {

    // We have now loaded the correct slot number for this player-save.
    // But we might want to be loading into a different world 
    if (m_forceSlot != -1) {
      #ifdef DEV
      pd->system->logToConsole("LOAD: Save:%i, Slot Override from %i to %i", m_save, m_slot, m_forceSlot);
      #endif
      setSlot(m_forceSlot);
    }

    snprintf(m_filePath, 32, "world_%i_%i.json", m_save, m_slot);
    m_file = pd->file->open(m_filePath, kFileRead|kFileReadData);

    // TODO - call this many times, each time with a different WILL DECODE
    pd->json->decode(&m_jd, (json_reader){ .read = doRead, .userdata = m_file }, NULL);

    pd->file->close(m_file);
    m_file == NULL;

  } else if (m_actionProgress == 2) {

    // Things which need to run post-load
    addObstacles();
    doWetness(/*for titles = */ false);
    setChunkBackgrounds(/*for title = */ false);
    showTutorialMsg(getTutorialStage());

    // Update audio settings
    updateSfx();
    updateMusic(/*isTitle=*/ false);

    setGameMode(kWanderMode);
    populateMenuGame();

    // Finished
    #ifdef SLOW_LOAD
    float f; for (int32_t i = 0; i < 10000; ++i) for (int32_t j = 0; j < 10000; ++j) { f*=i*j; }
    #endif

    return true;
  }

  ++m_actionProgress;    
  return false;
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
