#include "sound.h"

static PlaydateAPI* pd = NULL;

bool m_sfxOn = true;
bool m_musicOn = true;

FilePlayer* m_music;

SamplePlayer* m_samplePlayer;

AudioSample* m_a;

void music(bool _onoff) {
  m_musicOn = _onoff;
}

void sfx(bool _onoff) {
  m_sfxOn = _onoff;
}

void initSound(PlaydateAPI* _pd) {
  pd = _pd;

  m_music = pd->sound->fileplayer->newPlayer();
  //int result = pd->sound->fileplayer->loadIntoPlayer(m_music, "sounds/");
  
  m_samplePlayer = pd->sound->sampleplayer->newPlayer();


  //m_a = pd->sound->sample->load("sounds/");
}


void deinitSound() {
}

