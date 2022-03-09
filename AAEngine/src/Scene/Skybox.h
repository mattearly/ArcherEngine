#pragma once
#include "../../include/AAEngine/Scene/Camera.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/MeshInfo.h"
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
  void Render(const std::shared_ptr<Camera>& cam);
private:
  void SetViewMatrix(glm::mat4 view_mat);
  void SetProjectionMatrix(glm::mat4 proj_mat);
  unsigned int mCubemapTexId = 0;
  void setup_shader();
  void setup_cube_geometry();
  void setup_incoming_textures(std::vector<std::string>& incomingSkymapFiles);
};
}
