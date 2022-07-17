#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <map>
#include <memory>
#include "Animator.h"
#include "AnimationData.h"

namespace AA {

Animation::Animation() = default;

/// <summary>
/// Loads the first animation in the file (todo: multiload). 
/// Applies Missing Bones to animated prop
/// </summary>
/// <param name="animationPath">path to file</param>
/// <param name="anim_prop">shared pointer to animated prop</param>
Animation::Animation(const std::string& animationPath, Skeleton& in_out_skele) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
  assert(scene && scene->mRootNode);
  auto animation = scene->mAnimations[0];
  m_Duration = static_cast<float>(animation->mDuration);
  m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
  ReadHeirarchyData(m_RootNode, scene->mRootNode);
  ReadMissingBones(animation, in_out_skele);
}

Animation::~Animation() {}

float Animation::GetTicksPerSecond() { return m_TicksPerSecond; }

float Animation::GetDuration() { return m_Duration; }

const AnimationNodeTree& Animation::GetRootNode() { return m_RootNode; }

void Animation::ReadMissingBones(const aiAnimation* animation, Skeleton& in_out_skele) {
  int num_animation_channels = animation->mNumChannels;

  //reading channels(bones engaged in an animation and their keyframes)
  for (int i = 0; i < num_animation_channels; i++) {
    auto channel = animation->mChannels[i];
    std::string boneName = channel->mNodeName.data;

    if (in_out_skele.m_BoneInfoMap.find(boneName) == in_out_skele.m_BoneInfoMap.end()) {
      in_out_skele.m_BoneInfoMap[boneName].id = in_out_skele.m_BoneCounter;
      in_out_skele.m_BoneCounter++;
    }
    m_Skeleton.m_Bones.emplace_back(channel->mNodeName.data, in_out_skele.m_BoneInfoMap[channel->mNodeName.data].id, channel);
  }

  m_Skeleton.m_BoneInfoMap.merge(in_out_skele.m_BoneInfoMap);   // merge was added in c++17
  //m_Skeleton.m_BoneInfoMap = in_out_skele.m_BoneInfoMap;
}

void Animation::ReadHeirarchyData(AnimationNodeTree& dest, const aiNode* src) {
  assert(src);

  dest.name = src->mName.data;
  dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
  dest.childrenCount = src->mNumChildren;

  for (unsigned int i = 0; i < src->mNumChildren; i++) {
    AnimationNodeTree newData;
    ReadHeirarchyData(newData, src->mChildren[i]);
    dest.children.push_back(newData);
  }
}
}