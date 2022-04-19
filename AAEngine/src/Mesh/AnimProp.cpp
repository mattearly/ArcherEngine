#include "../../include/AAEngine/Mesh/AnimProp.h"
#include "../../include/AAEngine/Mesh/Prop.h"
#include "../OS/OpenGL/OGLGraphics.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/MeshLoader.h"
#include "../OS/AnimMeshLoader.h"
#include "../Math/Conversions.h"
namespace AA {

AnimProp::AnimProp(const std::string& path) : Prop() {
  Load(path);
}

void AnimProp::RemoveCache() {
  if (!mMeshes.empty()) {
    AnimMeshLoader::UnloadGameObject(mMeshes, cached_load_path);
    mMeshes.clear();
    mAnimator.reset();
  }
}

void AnimProp::Load(const std::string& path) {
  if (mMeshes.empty()) {
    if (AnimMeshLoader::LoadGameObjectFromFile(*this, path) == -1) {
      throw(std::_Xruntime_error, "failed to load path");
    }
    cached_load_path = path;
  } else {
    throw(std::_Xruntime_error, "Meshes are loaded already. Remove cache first.");
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

}