#include "Speaker.h"
#include "ErrorCheck.h"
#include <AL\al.h>
#include <vector>
#include <unordered_map>

/// <reference>
/// AL_PITCH f, fv pitch multiplier, always positive
/// AL_GAIN f, fv source gain value should be positive
/// AL_MAX_DISTANCE f, fv, i, iv used with the Inverse Clamped Distance Model
///                              to set the distance where there will no longer be
///                              any attenuation of the source
///                              AL_ROLLOFF_FACTOR f, fv, i, iv the rolloff rate for the source
///                              default is 1.0
/// AL_REFERENCE_DISTANCE f, fv, i, iv the distance under which the volume for the
///                                    source would normally drop by half(before
///                                    being influenced by rolloff factor or AL_MAX_DISTANCE)
/// AL_MIN_GAIN f, fv the minimum gain for this source
/// AL_MAX_GAIN f, fv the maximum gain for this source
/// AL_CONE_OUTER_GAIN f, fv the gain when outside the oriented cone
/// AL_CONE_INNER_ANGLE f, fv, i, iv the gain when inside the oriented cone
/// AL_CONE_OUTER_ANGLE f, fv, i, iv outer angle of the sound cone, in degrees
///                                  default is 360
/// AL_POSITION fv, 3f X, Y, Z position
/// AL_VELOCITY fv, 3f velocity vector
/// AL_DIRECTION fv, 3f, iv, 3i direction vector
/// AL_SOURCE_RELATIVE i, iv determines if the positions are relative to the listener
///                          default is AL_FALSE
/// AL_SOURCE_TYPE i, iv the source type – AL_UNDETERMINED, AL_STATIC, or AL_STREAMING
/// AL_LOOPING i, iv turns looping on(AL_TRUE) or off(AL_FALSE)
/// AL_BUFFER i, iv the ID of the attached buffer
/// AL_SOURCE_STATE i, iv the state of the source(AL_STOPPED, AL_PLAYING, …)
/// AL_BUFFERS_QUEUED* i, iv the number of buffers queued on this source
/// AL_BUFFERS_PROCESSED* i, iv the number of buffers in the queue that have been processed
/// AL_SEC_OFFSET f, fv, i, iv the playback position, expressed in seconds
/// AL_SAMPLE_OFFSET f, fv, i, iv the playback position, expressed in samples
/// AL_BYTE_OFFSET f, fv, i, iv the playback position, expressed in bytes
/// </reference>
namespace AA {

void Speaker::PlayInterrupt() {
  if (isPlaying()) {
    alSourceStop(p_Source);
    local_alErrorCheck();
  }
  alSourcePlay(p_Source);
  local_alErrorCheck();
}

void Speaker::PlayNoOverlap() {
  if (isPlaying())
    return;
  alSourcePlay(p_Source);
  local_alErrorCheck();
}

void Speaker::StopPlay() {
  if (isPlaying()) {
    alSourceStop(p_Source);
    local_alErrorCheck();
  }
}

void Speaker::AssociateBuffer(const ALuint& buffer) {
  if (!p_BufferSet) {
    alSourcei(p_Source, AL_BUFFER, buffer);
    local_alErrorCheck();
    p_BufferSet = true;
    return;
  }
  throw("buffer already set, changing not supported");
}
//
//void Speaker::SetPosition(const float& x, const float& y, const float& z) {
//  SetLocation(x, y, z);
//}
//
//void Speaker::SetLocation(const float& x, const float& y, const float& z) {
//  alSource3f(p_Source, AL_POSITION, x, y, z);
//  local_alErrorCheck();
//}
//
//void Speaker::SetPosition(const glm::vec3& loc)
//{
//  SetLocation(loc);
//}
//
//void Speaker::SetLocation(const glm::vec3& loc)
//{
//  alSource3f(p_Source, AL_POSITION, loc.x, loc.y, loc.z);
//  if (alGetError() != AL_NO_ERROR)
//  {
//    throw("error setting short sound Location");
//  }
//}
//
//void Speaker::SetVelocity(float velocity)
//{
//  alSourcef(p_Source, AL_VELOCITY, velocity);
//  if (alGetError() != AL_NO_ERROR)
//  {
//    throw("error setting short sound velocity");
//  }
//}
//
//void Speaker::SetDirection(const float& x, const float& y, const float& z)
//{
//  alSource3f(p_Source, AL_DIRECTION, x, y, z);
//  if (alGetError() != AL_NO_ERROR)
//  {
//    throw("error setting short sound direction");
//  }
//}
//
//void Speaker::SetDirection(const glm::vec3& dir)
//{
//  SetDirection(dir.x, dir.y, dir.z);
//}
//
//void Speaker::SetVolume(const float gain)
//{
//  float newvol = 0;
//  if (gain < 0.f)
//  {
//    newvol = 0;
//  }
//  else
//  {
//    newvol = gain;
//  }
//
//  alSourcef(p_Source, AL_GAIN, newvol);
//
//  //std::cout << ErrorCheck(alGetError()) << '\n';
//
//  if (alGetError() != AL_NO_ERROR)
//  {
//  	throw("error setting short sound volume");
//  }
//}

//void Speaker::SetLooping(const bool& opt)
//{
//  alSourcei(p_Source, AL_LOOPING, (ALint)opt);
//  if (alGetError() != AL_NO_ERROR)
//  {
//    throw("error setting short sound looping status");
//  }
//}

///// <summary>
///// determines if the positions are relative to the listener
///// </summary>
///// <param name="opt"></param>
//void Speaker::SetRelative(const bool& opt)
//{
//  alSourcei(p_Source, AL_SOURCE_RELATIVE, (ALint)opt);
//  if (alGetError() != AL_NO_ERROR)
//  {
//    throw("error setting short sound relative");
//  }
//}


/// <summary>
/// Sets the volume level (gain) of a speaker.
/// </summary>
/// <param name="gain">overall gain.  1.f is max in most systems. negative values are treated as 0.f</param>
void Speaker::SetVolume(const float gain) {
  alSourcef(p_Source, AL_GAIN, (gain < 0.f) ? 0.f : gain);
  local_alErrorCheck();
}

Speaker::Speaker() {
  //reset();
  float pitch = 1.f;
  float gain = 1.f;
  float position[3] = { 0,0,0 };
  float velocity[3] = { 0,0,0 };
  bool loop = false;

  alGenSources(1, &p_Source);
  local_alErrorCheck();

  alSourcef(p_Source, AL_PITCH, pitch);
  local_alErrorCheck();

  alSourcef(p_Source, AL_GAIN, gain);
  local_alErrorCheck();

  alSource3f(p_Source, AL_POSITION, position[0], position[1], position[2]);
  local_alErrorCheck();

  alSource3f(p_Source, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
  local_alErrorCheck();

  alSourcei(p_Source, AL_LOOPING, (ALint)loop);
  local_alErrorCheck();

  //alSourcei(p_Source, AL_BUFFER, p_LastPlayedBuffer);
}

Speaker::Speaker(const Speaker& old_speaker) {
  p_Source = old_speaker.p_Source;
  p_BufferSet = old_speaker.p_BufferSet;
}

Speaker::~Speaker() {
  StopPlay();
  alDeleteSources(1, &p_Source);
}

Speaker& Speaker::operator = (const Speaker& t) {
  return *this;
}

bool Speaker::isPlaying() {
  ALint play_status;
  alGetSourcei(p_Source, AL_SOURCE_STATE, &play_status);
  local_alErrorCheck();

  return (play_status == AL_PLAYING);
}

} //end namespace AA
