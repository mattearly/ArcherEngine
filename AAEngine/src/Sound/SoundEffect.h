#pragma once
#include "../../include/AAEngine/Base/UniqueInstance.h"
#include <AL\al.h>
#include <string>
namespace AA {
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
