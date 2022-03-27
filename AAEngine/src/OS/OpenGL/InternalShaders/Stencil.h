#pragma once
#include "../OGLShader.h"
namespace AA {
namespace InternalShaders {
class Stencil final {
public:
  static void Init();
  static OGLShader* Get();
  static void Shutdown();
private:
  Stencil() = delete;
  Stencil(Stencil&) = delete;
  Stencil(const Stencil&) = delete;
  Stencil(Stencil&&) = delete;
  Stencil(const Stencil&&) = delete;
};
}  // end namespace InternalShaders
}  // end namespace AA