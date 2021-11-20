#pragma once
#include <string>
#include "Animation.h"
#include "Bone.h"
#include <map>
#include <memory>
namespace AA {

class Bone;


class Animation;

struct AssimpNodeData;

class Animator {
public:
  Animator() = delete;

  Animator(std::shared_ptr<Animation> anim, glm::mat4 inv_trans);

  void UpdateAnimation(float dt);

  //void PlayAnimation(Animation* pAnimation);

  void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

  std::vector<glm::mat4> GetFinalBoneMatrices();

private:
  std::vector<glm::mat4> m_FinalBoneMatrices;
  std::shared_ptr<Animation> m_CurrentAnimation;
  float m_CurrentTime;
  float m_DeltaTime;

  glm::mat4 cached_global_inverse_transform;
};

}