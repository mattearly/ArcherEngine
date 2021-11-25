#pragma once
#include <string>

#include "Renderer/OpenGL/OGLShader.h"

namespace AA {

class DefaultShaders {
public:
  static void init_phong_3d();
  static OGLShader* get_phong_3d();

private:
  DefaultShaders() = delete;
};

}  // end namespace AA