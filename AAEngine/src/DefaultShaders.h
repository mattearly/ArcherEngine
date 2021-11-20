#pragma once
#include <string>

#include "Renderer/OpenGL/OGLShader.h"

namespace AA {

class DefaultShaders {
public:
  static void init_diffuse_3d();
  static void init_phong_3d();
  static void init_skel_3d();
  static OGLShader* get_diffuse_3d();
  static OGLShader* get_phong_3d();
  static OGLShader* get_skel_3d();

private:
  DefaultShaders() = delete;
};

}  // end namespace AA