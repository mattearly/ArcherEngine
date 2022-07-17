#pragma once
#include "Animation.h"
#include <string>
#include <map>
#include <memory>


namespace AA {

class Animation;

struct AnimationNodeTree;

class Scene;

class Animator {

public:

  Animator() = delete;

  Animator(std::shared_ptr<Animation> anim, glm::mat4 inv_trans);

  ~Animator();

  void UpdateAnimation(float dt);

  void CalculateBoneTransform(const AnimationNodeTree* node, glm::mat4 parentTransform);

  std::vector<glm::mat4> GetFinalBoneMatrices() const;

private:

  std::vector<glm::mat4> m_FinalBoneMatrices;

  std::shared_ptr<Animation> m_CurrentAnimation;

  float m_CurrentTime;

  float m_DeltaTime;

  glm::mat4 cached_global_inverse_transform;

};

}
