#pragma once
#include "../Base/UniqueInstance.h"
#include <AL\al.h>
#include <string>
namespace AA {
/// <summary>
/// does not support .mp3, most other common file types are fine.
/// </summary>
class SoundEffect : public UniqueInstance {
public:
  SoundEffect() = delete;
  SoundEffect(const char* filename);
  ~SoundEffect();
  ALuint _Buffer;
  std::string _FilePath;
private:
};
}  // namespace AA
