#pragma once
#include "Prop.h"
#include "../../../src/Mesh/Animator.h"
#include "../../../src/Mesh/Skeleton.h"
#include <vector>
#include <memory>
#include <string>

namespace AA {

class Animator;
class Animation;

/// <summary>
/// AKA Skeletal Mesh
/// </summary>
class AnimProp : public Prop {
public:
  AnimProp(const std::string& path);


  void UpdateAnim(float dt);

  void SetAnimator(std::shared_ptr<Animation> anim);

  glm::mat4 mGlobalInverseTransform;

  std::unique_ptr<Animator> mAnimator;

  Skeleton m_Skeleton;

protected:

  friend class AnimModelLoader;


  friend class Interface;
  virtual void RemoveCache() override;
  virtual void Load(const std::string& path) override;
};

}