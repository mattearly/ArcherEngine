#include "SoundListener.h"
#include "SoundDevice.h"

namespace AA {
SoundListener* SoundListener::Get() {
  // make sure sound device is initialized
  SoundDevice::Init();

  SoundListener* snd_listener = new SoundListener();
  return snd_listener;
}

void SoundListener::SetMasterGain(const float& gain) {
  // consider gain clamps
  float newVol = gain;
  if (newVol < 0.f) {
    newVol = 0.f;
  } else if (newVol > 5.f) {
    // now thats flippin loud, lets cap it
    newVol = 5.f;
  }

  alListenerf(AL_GAIN, newVol);

  if (alGetError() != AL_NO_ERROR) {
    throw("error setting gain");
  }
}

void SoundListener::SetPosition(const glm::vec3& pos) {
  SetLocation(pos);
}

void SoundListener::SetLocation(const glm::vec3& pos) {
  alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
  if (alGetError() != AL_NO_ERROR) {
    throw("error setting listener location");
  }
}

void SoundListener::SetPosition(const float& x, const float& y, const float& z) {
  SetLocation(x, y, z);
}

void SoundListener::SetLocation(const float& x, const float& y, const float& z) {
  alListener3f(AL_POSITION, x, y, z);
  if (alGetError() != AL_NO_ERROR) {
    throw("error setting listener location");
  }
}

void SoundListener::SetOrientation(const glm::vec3& at, const glm::vec3& up) {
  std::vector<float> ori;
  ori.push_back(at.x);
  ori.push_back(at.y);
  ori.push_back(at.z);
  ori.push_back(up.x);
  ori.push_back(up.y);
  ori.push_back(up.z);
  alListenerfv(AL_ORIENTATION, ori.data());
  if (alGetError() != AL_NO_ERROR) {
    throw("error setting gain");
  }
}

void SoundListener::SetOrientation(const float& atx, const float& aty, const float& atz, const float& upx, const float& upy, const float& upz) {
  float ori[6] = { atx, aty, atz, upx, upy, upz };
  alListenerfv(AL_ORIENTATION, &ori[0]);
  if (alGetError() != AL_NO_ERROR) {
    throw("error setting gain");
  }
}

/// <summary>
/// AL_INVERSE_DISTANCE, AL_INVERSE_DISTANCE_CLAMPED, AL_LINEAR_DISTANCE,
/// AL_LINEAR_DISTANCE_CLAMPED, AL_EXPONENT_DISTANCE,
/// AL_EXPONENT_DISTANCE_CLAMPED, or AL_NONE.
/// </summary>
/// <param name="textureType">option</param>
void SoundListener::SetDistanceModel(ALint textureType) {
  switch (textureType) {
  case AL_INVERSE_DISTANCE:
  case AL_INVERSE_DISTANCE_CLAMPED:
  case AL_LINEAR_DISTANCE:
  case AL_LINEAR_DISTANCE_CLAMPED:
  case AL_EXPONENT_DISTANCE:
  case AL_EXPONENT_DISTANCE_CLAMPED:
  case AL_NONE:
    break;
  default:
    throw("invalid distance model");
  }
  alDistanceModel(textureType);
  if (alGetError() != AL_NO_ERROR) {
    throw("error setting listener distance model");
  }
}

SoundListener::SoundListener() {}



}  // end namespace AA
