#pragma once
#include <glm/glm.hpp>
#include "Animator.h"
#include <memory>
#include "Skeleton.h"
namespace AA {
class Animator;
struct AnimationData {
  glm::mat4 mGlobalInverseTransform;
  std::unique_ptr<Animator> mAnimator;
  Skeleton m_Skeleton;
};
}