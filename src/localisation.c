#include "localisation.h"
#include "ui.h"
#include "sprite.h"

// 128 kb buffer to hold game strings, should be plenty
#define BUFFER_LENGTH (1024 * 128)

#define TEMP_BUFFER 128

char* m_strings;

char* m_tempBuffer0;
char* m_tempBuffer1;
char* m_tempBuffer2;
char* m_tempBuffer3;

enum kLanguage m_language = kEN;

size_t m_offsets[kNTR];

const char* getLangFileStr(const enum kLanguage _l);

/// ///

enum kLanguage getLanguage(void) {
  return m_language;
}

#define TEX_LEN_ADDITIONAL 2
size_t trLen(const enum kTR _tr) {
  return TEX_LEN_ADDITIONAL + pd->graphics->getTextWidth(getRoobert10(), tr(_tr), strlen(tr(_tr)), kUTF8Encoding, 0);
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
  resetLocalisation(m_language);
}

const char* tr(const enum kTR _tr) {
  char* tb = NULL;
  static size_t c = 0;
  //pd->system->logToConsole("use buf %i", c);
  switch (c) {
    case 0: tb = m_tempBuffer0;
    case 1: tb = m_tempBuffer1;
    case 2: tb = m_tempBuffer2;
    case 3: tb = m_tempBuffer3;
  }
  memset(tb, 0, TEMP_BUFFER);
  const size_t start = m_offsets[_tr];
  const size_t stop = m_offsets[_tr + 1];
  const size_t bytes = stop - start - 1; // -1 to avoid the new line character
  memcpy(tb, &m_strings[start], bytes);
  c = (c + 1) % 4;
  return tb;
}

const char* getLangFileStr(const enum kLanguage _l) {
  switch (_l) {
  	case kFR: return "lang/FR.txt";
  	case kRU: return "lang/RU.txt";
  	default: return "lang/EN.txt";
  }
}


void initLocalisation() {
  m_strings = pd->system->realloc(NULL, BUFFER_LENGTH);
  m_tempBuffer0 = pd->system->realloc(NULL, 128);
  m_tempBuffer1 = pd->system->realloc(NULL, 128);
  m_tempBuffer2 = pd->system->realloc(NULL, 128);
  m_tempBuffer3 = pd->system->realloc(NULL, 128);
  #ifdef DEV
  pd->system->logToConsole("malloc: for localisation %i kb", BUFFER_LENGTH/1024);
  #endif
  resetLocalisation(m_language);
}

void resetLocalisation(const enum kLanguage _l) {
  memset(m_strings, 0, BUFFER_LENGTH);
  memset(m_offsets, 0, kNTR * sizeof(size_t));
  SDFile* f = pd->file->open(getLangFileStr(_l), kFileRead);
  const size_t bytes = pd->file->read(f, (void*)m_strings, BUFFER_LENGTH);
  pd->file->close(f);
  f = NULL;

  // Hunt for new line characters
  size_t word = 1; // word 0 starts at index 0
  for (size_t c = 0; c < bytes; ++c) {
  	if (m_strings[c] == '\n') {
  	  m_offsets[word++] = c+1;
  	} 
  }

  updateLangUI();

}
