#pragma once
#include "../Math/Conversions.h"
#include "../Base/UniqueInstance.h"
#include "Vertex.h"
#include "Bone.h"
#include "Skeleton.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>  
#include <vector>
#include <memory>
#include <map>

namespace AA {

struct AnimationNodeTree {
  glm::mat4 transformation;
  std::string name;
  int childrenCount;
  std::vector<AnimationNodeTree> children;
};

class AnimProp;

struct AnimationData;

class Animation : public UniqueInstance {

public:

  Animation();

  Animation(const std::string& animationPath, Skeleton& in_out_skele);

  ~Animation();

  float GetTicksPerSecond();

  float GetDuration();

  const AnimationNodeTree& GetRootNode();

private:

  void ReadMissingBones(const aiAnimation* animation, Skeleton& in_out_skele);

  void ReadHeirarchyData(AnimationNodeTree& dest, const aiNode* src);

  friend class Animator;
  Skeleton m_Skeleton;

private:

  float m_Duration = 0.0f;

  float m_TicksPerSecond = 0.0f;

  AnimationNodeTree m_RootNode;

};

}
