#pragma once
#include "../OGLShader.h"
namespace AA {
namespace InternalShaders {
class Basic final {
public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
  static bool IsActive();
private:
  Basic() = delete;
  Basic(Basic&) = delete;
  Basic(const Basic&) = delete;
  Basic(Basic&&) = delete;
  Basic(const Basic&&) = delete;
};
}  // end namespace InternalShaders
}  // end namespace AA
