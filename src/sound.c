#include "sound.h"
#include "player.h"

bool m_sfxOn = true;

int8_t m_trackPlaying = -1;

FilePlayer* m_music[N_MUSIC_TRACKS + 1];

SamplePlayer* m_samplePlayer[kNSFX];
AudioSample* m_audioSample[kNSFX];

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

void playCredits() {
  if (m_trackPlaying == -1) {
    // music is off
    return;
  } 
  #ifdef DEV
  pd->system->logToConsole("Stopping %i", m_trackPlaying);
  #endif
  int8_t toStop = m_trackPlaying;
  m_trackPlaying = -1; // Caution: there will be a callback
  pd->sound->fileplayer->stop(m_music[toStop]);
  pd->sound->fileplayer->play(m_music[N_MUSIC_TRACKS], 1); 
  m_trackPlaying = N_MUSIC_TRACKS;
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

  for (int32_t i = 0; i < N_MUSIC_TRACKS + 1; ++i) {
    m_music[i] = pd->sound->fileplayer->newPlayer();
    switch (i) {
      case 0: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/1985"); break;
      case 1: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/b3"); break;
      case 2: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/pumped"); break;
      case 3: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/sweetSelfSatisfaction"); break;
      case 4: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/weAreTheResistors"); break;
      case 5: pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/SoftAndFuriousHorizonEnding"); break;
    }
    pd->sound->fileplayer->setFinishCallback(m_music[i], musicStopped);
  }

  m_audioSample[kSfxDestroy] = pd->sound->sample->load("sounds/destroy");
  m_audioSample[kSfxClearObstruction] = pd->sound->sample->load("sounds/clearObstruction");
  m_audioSample[kSfxPickCargo] = pd->sound->sample->load("sounds/pickCargo");
  m_audioSample[kSfxPlacePlant] = pd->sound->sample->load("sounds/placePlant");
  m_audioSample[kSfxUnlock] = pd->sound->sample->load("sounds/unlock");
  m_audioSample[kSfxSell] = pd->sound->sample->load("sounds/sell");
  m_audioSample[kSfxA] = pd->sound->sample->load("sounds/a");
  m_audioSample[kSfxB] = pd->sound->sample->load("sounds/b");
  m_audioSample[kSfxD] = pd->sound->sample->load("sounds/d");
  m_audioSample[kSfxMenuOpen] = pd->sound->sample->load("sounds/menuOpen");
  m_audioSample[kSfxRotate] = pd->sound->sample->load("sounds/rotate");
  m_audioSample[kSfxWarp] = pd->sound->sample->load("sounds/warp");
  m_audioSample[kSfxNo] = pd->sound->sample->load("sounds/no");

  for (int32_t i = 0; i < kNSFX; ++i) {
    m_samplePlayer[i] = pd->sound->sampleplayer->newPlayer();
    pd->sound->sampleplayer->setSample(m_samplePlayer[i], m_audioSample[i]);
  }

}

void sfx(enum SfxSample _sample) {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->play(m_samplePlayer[_sample], 1, 1.0f);
}

///////////////
