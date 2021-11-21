#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <map>
namespace AA {

Animation::Animation() = default;

Animation::Animation(const std::string& animationPath, std::shared_ptr<AnimProp> anim_prop) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
  assert(scene && scene->mRootNode);
  // todo: consider loading other animations than just the first one
  auto animation = scene->mAnimations[0];
  m_Duration = static_cast<float>(animation->mDuration);
  m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
  ReadHeirarchyData(m_RootNode, scene->mRootNode);
  ReadMissingBones(animation, anim_prop);
}

Animation::~Animation() {}

float Animation::GetTicksPerSecond() { return m_TicksPerSecond; }

float Animation::GetDuration() { return m_Duration; }

const AssimpNodeData& Animation::GetRootNode() { return m_RootNode; }

void Animation::ReadMissingBones(const aiAnimation* animation, std::shared_ptr<AnimProp> anim_prop) {
  int size = animation->mNumChannels;

  //reading channels(bones engaged in an animation and their keyframes)
  for (int i = 0; i < size; i++) {
    auto channel = animation->mChannels[i];
    std::string boneName = channel->mNodeName.data;

    if (anim_prop->m_Skeleton.m_BoneInfoMap.find(boneName) == anim_prop->m_Skeleton.m_BoneInfoMap.end()) {
      anim_prop->m_Skeleton.m_BoneInfoMap[boneName].id = anim_prop->m_Skeleton.m_BoneCounter;
      anim_prop->m_Skeleton.m_BoneCounter++;
    }
    m_Skeleton.m_Bones.emplace_back(channel->mNodeName.data, anim_prop->m_Skeleton.m_BoneInfoMap[channel->mNodeName.data].id, channel);
  }

  //m_Skeleton.m_BoneInfoMap.merge(anim_prop->m_Skeleton.m_BoneInfoMap);   // merge was added in c++17
  m_Skeleton.m_BoneInfoMap = anim_prop->m_Skeleton.m_BoneInfoMap;
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