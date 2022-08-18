#pragma once
#include "../OS/OpenGL/OGLShader.h"
#include <vector>
#include <string>
#include <memory>
namespace AA {
// sticking with this pattern: 
// https://learnopengl.com/Advanced-OpenGL/Cubemaps
// although many skybox texture sets found online seem to be in differing order
// you may need to adapt accordingly.
class Skybox {
public:
  Skybox(std::vector<std::string> incomingSkymapFiles);
  ~Skybox();
  const unsigned int GetCubeMapTexureID() const;
  const unsigned int GetVAO() const;
private:
  unsigned int mCubemapTexId = 0;
  unsigned int mVAO = 0;  // setup once via setup_cube_geometry operation
  void setup_cube_geometry();
  void setup_incoming_textures(std::vector<std::string>& incomingSkymapFiles);
};
}
