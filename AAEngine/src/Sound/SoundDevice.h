#pragma once
#include <vector>
#include <string>
#include <AL\alc.h>

namespace AA {

/// <summary>
/// By calling get() the first time, SoundDevice sets up the sound device and context for the device.
/// </summary>
class SoundDevice {
public:
  static SoundDevice* Get();
  static void Init();
  static void Init(const char* devicename);

  void SwitchDevice(const char* devicename);

  void PopulateDeviceVec(std::vector<std::string>& devicesVec);



  void SuspendContext();
  void ResumeContext();

protected:
  SoundDevice();
  SoundDevice(const char* devicename);
  ~SoundDevice();

private:


  ALCdevice* p_ALCDevice;
  ALCcontext* p_ALCContext;

};


}  //end namespace AA