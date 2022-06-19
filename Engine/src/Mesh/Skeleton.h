#pragma once
#include "Bone.h"
#include <vector>
#include <map>
namespace AA{

struct BoneInfo {
  /*id is index in finalBoneMatrices*/
  int id;

  /*offset matrix transforms vertex from model space to bone space*/
  glm::mat4 offset;
};

struct Skeleton {

  Bone* FindBone(const std::string& name);

  const std::map<std::string, BoneInfo>& GetBoneIDMap();

  // ---------------

  std::vector<Bone> m_Bones;

  std::map<std::string, BoneInfo> m_BoneInfoMap;

  unsigned int m_BoneCounter = 0;

};
}
