#pragma once
#include "Prop.h"
#include "Animator.h"
#include "../Renderer/MeshInfo.h"
#include "../Base/UniqueInstance.h"
#include "../Scene/Camera.h"
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <PxRigidDynamic.h>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "Skeleton.h"

namespace AA {

class Animator;
class Animation;

/// <summary>
/// AKA Skeletal Mesh
/// </summary>
class AnimProp : public Prop {
public:
  AnimProp(const std::string& path);

  virtual void RemoveCache() override;
  virtual void Load(const std::string& path) override;
  virtual void Draw(const std::shared_ptr<Camera>& cam) override;

  void UpdateAnim(float dt);

  void SetAnimator(std::shared_ptr<Animation> anim);

  glm::mat4 mGlobalInverseTransform;

  std::unique_ptr<Animator> mAnimator;

  Skeleton m_Skeleton;

  friend class AnimMeshLoader;
  friend class AncientArcher;

};

}