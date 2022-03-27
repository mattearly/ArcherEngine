#pragma once
#include "../OGLShader.h"
namespace AA {
namespace InternalShaders {
class Uber final {
public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
private:
  Uber() = delete;
  Uber(Uber&) = delete;
  Uber(const Uber&) = delete;
  Uber(Uber&&) = delete;
  Uber(const Uber&&) = delete;
};
}  // end namespace InternalShaders
}  // end namespace AA

