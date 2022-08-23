#include "sound.h"
#include "player.h"

bool m_sfxOn = true;

int8_t m_trackPlaying = -1;

FilePlayer* m_music[N_MUSIC_TRACKS];

SamplePlayer* m_samplePlayer;

AudioSample* m_destroy;
AudioSample* m_clearObstruction;
AudioSample* m_pickCargo;
AudioSample* m_placePlant;
AudioSample* m_unlock;
AudioSample* m_sell;

void musicStopped(SoundSource* _c);

/// ///

void pauseMusic() {
  if (m_trackPlaying == -1) return;
  pd->sound->fileplayer->pause(m_music[m_trackPlaying]);
}

void resumeMusic() {
  if (m_trackPlaying == -1) return;
  pd->sound->fileplayer->play(m_music[m_trackPlaying], 1);
}

void updateMusic(bool _isTitle) {
  if (_isTitle) {
    if (m_trackPlaying != -1) {
      int8_t toStop = m_trackPlaying;
      m_trackPlaying = -1; // Caution: there will be a callback
      pd->sound->fileplayer->stop(m_music[toStop]);
    }
    pd->sound->fileplayer->play(m_music[0], 1);
    m_trackPlaying = 0;
  } else {
    struct Player_t* p = getPlayer();
    const bool musicOn = (p->m_soundSettings & 1);
    if (musicOn) {
      if (m_trackPlaying == -1) {
        pd->sound->fileplayer->play(m_music[0], 1);
        m_trackPlaying = 0;
      } else {
        // music is already playing
      }
    } else { // music off
      if (m_trackPlaying == -1) {
        // music is already off
      } else {
        #ifdef DEV
        pd->system->logToConsole("Stopping %i", m_trackPlaying);
        #endif
        int8_t toStop = m_trackPlaying;
        m_trackPlaying = -1; // Caution: there will be a callback
        pd->sound->fileplayer->stop(m_music[toStop]);
      }
    }
  }
}

void musicStopped(SoundSource* _c) {
  if (m_trackPlaying == -1) {
    return;
  }
  int8_t next = m_trackPlaying;
  while (next == m_trackPlaying) {
    next = rand() % N_MUSIC_TRACKS;
  }
  pd->sound->fileplayer->play(m_music[next], 1);
  m_trackPlaying = next;
}

void updateSfx() {
  struct Player_t* p = getPlayer();
  m_sfxOn = (p->m_soundSettings & 2);
}

void initSound() {

  for (int32_t i = 0; i < N_MUSIC_TRACKS; ++i) {
    m_music[i] = pd->sound->fileplayer->newPlayer();
    switch (i) {
      case 0: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/1985"); break;
      case 1: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/b3"); break;
      case 2: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/pumped"); break;
      case 3: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/sweetSelfSatisfaction"); break;
      default: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/weAreTheResistors"); break;
    }
    pd->sound->fileplayer->setFinishCallback(m_music[i], musicStopped);
  }

  m_samplePlayer = pd->sound->sampleplayer->newPlayer();

  m_destroy = pd->sound->sample->load("sounds/destroy");
  m_clearObstruction = pd->sound->sample->load("sounds/clearObstruction");
  m_pickCargo = pd->sound->sample->load("sounds/pickCargo");
  m_placePlant = pd->sound->sample->load("sounds/placePlant");
  m_unlock = pd->sound->sample->load("sounds/unlock");
  m_sell = pd->sound->sample->load("sounds/sell");

}


void sfxDestroy() {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->setSample(m_samplePlayer, m_destroy);
  pd->sound->sampleplayer->play(m_samplePlayer, 1, 1.0f);
}

void sfxClearObstruction() {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->setSample(m_samplePlayer, m_clearObstruction);
  pd->sound->sampleplayer->play(m_samplePlayer, 1, 1.0f);
}

void sfxPickCargo() {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->setSample(m_samplePlayer, m_pickCargo);
  pd->sound->sampleplayer->play(m_samplePlayer, 1, 1.0f);
}

void sfxPlacePlant() {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->setSample(m_samplePlayer, m_placePlant);
  pd->sound->sampleplayer->play(m_samplePlayer, 1, 1.0f);
}

void sfxUnlock() {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->setSample(m_samplePlayer, m_unlock);
  pd->sound->sampleplayer->play(m_samplePlayer, 1, 1.0f);
}

void sfxSell() {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->setSample(m_samplePlayer, m_sell);
  pd->sound->sampleplayer->play(m_samplePlayer, 1, 1.0f);
}

///////////////
