#include "localisation.h"
#include "ui.h"
#include "sprite.h"

// 128 kb buffer to hold game strings, should be plenty
#define BUFFER_LENGTH (1024 * 128)

#define TEMP_BUFFER 256

char* m_strings;

char* m_tempBuffer;

enum kLanguage m_language = kEN;

uint16_t m_offsets[kNTR + 1]; // +1 due to end of last string

const char* getLangFileStr(const enum kLanguage _l);

enum kLanguage fileToLang(const uint8_t _buf);

#define LANG_SAVE "lang"

/// ///

enum kLanguage getLanguage(void) {
  return m_language;
}

// Y offset for Noto font sizes
int16_t tY(void) {
  return m_language == kEN ? 0 : -6;
}

#define TEX_LEN_ADD_NOTO 0
#define TEX_LEN_ADD_ROOB 5
size_t trLen(const enum kTR _tr) {
  const char* t = tr(_tr);
  size_t s = (m_language == kEN ? TEX_LEN_ADD_ROOB : TEX_LEN_ADD_NOTO) + pd->graphics->getTextWidth(getRoobert10(), t, strlen(t), kUTF8Encoding, 0);
  // Annoying pixel shifting
  switch (_tr) {
    case kTRConvFilter: s += 4; break;
    case kTRFacIn: s -= 2; break;
    case kTRUtilityStorage: case kTRExtractorHopper:
      switch (m_language) {
        case kEN: s -= 2; break;
        default: s -= 4; break;
      }
      break;
    case kTRUIInventoryValue: case kTRShopPrice: case kTROut:
      switch (m_language) {
        case kEN: break;
        default: s += 4; break;
      }
      break;
    default: break;
  }
  return s;
}

void modLanguage(const bool _forward) {
  if (_forward) {
    if (m_language + 1 == kNLanguage) {
      m_language = 0;
    } else {
      ++m_language;
    }
  } else {
    if (!m_language) {
      m_language = kNLanguage - 1;
    } else {
      --m_language;
    }
  }
  SDFile* file = pd->file->open(LANG_SAVE, kFileWrite);
  pd->file->write(file, (void*) &m_language, 1);
  pd->file->close(file);

  resetLocalisation(m_language);
}

const char* tr(const enum kTR _tr) {
  memset(m_tempBuffer, 0, TEMP_BUFFER);
  const size_t start = m_offsets[_tr];
  const size_t stop = m_offsets[_tr + 1];
  const size_t bytes = stop - start - 1; // -1 to avoid the new line character
  if (bytes > 128) {
    memcpy(m_tempBuffer, "STRING TOO LONG", strlen("STRING TOO LONG"));
    return m_tempBuffer;
  }
  //if (_tr == kTRFactoryFarming) pd->system->logToConsole("-> REQ FF: s-1:%i s:%i s+1:%i b:%i", m_offsets[_tr-1], m_offsets[_tr], m_offsets[_tr+1], bytes);
  memcpy(m_tempBuffer, &m_strings[start], bytes);
  return m_tempBuffer;
}

const char* getLangFileStr(const enum kLanguage _l) {
  switch (_l) {
    case kFR: return "lang/FR.txt";
    case kRU: return "lang/RU.txt";
    case kDE: return "lang/DE.txt";
    case kES: return "lang/ES.txt";
    default: return "lang/EN.txt";
  }
}

enum kLanguage fileToLang(const uint8_t _buf) {
  if (_buf >= kNLanguage) {
    return kEN;
  }
  return (enum kLanguage) _buf;
}

void initLocalisation() {
  m_strings = pd->system->realloc(NULL, BUFFER_LENGTH);
  m_tempBuffer = pd->system->realloc(NULL, TEMP_BUFFER);
  #ifdef DEV
  pd->system->logToConsole("malloc: for localisation %i kb", BUFFER_LENGTH/1024);
  #endif
  SDFile* file = pd->file->open(LANG_SAVE, kFileReadData);
  if (file) {
    uint8_t buf;
    int32_t status = pd->file->read(file, (void*) &buf, 1);
    if (status == 1) {
      m_language = fileToLang(buf);
      pd->system->logToConsole("got saved languate: %i", buf);
    }
    pd->file->close(file);
  }
  resetLocalisation(m_language);
}

void resetLocalisation(const enum kLanguage _l) {
  memset(m_strings, 0, BUFFER_LENGTH);
  memset(m_offsets, 0, (kNTR + 1) * sizeof(uint16_t));
  SDFile* f = pd->file->open(getLangFileStr(_l), kFileRead);
  const size_t bytes = pd->file->read(f, (void*)m_strings, BUFFER_LENGTH);
  pd->file->close(f);
  f = NULL;

  // Hunt for new line characters
  size_t word = 1; // word 0 starts at index 0
  for (size_t c = 0; c < bytes; ++c) {
    if (m_strings[c] == '\n') {
      m_offsets[word++] = c+1;

      const size_t length = m_offsets[word - 1] - m_offsets[word - 2];
      if (length > 128) {
        pd->system->error("String %i is too long at %i bytes", word-1, length);
      }
    } 
  }

  #ifdef DEV
  pd->system->logToConsole("resetLocalisation: read %i kb, %i words (expecting %i)", bytes/1024, word, kNTR);
  #endif

  updateLangUI();

}
