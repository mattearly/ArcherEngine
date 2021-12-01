#include "AnimProp.h"
#include "Prop.h"
#include "../Scene/Camera.h"
#include "../Renderer/OpenGL/OGLGraphics.h"
#include "../Renderer/OpenGL/OGLShader.h"
#include "../Renderer/MeshLoader.h"
#include "../Renderer/AnimMeshLoader.h"
#include "../Renderer/LoadCube.h"
#include "../Utility/Conversions.h"
#include "../DefaultShaders.h"
namespace AA {

AnimProp::AnimProp(const std::string& path) : Prop() {
  if (mMeshes.empty()) {
    AnimMeshLoader::LoadGameObjectFromFile(*this, path);
  }
}

void AnimProp::RemoveCache() {
  if (!mMeshes.empty()) {
    AnimMeshLoader::UnloadGameObject(mMeshes);
    mMeshes.clear();
    mAnimator.reset();
  }
}

void AnimProp::Load(const std::string& path) {
  if (mMeshes.empty()) {
    AnimMeshLoader::LoadGameObjectFromFile(*this, path);
  }
}

void AnimProp::UpdateAnim(float dt) {
  if (mAnimator) {
    mAnimator->UpdateAnimation(dt);
  }
}

void AnimProp::SetAnimator(std::shared_ptr<Animation> anim) {
  if (mAnimator)
    mAnimator.reset();

  mAnimator = std::make_unique<Animator>(anim, glm::mat4(1), *this);
}

void AnimProp::Draw(const std::shared_ptr<Camera>& cam) {
  OGLShader* shader = nullptr;
  shader = DefaultShaders::get_ubershader();
  shader->Use();
  shader->SetMat4("u_projection_matrix", cam->mProjectionMatrix);
  shader->SetMat4("u_view_matrix", cam->mViewMatrix);
  shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);
  shader->SetBool("hasAlbedo", false);
  shader->SetBool("hasSpecular", false);
  shader->SetBool("hasNormal", false);
  shader->SetBool("hasEmission", false);

  for (MeshInfo& m : mMeshes) {
    for (const auto& texture : m.textureDrawIds) {
      const std::string texType = texture.second;  // get the texture type
      if (texType == "Albedo") {
        shader->SetBool("hasAlbedo", true);
        shader->SetInt(("material." + texType).c_str(), 0);
        OGLGraphics::SetTexture(0, texture.first);
      }
      if (texType == "Specular") {
        shader->SetBool("hasSpecular", true);
        shader->SetInt(("material." + texType).c_str(), 1);
        OGLGraphics::SetTexture(1, texture.first);
      }
      if (texType == "Normal") {
        shader->SetBool("hasNormal", true);
        shader->SetInt(("material." + texType).c_str(), 2);
        OGLGraphics::SetTexture(2, texture.first);
      }
      if (texType == "Emission") {
        shader->SetBool("hasEmission", true);
        shader->SetInt(("material." + texType).c_str(), 3);
        OGLGraphics::SetTexture(3, texture.first);
      }
    }
    OGLGraphics::SetCullFace(m.backface_culled);
    OGLGraphics::RenderElements(m.vao, m.numElements);
  }
}

}