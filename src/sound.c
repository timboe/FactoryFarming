#include "sound.h"
#include "player.h"

bool m_sfxOn = true;

bool m_hasMusic = true;

int8_t m_trackPlaying = -1;

FilePlayer* m_music[N_MUSIC_TRACKS + 1];

SamplePlayer* m_samplePlayer[kNSFX];
AudioSample* m_audioSample[kNSFX];

void musicStopped(SoundSource* _c);

/// ///

void pauseMusic() {
  if (!m_hasMusic) return;
  if (m_trackPlaying == -1) return;
  pd->sound->fileplayer->pause(m_music[m_trackPlaying]);
}

void resumeMusic() {
  if (!m_hasMusic) return;
  if (m_trackPlaying == -1) return;
  pd->sound->fileplayer->play(m_music[m_trackPlaying], 1);
}

void updateMusicVol() {
  if (!m_hasMusic) return;
  if (m_trackPlaying == -1) return;
  int8_t i = getPlayer()->m_musicVol;
  if (!i) i = 100;
  float v = i * 0.01f;
  pd->sound->fileplayer->setVolume(m_music[m_trackPlaying], v, v);
}

void updateMusic(bool _isTitle) {
  if (!m_hasMusic) return;
  if (_isTitle) {
    if (m_trackPlaying != -1) {
      int8_t toStop = m_trackPlaying;
      m_trackPlaying = -1; // Caution: there will be a callback
      pd->sound->fileplayer->stop(m_music[toStop]);
    }
    pd->sound->fileplayer->play(m_music[0], 1);
    m_trackPlaying = 0;
    updateMusicVol();
  } else {
    struct Player_t* p = getPlayer();
    const bool musicOn = (p->m_soundSettings & 1);
    if (musicOn) {
      if (m_trackPlaying == -1) {
        pd->sound->fileplayer->play(m_music[0], 1);
        m_trackPlaying = 0;
        updateMusicVol();
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

void chooseMusic(int8_t _id) {
  if (!m_hasMusic) return;
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
  pd->sound->fileplayer->play(m_music[_id], 1); 
  m_trackPlaying = _id;
  updateMusicVol();
}

void musicStopped(SoundSource* _c) {
  if (!m_hasMusic) return;
  if (m_trackPlaying == -1) {
    return;
  }
  int8_t next = m_trackPlaying;
  while (next == m_trackPlaying) {
    next = rand() % N_MUSIC_TRACKS;
  }
  pd->sound->fileplayer->play(m_music[next], 1);
  m_trackPlaying = next;
  updateMusicVol();
}

void updateSfx() {
  struct Player_t* p = getPlayer();
  m_sfxOn = (p->m_soundSettings & 2);
}

void initSound() {

  m_audioSample[kSfxDestroy] = pd->sound->sample->load("sounds/destroy");
  m_audioSample[kSfxClearObstruction] = pd->sound->sample->load("sounds/clearObstruction");
  m_audioSample[kSfxPickCargo] = pd->sound->sample->load("sounds/pickCargo");
  m_audioSample[kSfxPlacePlant] = pd->sound->sample->load("sounds/placePlant");
  m_audioSample[kSfxPlaceConv] = pd->sound->sample->load("sounds/placeConv");
  m_audioSample[kSfxPlaceFac] = pd->sound->sample->load("sounds/placeFac");
  m_audioSample[kSfxPlaceExtract] = pd->sound->sample->load("sounds/placeExtract");
  m_audioSample[kSfxPlaceUtil] = pd->sound->sample->load("sounds/placeUtil");
  m_audioSample[kSfxPlaceCargo] = pd->sound->sample->load("sounds/placeCargo");
  m_audioSample[kSfxUnlock] = pd->sound->sample->load("sounds/unlock");
  m_audioSample[kSfxSell] = pd->sound->sample->load("sounds/sell");
  m_audioSample[kSfxBuy] = pd->sound->sample->load("sounds/buy");
  m_audioSample[kSfxA] = pd->sound->sample->load("sounds/a");
  m_audioSample[kSfxB] = pd->sound->sample->load("sounds/b");
  m_audioSample[kSfxD] = pd->sound->sample->load("sounds/d");
  m_audioSample[kSfxMenuOpen] = pd->sound->sample->load("sounds/menuOpen");
  m_audioSample[kSfxRotate] = pd->sound->sample->load("sounds/rotate");
  m_audioSample[kSfxWarp] = pd->sound->sample->load("sounds/warp");
  m_audioSample[kSfxZoom] = pd->sound->sample->load("sounds/zoom");
  m_audioSample[kSfxNo] = pd->sound->sample->load("sounds/no");
  m_audioSample[kSfxConveyor] = pd->sound->sample->load("sounds/conveyor");
  m_audioSample[kFootstepDefault] = pd->sound->sample->load("sounds/foot0");
  m_audioSample[kFootstepDefault1] = pd->sound->sample->load("sounds/foot1");
  m_audioSample[kFootstepDefault2] = pd->sound->sample->load("sounds/foot2");
  m_audioSample[kFootstepDefault3] = pd->sound->sample->load("sounds/foot3");
  m_audioSample[kFootstepDefault4] = pd->sound->sample->load("sounds/foot4");
  m_audioSample[kFootstepDefault5] = pd->sound->sample->load("sounds/foot5");
  m_audioSample[kFootstepWater] = pd->sound->sample->load("sounds/step0");
  m_audioSample[kFootstepWater1] = pd->sound->sample->load("sounds/step1");
  m_audioSample[kFootstepWater2] = pd->sound->sample->load("sounds/step2");
  m_audioSample[kFootstepWater3] = pd->sound->sample->load("sounds/step3");
  m_audioSample[kFootstepWater4] = pd->sound->sample->load("sounds/step4");
  m_audioSample[kFootstepWater5] = pd->sound->sample->load("sounds/step5");

  for (int32_t i = 0; i < kNSFX; ++i) {
    m_samplePlayer[i] = pd->sound->sampleplayer->newPlayer();
    pd->sound->sampleplayer->setSample(m_samplePlayer[i], m_audioSample[i]);
  }

  m_hasMusic = true;
  for (int32_t i = 0; i < N_MUSIC_TRACKS + 1; ++i) {
    m_music[i] = pd->sound->fileplayer->newPlayer();
    pd->sound->fileplayer->setBufferLength(m_music[i], 5.0f); 
    switch (i) {
      case 0: m_hasMusic &= pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/1985"); break;
      case 1: m_hasMusic &= pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/b3"); break;
      case 2: m_hasMusic &= pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/weeklybeats2014_4"); break;
      case 3: m_hasMusic &= pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/sweetSelfSatisfaction"); break;
      case 4: m_hasMusic &= pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/weAreTheResistors"); break;
      case 5: m_hasMusic &= pd->sound->fileplayer->loadIntoPlayer(m_music[i], "music/SoftAndFuriousHorizonEnding"); break;
    }
    pd->sound->fileplayer->setFinishCallback(m_music[i], musicStopped);
  }
}

void sfx(enum SfxSample _sample) {
  if (!m_sfxOn) return;
  pd->sound->sampleplayer->play(m_samplePlayer[_sample], 1, 1.0f);
}

///////////////
