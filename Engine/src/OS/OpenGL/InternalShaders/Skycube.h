#pragma once
#include "../OGLShader.h"
namespace AA {
namespace InternalShaders {
class Skycube final {
public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
  static bool IsActive();
private:
  Skycube() = delete;
  Skycube(Skycube&) = delete;
  Skycube(const Skycube&) = delete;
  Skycube(Skycube&&) = delete;
  Skycube(const Skycube&&) = delete;
};
}  // end namespace InternalShaders
}  // end namespace AA
