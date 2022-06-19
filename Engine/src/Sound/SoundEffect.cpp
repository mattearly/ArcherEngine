#include "SoundEffect.h"
#include <sndfile.h>
#include <AL\alext.h>
#include "ErrorCheck.h"
#include <inttypes.h>
#include <vector>

namespace AA {

SoundEffect::SoundEffect(const char* filename) {
  ALenum err, format;
  ALuint buffer;
  SNDFILE* sndfile;
  SF_INFO sfinfo;
  short* membuf;
  sf_count_t num_frames;
  ALsizei num_bytes;

  /* Open the audio file and check that it's usable. */
  sndfile = sf_open(filename, SFM_READ, &sfinfo);
  if (!sndfile) {
    fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
    throw("short soundfile could not be loaded");
  }
  if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
    fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
    sf_close(sndfile);
    throw("bad sample count");
  }

  /* Get the sound format, and figure out the OpenAL format */
  format = AL_NONE;
  if (sfinfo.channels == 1)
    format = AL_FORMAT_MONO16;
  else if (sfinfo.channels == 2)
    format = AL_FORMAT_STEREO16;
  else if (sfinfo.channels == 3) {
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
      format = AL_FORMAT_BFORMAT2D_16;
  } else if (sfinfo.channels == 4) {
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
      format = AL_FORMAT_BFORMAT3D_16;
  }
  if (!format) {
    fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
    sf_close(sndfile);
    throw("unsupported channel count");
  }

  /* Decode the whole audio file to a buffer. */
  membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

  num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
  if (num_frames < 1) {
    free(membuf);
    sf_close(sndfile);
    fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
    throw("failed to read samples");
  }
  num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

  /* Buffer the audio data into a new buffer object, then free the data and
  * close the file.
  */
  buffer = 0;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

  free(membuf);
  sf_close(sndfile);

  /* Check if an error occured, and clean up if so. */
  err = alGetError();
  if (err != AL_NO_ERROR) {
    fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
    if (buffer && alIsBuffer(buffer))
      alDeleteBuffers(1, &buffer);
    throw("openALerror");
  }

  if (buffer == 0)
    throw("buffer is 0, unable to load");

  _Buffer = buffer;
  _FilePath = filename;
}

SoundEffect::~SoundEffect() {
  alDeleteBuffers(1, &_Buffer);
}

}  // end namespace AA