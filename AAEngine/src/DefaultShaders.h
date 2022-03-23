#pragma once
#include <string>

#include "OS/OpenGL/OGLShader.h"

namespace AA {

class DefaultShaders {
public:
  static void init_ubershader();
  static OGLShader* get_ubershader();

  static void init_stencilshader();
  static OGLShader* get_stencilshader();

private:
  DefaultShaders() = delete;
  DefaultShaders(DefaultShaders&) = delete;
  DefaultShaders(const DefaultShaders&) = delete;
  DefaultShaders(DefaultShaders&&) = delete;
  DefaultShaders(const DefaultShaders&&) = delete;


};

}  // end namespace AA