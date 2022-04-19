#include "Animator.h"
namespace AA {

Animator::Animator(std::shared_ptr<Animation> anim, glm::mat4 inv_trans, AnimProp& anim_prop) {
  cached_global_inverse_transform = inv_trans;
  m_CurrentTime = 0.0;
  m_CurrentAnimation = anim;

  m_FinalBoneMatrices.reserve(100);

  for (int i = 0; i < 100; i++)
    m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

Animator::~Animator() {
  m_FinalBoneMatrices.clear();
  m_CurrentAnimation.reset();
}

void Animator::UpdateAnimation(float dt) {
  m_DeltaTime = dt;
  if (m_CurrentAnimation) {
    m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
    m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
    CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
  }
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
  std::string nodeName = node->name;
  glm::mat4 nodeTransform = node->transformation;

  Bone* Bone = m_CurrentAnimation->m_Skeleton.FindBone(nodeName);

  if (Bone) {
    Bone->Update(m_CurrentTime);
    nodeTransform = Bone->GetLocalTransform();
  }

  glm::mat4 accum_transform = parentTransform * nodeTransform;

  auto boneInfoMap = m_CurrentAnimation->m_Skeleton.GetBoneIDMap();

  if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {  // if bone found
    int index = boneInfoMap[nodeName].id;
    glm::mat4 offset = boneInfoMap[nodeName].offset;
    m_FinalBoneMatrices[index] =  cached_global_inverse_transform * accum_transform * offset;
  }

  for (int i = 0; i < node->childrenCount; i++)
    CalculateBoneTransform(&node->children[i], accum_transform);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices() {
  return m_FinalBoneMatrices;
}

}