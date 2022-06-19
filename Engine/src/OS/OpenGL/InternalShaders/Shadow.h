#pragma once
#include "../OGLShader.h"
namespace AA {
namespace InternalShaders {
class Shadow final {
public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
  static bool IsActive();
private:
  Shadow() = delete;
  Shadow(Shadow&) = delete;
  Shadow(const Shadow&) = delete;
  Shadow(Shadow&&) = delete;
  Shadow(const Shadow&&) = delete;
};
}  // end namespace InternalShaders
}  // end namespace AA
