#pragma once
#include "../Base/UniqueInstance.h"
#include <vector>
#include <AL\al.h>
#include <glm/glm.hpp>

namespace AA {
/// <summary>
/// Source of playback for a buffer.
/// </summary>
class Speaker : public UniqueInstance {
public:
  void PlayInterrupt();
  void PlayNoOverlap();
  void StopPlay();
  void AssociateBuffer(const ALuint& buffer);

  //void SetPosition(const float& x, const float& y, const float& z);
  //void SetPosition(const glm::vec3& loc);
  //void SetLocation(const float& x, const float& y, const float& z);
  //void SetLocation(const glm::vec3& loc);

  //void SetVelocity(float velocity);

  //void SetDirection(const float& x, const float& y, const float& z);
  //void SetDirection(const glm::vec3& dir);

  void SetVolume(const float gain);

  //void SetLooping(const bool& opt);
  //void SetRelative(const bool& opt);

  Speaker();
  Speaker(const Speaker& old_speaker); // copy
  Speaker& operator = (const Speaker& t);
  ~Speaker();

private:
  bool   isPlaying();
  bool   p_BufferSet = false;
  ALuint p_Source = 0;
};

} // end namespace AA
