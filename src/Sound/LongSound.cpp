#include "../Sound/LongSound.h"
#include "../Sound/SoundDevice.h"
#include <cassert>
#include <AL/alc.h>
#include <AL/alext.h>
#include "ErrorCheck.h"

namespace AA {

void LongSound::Play() {
  // if already playing, don't do anything
  if (GetPlayingState() == AL_PLAYING)
    return;

  /* Rewind the source position and clear the buffer queue */
  alSourceRewind(p_Source);
  local_alErrorCheck();

  alSourcei(p_Source, AL_BUFFER, 0);
  local_alErrorCheck();

  /* Fill the buffer queue */
  ALsizei i;
  for (i = 0; i < BUFFER_COUNT; i++) {
    /* Get some data to give it to the buffer */
    sf_count_t slen = sf_readf_short(p_Sndfile, p_Membuf, BUFFER_SAMPLES);
    if (slen < 1) break;

    slen *= p_Sfinfo.channels * (sf_count_t)sizeof(short);
    alBufferData(p_Buffers[i], p_Format, p_Membuf, (ALsizei)slen, p_Sfinfo.samplerate);
    local_alErrorCheck();
  }

  /* Now queue and start playback! */
  alSourceQueueBuffers(p_Source, i, p_Buffers);
  local_alErrorCheck();

  alSourcePlay(p_Source);
  local_alErrorCheck();
}

void LongSound::Pause() {
  // if not currently Playing, pause does nothing
  if (GetPlayingState() != AL_PLAYING)
    return;

  alSourcePause(p_Source);
  local_alErrorCheck();
}

void LongSound::Resume() {
  // do nothing if not paused
  if (GetPlayingState() != AL_PAUSED)
    return;

  alSourcePlay(p_Source);
  local_alErrorCheck();
}

void LongSound::Stop() {
  // if already stopped, do nothing
  if (GetPlayingState() == AL_STOPPED)
    return;

  alSourceStop(p_Source);
  local_alErrorCheck();
}

void LongSound::SetVolume(const float& val) {
  float newvolume = val;
  if (newvolume < 0.f) {
    newvolume = 0.f;
  }
  alSourcef(p_Source, AL_GAIN, newvolume);
  local_alErrorCheck();
}

void LongSound::UpdatePlayBuffer() {
  if (GetPlayingState() != AL_PLAYING)
    return;

  ALint processed;
  alGetSourcei(p_Source, AL_BUFFERS_PROCESSED, &processed);
  local_alErrorCheck();

  /* Unqueue and handle each processed buffer */
  while (processed > 0) {
    ALuint bufid;
    sf_count_t slen;

    alSourceUnqueueBuffers(p_Source, 1, &bufid);
    local_alErrorCheck();

    processed--;

    /* Read the next chunk of data, refill the buffer, and queue it back on the source */
    slen = sf_readf_short(p_Sndfile, p_Membuf, BUFFER_SAMPLES);
    if (slen > 0) {
      slen *= p_Sfinfo.channels * (sf_count_t)sizeof(short);
      alBufferData(bufid, p_Format, p_Membuf, (ALsizei)slen, p_Sfinfo.samplerate);
      local_alErrorCheck();

      alSourceQueueBuffers(p_Source, 1, &bufid);
      local_alErrorCheck();
    }
  }
}

ALint LongSound::GetPlayingState() {
  ALint curr_state;
  alGetSourcei(p_Source, AL_SOURCE_STATE, &curr_state);
  local_alErrorCheck();
  return curr_state;
}

bool LongSound::IsPlaying() {
  if (GetPlayingState() == AL_PLAYING)
    return true;

  return false;
}

LongSound::LongSound(const char* filename) {
  alGenSources(1, &p_Source);
  local_alErrorCheck();

  alGenBuffers(BUFFER_COUNT, p_Buffers);
  local_alErrorCheck();

  std::size_t frame_size;

  p_Sndfile = sf_open(filename, SFM_READ, &p_Sfinfo);
  if (!p_Sndfile) {
    //fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(NULL));
    throw("Could not open audio file");
  }

  /* Get the sound format, and figure out the OpenAL format */
  p_Format = 0;
  if (p_Sfinfo.channels == 1)
    p_Format = AL_FORMAT_MONO16;
  else if (p_Sfinfo.channels == 2)
    p_Format = AL_FORMAT_STEREO16;
  else if (p_Sfinfo.channels == 3) {
    if (sf_command(p_Sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
      p_Format = AL_FORMAT_BFORMAT2D_16;
  } else if (p_Sfinfo.channels == 4) {
    if (sf_command(p_Sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
      p_Format = AL_FORMAT_BFORMAT3D_16;
  }

  if (!p_Format) {
    //fprintf(stderr, "Unsupported channel count: %d\n", p_Sfinfo.channels);
    sf_close(p_Sndfile);
    p_Sndfile = NULL;
    throw("Unsupported channel count in opening new sound file");
  }

  frame_size = ((size_t)BUFFER_SAMPLES * (size_t)p_Sfinfo.channels) * sizeof(short);
  p_Membuf = static_cast<short*>(malloc(frame_size));

}

LongSound::~LongSound() {
  if (IsPlaying())
    Stop();

  //routine: delete all buffer data and snd file stuff
  //close player file
  if (p_Sndfile) {
    sf_close(p_Sndfile);
  }
  p_Sndfile = nullptr;
  free(p_Membuf);
  p_Membuf = nullptr;

  //delete sources
  alDeleteSources(1, &p_Source);

  //delete buffers
  alDeleteBuffers(BUFFER_COUNT, p_Buffers);
}

};
