#pragma once
#include "../OGLShader.h"
namespace AA {
namespace InternalShaders {
class Icon final {
public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
  static bool IsActive();
private:
  Icon() = delete;
  Icon(Icon&) = delete;
  Icon(const Icon&) = delete;
  Icon(Icon&&) = delete;
  Icon(const Icon&&) = delete;
};
}  // end namespace InternalShaders
}  // end namespace AA
