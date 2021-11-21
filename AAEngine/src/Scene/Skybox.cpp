#include "Skybox.h"
#include "../../include/AncientArcher/Utility/Files.h"
#include "../Renderer/OpenGL/OGLGraphics.h"
#include "../Renderer/LoadCube.h"
#include "../Renderer/TextureLoader.h"
#include "../DefaultShaders.h"
#ifdef _DEBUG
#include <iostream>
#endif
namespace AA {

static OGLShader* mSkyboxShader = NULL;  // setup once
static unsigned int mVAO = 0;  // setup once via load_cube operation
static const unsigned int mNumElements = 36;

Skybox::Skybox(std::vector<std::string> incomingSkymapFiles) {
  setup_shader();
  setup_cube_geometry();
  setup_incoming_textures(incomingSkymapFiles);

  mSkyboxShader->Use();
  mSkyboxShader->SetInt("skybox", 0);
}

void Skybox::Render(const std::shared_ptr<Camera>& cam) {
  SetViewMatrix(cam->mViewMatrix);
  SetProjectionMatrix(cam->mProjectionMatrix);  // todo (matt): unhack. not set this every frame but only when it changes
  OGLGraphics::SetDepthTest(true); // should usually already be true
  OGLGraphics::SetDepthMode(GL_LEQUAL);
  OGLGraphics::SetSamplerCube(0, mCubemapTexId);
  OGLGraphics::SetCullFace(false);
  OGLGraphics::RenderElements(mVAO, mNumElements);
}

void Skybox::SetProjectionMatrix(glm::mat4 proj_mat) {
  mSkyboxShader->Use();
  mSkyboxShader->SetMat4("projection", proj_mat);
}

void Skybox::SetViewMatrix(glm::mat4 view_mat) {
  // turn the last column into 0's
  const glm::mat4 viewMatrix = glm::mat4(glm::mat3(view_mat));
  mSkyboxShader->Use();
  mSkyboxShader->SetMat4("view", viewMatrix);
}

const std::string frag_skycube = R"(
#version 430 core
in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube skybox;
void main() {
  FragColor = texture(skybox, TexCoords);
}
)";

const std::string vert_skycube = R"(
#version 430 core
layout(location = 0) in vec3 aPos;
out vec3 TexCoords;
uniform mat4 projection;
uniform mat4 view;
void main(){
  TexCoords = aPos;
  vec4 pos = projection * view * vec4(aPos, 1.0);
  gl_Position = pos.xyww;
}
)";

void Skybox::setup_shader() {
  if (!mSkyboxShader) {
    mSkyboxShader = new OGLShader(
      vert_skycube.c_str(),
      frag_skycube.c_str()
    );
  }
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