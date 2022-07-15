#include <Scene/Scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <memory>
#include "../Mesh/Animator.h"
#include "../Mesh/AnimationData.h"
#include "../OS/OpenGL/Loaders/AssimpSceneLoader.h"

namespace AA {

struct SceneData {
  bool stenciled{ false };
  glm::vec3 stencil_color{ glm::vec3(0.0f) };
  bool stenciled_with_normals{ false };
  float stencil_scale{ 1.1f };
  bool cull_backface{ true };
  bool render_shadows{ true };
  bool cull_frontface_for_shadows{ true };
  Spacial3D spacial_data{};
  std::vector<MeshInfo> mMeshes{};
  std::string cached_load_path{};
};

Scene::Scene() {
  scenedata_ = std::make_unique<SceneData>();
}

Scene::Scene(const char* path, const bool& do_get_bones) {
  scenedata_ = std::make_unique<SceneData>();
  if (do_get_bones) {
    animdata_ = std::make_unique<AnimationData>();
  }
  Load(path, do_get_bones);
}

Scene::Scene(const char* path) {
  scenedata_ = std::make_unique<SceneData>();
  Load(path, false);
}

Scene::~Scene() {
  if (scenedata_) {
    if (!scenedata_->mMeshes.empty()) {
      AssimpSceneLoader::Unload(scenedata_->cached_load_path);
    }
    scenedata_.reset();
  }
  if (animdata_) {
    animdata_.reset();
  }
}

void Scene::Update() {
  if (scenedata_)
    scenedata_->spacial_data.ProcessModifications();
}

// copy meshes
void Scene::SetMeshes(const std::vector<MeshInfo>& meshes) {
  scenedata_->mMeshes = meshes;
}

void Scene::SetSkeleton(const Skeleton& skel) {
  if (animdata_) {
    animdata_->m_Skeleton = skel;
  }
}

Skeleton* Scene::GetSkeleton() {
  if (!animdata_)
    return nullptr;
  else
    return &animdata_->m_Skeleton;
}

std::vector<glm::mat4> Scene::GetFinalBoneMatrices() const {
  if (HasAnimation())
    return animdata_->mAnimator->GetFinalBoneMatrices();
  else return std::vector<glm::mat4>();
}

const std::vector<MeshInfo>& Scene::GetMeshes() {
  return scenedata_->mMeshes;
}



/// <summary>
/// if this doesn't throw, assume load successful
/// </summary>
/// <param name="path">path to model to load and cache</param>
void Scene::Load(const std::string& path, const bool& getanimdata) {
  if (!scenedata_->mMeshes.empty())
    throw(std::_Xruntime_error, "Meshes are loaded already. Remove cache first.");

  int load_code = 0;

  if (getanimdata) {
    load_code = AssimpSceneLoader::Load_MeshesTexturesMaterialsBones(*this, path);
  } else {
    load_code = AssimpSceneLoader::Load_MeshesTexturesMaterials(*this, path);
  }

  if (load_code == 0) {  // loaded from path
    scenedata_->cached_load_path = path;
  } else if (load_code == 1) {  // reused already loaded cache
    scenedata_->cached_load_path = path;
  } else if (load_code == -1) {
    throw(std::_Xruntime_error, "scene was null");
  } else if (load_code == -2) {
    throw(std::_Xruntime_error, "scene has incomplete flags");
  } else if (load_code == -3) {
    throw(std::_Xruntime_error, "scene has incomplete flags");
  }
}

void Scene::SetPathID(const std::string& path) {
  scenedata_->cached_load_path;
}

const glm::mat4 Scene::GetFMM() const {
  return scenedata_->spacial_data.mFinalModelMatrix;
}

const bool Scene::IsStenciled() const {
  return scenedata_->stenciled;
}

const bool Scene::IsStenciledWithNormals() const {
  return scenedata_->stenciled_with_normals;
}

const bool Scene::IsAnimated() const {
  return (animdata_) ? true : false;
}

const bool Scene::HasAnimation() const {
  if (animdata_)
    if (animdata_->mAnimator)
      return true;

  return false;
}

const float Scene::GetStencilScale() const {
  return scenedata_->stencil_scale;
}

const glm::vec3 Scene::GetStencilColor() const {
  return scenedata_->stencil_color;
}

const bool Scene::GetRenderShadows() const { return scenedata_->render_shadows; }

const bool Scene::GetCullFrontFaceForShadows() const {
  return scenedata_->cull_frontface_for_shadows;
}

const bool Scene::GetBackFaceCull() const {
  return scenedata_->cull_backface;
}

const glm::vec3& Scene::GetLocation() const {
  return scenedata_->spacial_data.mCurrentLocation;
}

void Scene::SetLocation(const glm::vec3& loc) {
  scenedata_->spacial_data.MoveTo(loc);
}

void Scene::SetScale(const glm::vec3& scale) {
  scenedata_->spacial_data.ScaleTo(scale);
}

void Scene::SetRotation(const glm::vec3& rot) {
  scenedata_->spacial_data.RotateTo(rot);
}

void Scene::SetStencil(const bool& tf) {
  scenedata_->stenciled = tf;
}

void Scene::SetStencilColor(const glm::vec3& color) {
  scenedata_->stencil_color = color;
}

void Scene::SetStencilWithNormals(const bool& tf) {
  scenedata_->stenciled_with_normals = tf;
}

void Scene::SetStencilScale(const float& scale) {
  scenedata_->stencil_scale = scale;
}

void Scene::SetRenderShadows(const bool tf) {
  scenedata_->render_shadows = tf;
}

void Scene::SetFrontFaceCullForShadows(const bool tf) {
  scenedata_->cull_frontface_for_shadows = tf;
}

void Scene::SetBackfaceCull(const bool tf) {
  scenedata_->cull_backface = tf;
}

void Scene::UpdateAnim(float dt) {
  if (animdata_) {
    if (animdata_->mAnimator)
      animdata_->mAnimator->UpdateAnimation(dt);
  }
}

void Scene::ClearAnimator() {
  if (animdata_)
    if (animdata_->mAnimator)
      animdata_->mAnimator.reset();
}

void Scene::SetAnimator(std::shared_ptr<Animation>& anim) {
  if (!animdata_)
    return;
  animdata_->mAnimator = std::make_unique<Animator>(anim, animdata_->mGlobalInverseTransform);

}

void Scene::SetGlobalInverseTransform(const glm::mat4& inv_trans) {
  if (animdata_) {
    animdata_->mGlobalInverseTransform = inv_trans;
  }
}

const std::vector<unsigned int> Scene::GetVAOList() noexcept {
  std::vector<unsigned int> ret_list;

  ret_list.reserve(scenedata_->mMeshes.size());
  for (const auto& mesh : scenedata_->mMeshes) {
    ret_list.push_back(mesh.vao);
  }

  return ret_list;
}

}  // end namespace AA