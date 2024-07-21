#include "SoundDevice.h"
#include <AL/al.h>
#include <AL/alext.h>
namespace AA {

static SoundDevice* p_SoundDevice_instance = nullptr;

ALCdevice* p_ALCDevice;

ALCcontext* p_ALCContext;

SoundDevice* SoundDevice::Get() {
  Init(); // uses default device unless device was custom init
  return p_SoundDevice_instance;
}

void SoundDevice::Init() {
  if (!p_SoundDevice_instance) {
    p_SoundDevice_instance = new SoundDevice();
  }
}

void SoundDevice::Init(const char* devicename) {
  if (!p_SoundDevice_instance) {
    p_SoundDevice_instance = new SoundDevice(devicename);
  }
}

void SoundDevice::Shutdown() {
  if (p_SoundDevice_instance) {
    delete p_SoundDevice_instance;
    p_SoundDevice_instance = nullptr;
  }
}

void SoundDevice::SwitchDevice(const char* devicename) {
  //close previous device
  if (!alcMakeContextCurrent(nullptr))
    throw("failed to set context to nullptr");

  alcDestroyContext(p_ALCContext);
  if (p_ALCContext)
    throw("failed to unset during close");

  if (!alcCloseDevice(p_ALCDevice))
    throw("failed to close sound device");

  //open new device
  p_ALCDevice = alcOpenDevice(devicename);
  if (!p_ALCDevice)
    throw("failed to get sound device");

  p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
  if (!p_ALCContext)
    throw("failed to set sound context");

  if (!alcMakeContextCurrent(p_ALCContext))
    throw("failed to make context current");


  //#ifdef _DEBUG
  //  //verbose
  //  const ALCchar* name = NULL;
  //  if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
  //    name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
  //  if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
  //    name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
  //  printf("Opened \"%s\"\n", name);
  //#endif
}

void SoundDevice::PopulateDeviceVec(std::vector<std::string>& devicesVec) {
  const ALCchar* devicenames;
  //devicenames = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER); // get current without windows tag
  devicenames = alcGetString(nullptr, ALC_DEVICE_SPECIFIER); // get all without windows tag
  ////devicenames = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER); // get all with windows tag
  //devicenames = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER); // get current with windows tag

  const char* ptr = devicenames;

  devicesVec.clear();

  do {
    devicesVec.push_back(std::string(ptr));
    ptr += devicesVec.back().size() + 1;
  } while (*(ptr + 1) != '\0');

}

void SoundDevice::SuspendContext() {
  alcSuspendContext(p_ALCContext);
}

void SoundDevice::ResumeContext() {
  alcProcessContext(p_ALCContext);
}

SoundDevice::SoundDevice() {
  p_ALCDevice = alcOpenDevice(nullptr);  // get default device
  if (!p_ALCDevice)
    throw("failed to get sound device");

  p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
  if (!p_ALCContext)
    throw("failed to set sound context");

  if (!alcMakeContextCurrent(p_ALCContext))
    throw("failed to make context current");

  //#ifdef _DEBUG
    //verbose say device name opened
    //const ALCchar* name = NULL;
    //if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
      //name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
    //if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
      //name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
    //printf("Opened \"%s\"\n", name);
  //#endif

    // todo: check for surround sound devices/set options for hrtf https://github.com/kcat/openal-soft/blob/master/examples/alhrtf.c

    //ALCint attri[5];
    //ALC_FREQUENCY
    //	ALC_MONO_SOURCES
    //	ALC_REFRESH
    //	ALC_STEREO_SOURCES
    //ALC_SYNC
    //ALC_TRUE;
    //ALC_HRTF_SOFT;
    //attri[0] = 0;
    //attri[1] = ALC_HRTF_SOFT;
    //p_ALCContext = alcCreateContext(p_ALCDevice, attri);


}

SoundDevice::SoundDevice(const char* devicename) {
  p_ALCDevice = alcOpenDevice(devicename);
  if (!p_ALCDevice)
    throw("failed to get sound device");

  p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
  if (!p_ALCContext)
    throw("failed to set sound context");

  if (!alcMakeContextCurrent(p_ALCContext))
    throw("failed to make context current");

  //
  //#ifdef _DEBUG
  //  //verbose say device name opened
  //  const ALCchar* name = NULL;
  //  if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
  //    name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
  //  if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
  //    name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
  //  printf("Opened \"%s\"\n", name);
  //#endif
}


SoundDevice::~SoundDevice() {
  //if (!alcMakeContextCurrent(nullptr))
  //	throw("failed to set context to nullptr");

  //alcDestroyContext(p_ALCContext);
  //if (p_ALCContext)
  //	throw("failed to unset during close");

  //if (!alcCloseDevice(p_ALCDevice))
  //	throw("failed to close sound device");

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(p_ALCContext);
  alcCloseDevice(p_ALCDevice);
}

} // end namespace AA
