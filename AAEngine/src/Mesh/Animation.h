#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>  
#include <iostream>
#include <vector>
#include <map>
#include "Bone.h"
#include "AnimProp.h"
#include "../Utility/Conversions.h"
#include "../OS/Vertex.h"
#include "../Base/UniqueInstance.h"
#include "Skeleton.h"

namespace AA {

struct AssimpNodeData {
  glm::mat4 transformation;
  std::string name;
  int childrenCount;
  std::vector<AssimpNodeData> children;
};

class AnimProp;

class Animation : public UniqueInstance {

public:

  Animation();

  Animation(const std::string& animationPath, std::shared_ptr<AnimProp> anim_prop);

  ~Animation();

  float GetTicksPerSecond();

  float GetDuration();

  const AssimpNodeData& GetRootNode();

  void ReadMissingBones(const aiAnimation* animation, std::shared_ptr<AnimProp> anim_prop);

  void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

  Skeleton m_Skeleton;

private:

  float m_Duration;

  float m_TicksPerSecond;
  
  AssimpNodeData m_RootNode;
  
};

}
