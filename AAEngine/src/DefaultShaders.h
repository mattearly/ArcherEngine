#pragma once
#include <string>

#include "OS/OpenGL/OGLShader.h"

namespace AA {

class DefaultShaders {
public:
  static void init_ubershader();
  static OGLShader* get_ubershader();

private:
  DefaultShaders() = delete;
};

}  // end namespace AA