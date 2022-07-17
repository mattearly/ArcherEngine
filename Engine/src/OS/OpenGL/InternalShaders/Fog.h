#pragma once
#include "../OGLShader.h"

namespace AA {
namespace InternalShaders {
class Fog {

public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
  static bool IsActive();
private:
  Fog() = delete;
  Fog(Fog&) = delete;
  Fog(const Fog&) = delete;
  Fog(Fog&&) = delete;
  Fog(const Fog&&) = delete;
};

} // end namespace InternalShaders
} // end namespace AA

