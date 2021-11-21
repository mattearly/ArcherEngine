#include "Skeleton.h"
namespace AA {

Bone* Skeleton::FindBone(const std::string& name) {
  auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& Bone) { return Bone.GetBoneName() == name; });
  if (iter == m_Bones.end()) 
    return nullptr;
  else 
    return &(*iter);
}


const std::map<std::string, BoneInfo>& Skeleton::GetBoneIDMap() {
  return m_BoneInfoMap;
}



}