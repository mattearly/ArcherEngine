#include "Skybox.h"
#include "../../include/AAEngine/Utility/Files.h"
#include "../OS/OpenGL/OGLGraphics.h"
#include "../OS/OpenGL/InternalShaders/Skycube.h"
#include "../OS/LoadCube.h"
#include "../OS/TextureLoader.h"
namespace AA {

static unsigned int mVAO = 0;  // setup once via load_cube operation
static const unsigned int mNumElements = 36;

Skybox::Skybox(std::vector<std::string> incomingSkymapFiles) {
  setup_cube_geometry();
  setup_incoming_textures(incomingSkymapFiles);
  InternalShaders::Skycube::Get()->SetInt("u_skybox", 0);
}

Skybox::~Skybox() {
  if (mCubemapTexId != 0) {
    OGLGraphics::DeleteTex(mCubemapTexId);
  }
  InternalShaders::Skycube::Shutdown();
}

void Skybox::Render(const std::shared_ptr<Camera>& cam) {
  SetViewMatrix(cam->GetViewMatrix());
  SetProjectionMatrix(cam->GetProjectionMatrix());  // todo (matt): unhack. not set this every frame but only when it changes
  OGLGraphics::SetDepthTest(true); // should usually already be true
  OGLGraphics::SetDepthMode(GL_LEQUAL);
  OGLGraphics::SetSamplerCube(0, mCubemapTexId);
  OGLGraphics::SetCullFace(false);
  OGLGraphics::RenderElements(mVAO, mNumElements);
}

void Skybox::SetProjectionMatrix(glm::mat4 proj_mat) {
  InternalShaders::Skycube::Get()->SetMat4("u_projection_matrix", proj_mat);
}

void Skybox::SetViewMatrix(glm::mat4 view_mat) {
  // turn the last column into 0's
  const glm::mat4 viewMatrix = glm::mat4(glm::mat3(view_mat));
  InternalShaders::Skycube::Get()->SetMat4("u_view_matrix", viewMatrix);
}

void Skybox::setup_cube_geometry() {
  if (mVAO == 0) {
    mVAO = load_cube();
  }
}

void Skybox::setup_incoming_textures(std::vector<std::string>& incomingSkymapFiles) {
  mCubemapTexId = TextureLoader::LoadCubeMapTexture(incomingSkymapFiles);     // todo: load up debug box if skymap files are wrong
}

}  // end namespace AA