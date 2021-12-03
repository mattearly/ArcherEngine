#include "AnimProp.h"
#include "Prop.h"
#include "../OS/OpenGL/OGLGraphics.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/MeshLoader.h"
#include "../OS/AnimMeshLoader.h"
#include "../OS/LoadCube.h"
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

}