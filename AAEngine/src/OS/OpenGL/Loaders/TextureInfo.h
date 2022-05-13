#pragma once
#include <string>
namespace AA {
struct TextureInfo {
  uint32_t accessId = 0; // id to access it on the video mem (drawId)
  std::string type = "";
  std::string path = "";
  int ref_count = 1;
};
}