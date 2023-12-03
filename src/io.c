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
#include "sshot.h"
#include "buildings/special.h"
#include "buildings/factory.h"
#include "buildings/conveyor.h"

uint8_t m_save = 0;

uint8_t m_slot = 0;

uint8_t m_scanSlot = 0;

char m_filePath[32];

int8_t m_worldVersions[N_SAVES][WORLD_SAVE_SLOTS] = {0};

bool m_worldExists[N_SAVES][WORLD_SAVE_SLOTS] = {false};

bool m_foundSaveData[N_SAVES] = {false};

enum kSaveLoadRequest m_doFirst = kDoNothing, m_andThen = kDoNothing, m_andFinally = kDoNothing;

int8_t m_ioStage = 0;

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

void scanDidDecodeSaveFormat(json_decoder* jd, const char* _key, json_value _value);

void deserialiseValueSaveFormat(json_decoder* jd, const char* _key, json_value _value);

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

void doIO(enum kSaveLoadRequest _first, enum kSaveLoadRequest _andThen, enum kSaveLoadRequest _andFinally) {
  if (IOOperationInProgress()) {
    return;
  }
  m_doFirst = _first;
  m_andThen = _andThen;
  m_andFinally = _andFinally;
  m_ioStage = 0;
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
  switch (m_ioStage) {
    case 0: return m_doFirst;
    case 1: return m_andThen;
    case 2: return m_andFinally;
    default: return kDoNothing;
  }
  return kDoNothing;
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
    case kDoScreenShot: finished = doScreenShot(&m_actionProgress); break;
  }

  if (finished) {
    if (m_ioStage == 0) {
      #ifdef DEV
      pd->system->logToConsole("IO: Stage 1 Finished");
      #endif
      ++m_ioStage;
      m_actionProgress = 0;
    } else if (m_ioStage == 1) {
      #ifdef DEV
      pd->system->logToConsole("IO: Stage 2 Finished");
      #endif
      ++m_ioStage;
      m_actionProgress = 0;
    } else {
      // Specials...
      #ifdef DEV
      pd->system->logToConsole("IO: Stage 3 Finished");
      #endif
      m_ioStage = 0;
      m_actionProgress = 0;
      m_doFirst = kDoNothing;
      m_andThen = kDoNothing;
      m_andFinally = kDoNothing;
      m_forceSlot = -1;
      updateRenderList();
      resumeMusic();
      resetMomentum();
      setPlayerVisible(true);
      setTruckPosition(0);
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
  doIO(kDoTitle, /*and then*/ kDoNothing, /*and finally*/ kDoNothing);
}

void setSlot(uint8_t _slot) { m_slot = _slot; }

uint8_t getSlot() { return m_slot; }

uint8_t getSave() { return m_save; }


void setForceSlot(int8_t _slot) { m_forceSlot = _slot; }

///

bool doTitle() {

  reset(true); // including player
  setGameMode(kTitles);
  generateTitle();
  setChunkBackgrounds(/*for title = */ true);
  unZoom();
  setPlayerPosition(12*TILE_PIX, 7*TILE_PIX, /*update current location = */ true);

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
  addSaveLoadProgressSprite(m_actionProgress, 10);

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

    findPlotCentres();

    setGameMode(kWanderMode);

    populateMenuGame(); // Save, Load, Menu

    if (getSlot() == 0) {
      // Starting a new game, set the default settings
      setDefaultPlayerSettings();
      showTutorialMsg(getTutorialStage());
      forceTorus();
    }

    setPlotCursorToWorld(getSlot());

    // Finished
    return true;
  }

  ++m_actionProgress;
  return false;
}

///

bool doSaveDelete() {
  #ifdef DEV
  pd->system->logToConsole("DELETE SAVE (via rename to delete_world and delete_save) %i", m_save);
  #endif

  char filePathDelete[32];

  // Don't actually delete, just rename to "deleted_"
  for (uint16_t ss = 0; ss < WORLD_SAVE_SLOTS; ++ss) {
    snprintf(m_filePath, 32, "world_%i_%i.json", m_save+1, ss+1);
    snprintf(filePathDelete, 32, "deleted_world_%i_%i.json", m_save+1, ss+1);
    // First unlink before moving
    int status = pd->file->unlink(filePathDelete, 0);
    #ifdef DEV
    pd->system->logToConsole("DELETE: unlink previous world save %s, status %i", filePathDelete, status);
    #endif
    pd->file->rename(m_filePath, filePathDelete);
  }
  snprintf(m_filePath, 32, "player_%i.json", m_save+1);
  snprintf(filePathDelete, 32, "deleted_player_%i.json", m_save+1);
  int status = pd->file->unlink(filePathDelete, 0);
  #ifdef DEV
  pd->system->logToConsole("DELETE unlink previous player save %s, status %i", filePathDelete, status);
  #endif
  pd->file->rename(m_filePath, filePathDelete);

  // Finished
  return true;
}

///

void scanSlots() {
  m_foundSaveData[m_save] = false;
  for (m_scanSlot = 0; m_scanSlot < WORLD_SAVE_SLOTS; ++m_scanSlot) {
    m_worldVersions[m_save][m_scanSlot] = -1;

    snprintf(m_filePath, 32, "world_%i_%i.json", m_save+1, m_scanSlot+1);
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
      .didDecodeTableValue = scanDidDecodeSaveFormat,
      .shouldDecodeTableValueForKey = scanShouldDecodeTableValueForKey
    };

    pd->json->decode(&jd, (json_reader){ .read = doRead, .userdata = file }, NULL);
    int status = pd->file->close(file);
    if (status) pd->system->error("SCAN SLOTS ERROR: wold file->close status code: %i", status);

    #ifdef DEV
    pd->system->logToConsole("Scan world: Save:%i, Slot:%i, Version:%i", m_save, m_scanSlot, m_worldVersions[m_save][m_scanSlot]);
    #endif
    m_worldExists[m_save][m_scanSlot] = true;
    m_foundSaveData[m_save] = true;
  }

  // Filter
  for (uint16_t ss = 0; ss < WORLD_SAVE_SLOTS; ++ss) {
    if (m_worldExists[m_save][ss]) {
      if (m_worldVersions[m_save][ss] == -1) {
        pd->system->error("Scan world: Unable to determine save version for save file %i", m_save);
        m_foundSaveData[m_save] = false;
        m_worldExists[m_save][ss] = false;
      } else if (m_worldVersions[m_save][ss] < EARLIEST_SUPPORTED_SAVE_FORMAT) {
        pd->system->logToConsole("Scan world: PRE-BETA WORLD DETECTED! Version %i < %i."
          " ACTION: Delete everything and start again", m_worldVersions[m_save][ss], EARLIEST_SUPPORTED_SAVE_FORMAT);
        m_foundSaveData[m_save] = false;
        m_worldExists[m_save][ss] = false;
        doSaveDelete();
      } else if (m_worldVersions[m_save][ss] > CURRENT_SAVE_FORMAT) {
        pd->system->error("Scan world: FUTURE SAVE VERSION DETECTED! Version %i > %i."
          " Update Factory Farming to the latest version!", m_worldVersions[m_save][ss], CURRENT_SAVE_FORMAT);
        m_foundSaveData[m_save] = false;
        m_worldExists[m_save][ss] = false;
      }
    }
  }

  {
    snprintf(m_filePath, 32, "player_%i.json", m_save+1);
    SDFile* file = pd->file->open(m_filePath, kFileRead|kFileReadData);
    if (!file) {
      m_foundSaveData[m_save] = false;
      #ifdef DEV
      pd->system->logToConsole("Scan world: No Player data found!");
      #endif
    } else {
      int status = pd->file->close(file);
      if (status) pd->system->error("SCAN SLOTS ERROR: player file->close status code: %i", status);
      #ifdef DEV
      pd->system->logToConsole("Scan world: Player data found");
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

void scanDidDecodeSaveFormat(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "sf") == 0) {
    m_worldVersions[m_save][m_scanSlot] = json_intValue(_value);
  } else {
    pd->system->error("scanDidDecodeSaveFormat DECODE ISSUE, %s", _key);
  }
}

void deserialiseValueSaveFormat(json_decoder* jd, const char* _key, json_value _value) {
  if (strcmp(_key, "sf") == 0) {
    m_worldVersions[m_save][m_slot] = json_intValue(_value);
  } else {
    pd->system->error("deserialiseValueSaveFormat DECODE ISSUE, %s", _key);
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
  #ifdef DEMO
  return true;
  #endif

  uint8_t pretty = 0;
  #ifdef DEV
  pretty = 1;
  #endif

  if (!_synchronous) {
    pd->sprite->addSprite(getSaveSprite());
    addSaveLoadProgressSprite(m_actionProgress, 8);
  }
  #ifdef DEV
  pd->system->logToConsole("SAVE: Save:%i, Sync:%i, Progress %i", m_save, _synchronous, m_actionProgress);
  #endif

  if (m_actionProgress == 0) {

    // Should get the latest export averages, guaranteed to be between 60 and 120s worth of data
    updateExport();
    updateSales();

    snprintf(m_filePath, 32, "TMP_player_%i.json", m_save+1);

    // This file should not already exist, double check
    int status = pd->file->unlink(m_filePath, 0);
    #ifdef DEV
    pd->system->logToConsole("SAVE: unlink previous TMP_player %s, status %i (expect this to fail, err: %s)", m_filePath, status, pd->file->geterr());
    #endif

    if (m_file) pd->system->error("SAVE ERROR: tmp player error: overwriting exiting file ptr");
    m_file = pd->file->open(m_filePath, kFileWrite);
    if (!m_file) pd->system->error("SAVE ERROR: tmp player file creation error: %s", pd->file->geterr());

    pd->json->initEncoder(&m_je, doWrite, m_file, pretty);

  } else if (m_actionProgress == 1) {

    m_je.startTable(&m_je);
    serialisePlayer(&m_je);
    m_je.endTable(&m_je);

    int status = pd->file->close(m_file);
    if (status) pd->system->error("SAVE ERROR: tmp player file->close status code: %i, err: %s", status, pd->file->geterr());
    m_file = NULL;

  } else if (m_actionProgress == 2) {

    snprintf(m_filePath, 32, "TMP_world_%i_%i.json", m_save+1, m_slot+1);

    // This file should not already exist, double check
    int status = pd->file->unlink(m_filePath, 0);
    #ifdef DEV
    pd->system->logToConsole("SAVE: unlink previous TMP_world %s, status %i (expect this to fail, err: %s)", m_filePath, status, pd->file->geterr());
    #endif

    if (m_file) pd->system->error("SAVE ERROR: tmp world error: overwriting exiting file ptr");
    m_file = pd->file->open(m_filePath, kFileWrite);
    if (!m_file) pd->system->error("SAVE ERROR: tmp world file creation error: %s", pd->file->geterr());

    pd->json->initEncoder(&m_je, doWrite, m_file, pretty);

    m_je.startTable(&m_je);

    m_je.addTableMember(&m_je, "sf", 2);
    m_je.writeInt(&m_je, CURRENT_SAVE_FORMAT);

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
    if (status) pd->system->error("SAVE ERROR: tmp world file->close status code: %i, err %s", status, pd->file->geterr());
    m_file = NULL;

    // Create backup
    char filePathBackup[32] = {0};
    FileStat fs;

    // We can only backup the player if we are not resetting them (new game)
    snprintf(filePathBackup, 32, "backup_player_%i.json", m_save+1);
    status = pd->file->stat(filePathBackup, &fs); // Check also we have a player to backup
    if (m_doFirst != kDoResetPlayer && status == 0) {
      snprintf(m_filePath, 32, "player_%i.json", m_save+1);
      status = pd->file->unlink(filePathBackup, 0);
      #ifdef DEV
      pd->system->logToConsole("SAVE: unlink previous player backup %s, status %i", filePathBackup, status);
      pd->system->logToConsole("SAVE: Backup: %s -> %s", m_filePath, filePathBackup);
      #endif
      status = pd->file->rename(m_filePath, filePathBackup);
      if (status) pd->system->error("SAVE ERROR: backup player file->rename status code: %i, err: %s", status, pd->file->geterr());
    }

    // We can only backup a previous world file if we are not in worldgen mode
    snprintf(filePathBackup, 32, "backup_world_%i_%i.json", m_save+1, m_slot+1);
    status = pd->file->stat(filePathBackup, &fs); // Check also we have a world to backup
    if (m_andThen != kDoNewWorld && status == 0) {
      snprintf(m_filePath, 32, "world_%i_%i.json", m_save+1, m_slot+1);
      status = pd->file->unlink(filePathBackup, 0);
      #ifdef DEV
      pd->system->logToConsole("SAVE: unlink previous world backup %s, status %i", filePathBackup, status);
      pd->system->logToConsole("SAVE: Backup: %s -> %s", m_filePath, filePathBackup);
      #endif
      status = pd->file->rename(m_filePath, filePathBackup);
      if (status) pd->system->error("SAVE ERROR: backup world file->rename status code: %i, err: %s", status, pd->file->geterr());
    }

    // Finish by moving into location
    {
      char filePathFinal[32];
      snprintf(m_filePath, 32, "TMP_player_%i.json", m_save+1);
      snprintf(filePathFinal, 32, "player_%i.json", m_save+1);
      status = pd->file->unlink(filePathFinal, 0);
      #ifdef DEV
      pd->system->logToConsole("SAVE: unlink previous player %s, status %i (expect this to fail, err: %s)", filePathFinal, status, pd->file->geterr());
      pd->system->logToConsole("SAVE: Finalise: %s -> %s", m_filePath, filePathFinal);
      #endif
      status = pd->file->rename(m_filePath, filePathFinal);
      if (status) pd->system->error("SAVE ERROR: mv player file->rename status code: %i, err: %s", status, pd->file->geterr());

      snprintf(m_filePath, 32, "TMP_world_%i_%i.json", m_save+1, m_slot+1);
      snprintf(filePathFinal, 32, "world_%i_%i.json", m_save+1, m_slot+1);
      status = pd->file->unlink(filePathFinal, 0);
      #ifdef DEV
      pd->system->logToConsole("SAVE: unlink previous world %s, status %i (expect this to fail, err: %s)", filePathFinal, status, pd->file->geterr());
      pd->system->logToConsole("SAVE: Finalise: %s -> %s", m_filePath, filePathFinal);
      #endif
      status = pd->file->rename(m_filePath, filePathFinal);
      if (status) pd->system->error("SAVE ERROR: mv world file->rename status code: %i, err: %s", status, pd->file->geterr());
    }

    #ifdef DEV
    pd->system->logToConsole("SAVE: Saved to Save:%i, Slot %u, save status %i", m_save, m_slot, status);
    #endif

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
  addSaveLoadProgressSprite(m_actionProgress, 5);
  #ifdef DEV
  pd->system->logToConsole("LOAD: Save:%i, Progress %i", m_save, m_actionProgress);
  #endif

  if (m_actionProgress == 0) {

    // Clear in preparation for load (including player, if we haven't been given a slot override)
    const bool resetThePlayerToo = (m_forceSlot == -1);
    reset(resetThePlayerToo);

    snprintf(m_filePath, 32, "player_%i.json", m_save+1);
    if (m_file) pd->system->error("LOAD ERROR: read player error: overwriting exiting file ptr");
    m_file = pd->file->open(m_filePath, kFileRead|kFileReadData);
    if (!m_file) pd->system->error("LOAD ERROR: read player error: %s", pd->file->geterr());

    pd->json->decode(&m_jd_p, (json_reader){ .read = doRead, .userdata = m_file }, NULL);

    int status = pd->file->close(m_file);
    if (status) pd->system->error("LOAD ERROR: player file->close status code: %i", status);
    m_file = NULL;

  } else if (m_actionProgress >= 1 && m_actionProgress <= 4) {

    // We have now loaded the correct slot number for this player-save.
    // But we might want to be loading into a different world 
    if (m_forceSlot != -1) {
      #ifdef DEV
      pd->system->logToConsole("LOAD: Save:%i, Slot Override from %i to %i", m_save, m_slot, m_forceSlot);
      #endif
      setSlot(m_forceSlot);
    }

    snprintf(m_filePath, 32, "world_%i_%i.json", m_save+1, m_slot+1);
    if (m_file) pd->system->error("LOAD ERROR: read world error: overwriting exiting file ptr");
    m_file = pd->file->open(m_filePath, kFileRead|kFileReadData);
    if (!m_file) pd->system->error("LOAD ERROR: read world error: %s", pd->file->geterr());

    pd->json->decode(&m_jd, (json_reader){ .read = doRead, .userdata = m_file }, NULL);

    int status = pd->file->close(m_file);
    if (status) pd->system->error("LOAD ERROR: world file->close status code: %i", status);
    m_file = NULL;

  } else if (m_actionProgress == 5) {

    // SCHEMA EVOLUTION - V4 to V5 (v1.0 to v1.1)
    // Factories now save their production time internally. Need to compute this for all existing factories.
    if (m_worldVersions[m_save][m_slot] == V1p0_SAVE_FORMAT) {
      m_worldVersions[m_save][m_slot] = V1p1_SAVE_FORMAT;
      uint16_t nFacsUpdated = 0;
      for (uint16_t i = 0; i < BUILDING_LIMIT; ++i) {
        struct Building_t* b = buildingManagerGetByIndex(i);
        if (!b) continue;
        if (b->m_type != kFactory) continue;
        ++nFacsUpdated;
        updateFactoryUpgrade(b);
      } 
      pd->system->logToConsole("-- Performed world schema evolution from v%i to v%i (Save:%i, World:%i), updated %i factories", 
        V1p0_SAVE_FORMAT, V1p1_SAVE_FORMAT, m_save, m_slot, nFacsUpdated);
    }

    // SCHEMA EVOLUTION - V5 to V6 (v1.1 to v1.5)
    if (m_worldVersions[m_save][m_slot] == V1p1_SAVE_FORMAT) {
      m_worldVersions[m_save][m_slot] = V1p5_SAVE_FORMAT;
      // Just adding data - nothing to migrate
      pd->system->logToConsole("-- Performed world schema evolution from v%i to v%i", V1p1_SAVE_FORMAT, V1p5_SAVE_FORMAT);
    }

    // BUG FIX - v < 1.6
    struct Player_t* p = getPlayer();
    if (p->m_importConsumers[kNoCargo]) {
      pd->system->logToConsole("-- BUG: Have %i importers of the NoCargo placeholder in this save. Fixing.", p->m_importConsumers[kNoCargo]);
      p->m_importConsumers[kNoCargo] = 0;
    }

    // Things which need to run post-load
    setGameMode(kWanderMode);

    // need to refresh conveyor sprite connections
    for (uint16_t i = 0; i < BUILDING_LIMIT; ++i) {
      conveyorUpdateSprite(buildingManagerGetByIndex(i));
    }

    findPlotCentres();
    addObstacles();
    doWetness(/*for titles = */ false);
    setChunkBackgrounds(/*for title = */ false);
    showTutorialMsg(getTutorialStage());

    // Update audio settings
    updateSfx();
    updateMusic(/*isTitle=*/ false);
    updateMusicVol();

    populateMenuGame();

    forceTorus();

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
    jd->didDecodeTableValue = deserialiseValueSaveFormat;
  } else if (m_actionProgress == 1 && strcmp(truncated, "cargo") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueCargo;
    jd->didDecodeSublist = deserialiseStructDoneCargo;
  } else if (m_actionProgress == 2 && strcmp(truncated, "build") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueBuilding;
    jd->didDecodeSublist = deserialiseStructDoneBuilding;
  } else if (m_actionProgress == 3 && strcmp(truncated, "locat") == 0 && _type == kJSONTable) {
    jd->didDecodeTableValue = deserialiseValueLocation;
    jd->didDecodeSublist = deserialiseStructDoneLocation;
  } else if (m_actionProgress == 4 && strcmp(truncated, "world") == 0 && _type == kJSONArray) {
    jd->didDecodeTableValue = NULL;
    jd->didDecodeSublist = NULL;
    jd->didDecodeArrayValue = deserialiseArrayValueWorld;
  } else {
    jd->didDecodeTableValue = NULL;
  }
  
}
