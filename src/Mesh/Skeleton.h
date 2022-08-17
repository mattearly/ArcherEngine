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

typedef const std::string BONE_MAP_KEY;

struct Skeleton {

  Bone* FindBone(const std::string& name);

  const std::map<BONE_MAP_KEY, BoneInfo>& GetBoneIDMap();

  std::vector<Bone> m_Bones;

  std::map<BONE_MAP_KEY, BoneInfo> m_BoneInfoMap;

  unsigned int m_BoneCounter = 0;

};
}
