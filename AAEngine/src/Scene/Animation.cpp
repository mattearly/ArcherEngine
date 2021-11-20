#include "Animation.h"
#include <map>
namespace AA {

Animation::Animation() = default;

Animation::Animation(const std::string& animationPath) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
  assert(scene && scene->mRootNode);
  // todo: consider loading other animations than just the first one
  auto animation = scene->mAnimations[0];
  m_Duration = static_cast<float>(animation->mDuration);
  m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
  ReadHeirarchyData(m_RootNode, scene->mRootNode);
  //ReadMissingBones(animation, *model);
  m_BoneCounter = 0;
  ReadBones(animation);
}

Animation::~Animation() {}

Bone* Animation::FindBone(const std::string& name) {
  auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
    [&](const Bone& Bone)
  {
    return Bone.GetBoneName() == name;
  }
  );
  if (iter == m_Bones.end()) return nullptr;
  else return &(*iter);
}

float Animation::GetTicksPerSecond() { return m_TicksPerSecond; }

float Animation::GetDuration() { return m_Duration; }

const AssimpNodeData& Animation::GetRootNode() { return m_RootNode; }

const std::map<std::string, BoneInfo>& Animation::GetBoneIDMap() {
  return m_BoneInfoMap;
}


//void Animation::ReadMissingBones(const aiAnimation* animation, AnimProp& model) {
//  int size = animation->mNumChannels;
//
//  //reading channels(bones engaged in an animation and their keyframes)
//  for (int i = 0; i < size; i++) {
//    auto channel = animation->mChannels[i];
//    std::string boneName = channel->mNodeName.data;
//
//    if (model.m_BoneInfoMap.find(boneName) == model.m_BoneInfoMap.end()) {
//      model.m_BoneInfoMap[boneName].id = model.m_BoneCounter;
//      model.m_BoneCounter++;
//    }
//    m_Bones.emplace_back(channel->mNodeName.data, model.m_BoneInfoMap[channel->mNodeName.data].id, channel);
//  }
//
//  m_BoneInfoMap.merge(model.m_BoneInfoMap);   // merge was added in c++17
//}


void Animation::ReadBones(const aiAnimation* animation) {
  unsigned int size = animation->mNumChannels;
  for (unsigned int i = 0; i < size; i++) {
    auto channel = animation->mChannels[i];
    std::string boneName = channel->mNodeName.data;
    if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {  // bone not already on list
      m_BoneInfoMap[boneName].id = m_BoneCounter;
      m_BoneCounter++;
    }
    m_Bones.emplace_back(boneName, m_BoneInfoMap[boneName].id, channel);
  }
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
  assert(src);

  dest.name = src->mName.data;
  dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
  dest.childrenCount = src->mNumChildren;

  for (unsigned int i = 0; i < src->mNumChildren; i++) {
    AssimpNodeData newData;
    ReadHeirarchyData(newData, src->mChildren[i]);
    dest.children.push_back(newData);
  }
}


}