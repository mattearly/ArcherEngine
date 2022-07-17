#pragma once
#include <string>
#include <unordered_map>
namespace AA {

enum class TextureType : int { UNKNOWN = 0, ALBEDO, SPECULAR, NORMAL, EMISSION };

struct Texture2DInfo {
  Texture2DInfo() = default;
  unsigned int accessId = 0;  // id to access it on the video mem (drawId)
  TextureType textureType = TextureType::UNKNOWN;
  std::string path = "";
  int ref_count = 1;
};

typedef std::vector<Texture2DInfo> TextureMapType;

static inline std::string toString(const TextureType& t) {
  switch (t) {
  case TextureType::ALBEDO: return std::string("Albedo");
  case TextureType::EMISSION: return std::string("Emission");
  case TextureType::NORMAL: return std::string("Normal");
  case TextureType::SPECULAR: return std::string("Specular");
  case TextureType::UNKNOWN: return std::string("Unknown");
  default: return std::string("Undefined");
  }
  
}

}