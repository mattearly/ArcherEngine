#include "Skybox.h"
#include "../../include/AAEngine/Utility/Files.h"
#include "../OS/OpenGL/Graphics.h"
#include "../OS/OpenGL/InternalShaders/Skycube.h"
#include "../OS/OpenGL/Loaders/TextureLoader.h"
namespace AA {

Skybox::Skybox(std::vector<std::string> incomingSkymapFiles) {
  setup_cube_geometry();
  setup_incoming_textures(incomingSkymapFiles);
  InternalShaders::Skycube::Get()->SetInt("u_skybox", 0);
}

Skybox::~Skybox() {
  if (mCubemapTexId != 0) {
    OpenGL::DeleteTex(1u, mCubemapTexId);  // todo: cache textures and reuse
  }
}

const unsigned int Skybox::GetCubeMapTexureID() const {
  return mCubemapTexId;
}

const unsigned int Skybox::GetVAO() const {
  return mVAO;
}

void Skybox::setup_cube_geometry() {
  if (mVAO == 0) {
    mVAO = OpenGL::Primitives::load_cube();
  }
}

void Skybox::setup_incoming_textures(std::vector<std::string>& incomingSkymapFiles) {
  mCubemapTexId = TextureLoader::LoadCubeMapTexture(incomingSkymapFiles);     // todo: load up debug box if skymap files are wrong
}

}  // end namespace AA