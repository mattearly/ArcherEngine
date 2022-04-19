#include "../../include/AAEngine/Mesh/Prop.h"
#include "../OS/OpenGL/OGLGraphics.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/OpenGL/InternalShaders/Uber.h"
#include "../OS/OpenGL/InternalShaders/Stencil.h"
#include "../OS/MeshLoader.h"
#include <glm/gtx/transform.hpp>

namespace AA {

Prop::Prop() {
  stenciled = false;
  stencil_color = glm::vec3(0.1f, 0.87f, 0.1f);
  stenciled_with_normals = false;
  stencil_scale = 1.1f;
  cached_load_path.clear();
}

Prop::Prop(const char* path) {
  Load(path);
  stenciled = false;
  stencil_color = glm::vec3(0.1f, 0.87f, 0.1f);
  stenciled_with_normals = false;
  stencil_scale = 1.1f;
}

void Prop::RemoveCache() {
  if (!mMeshes.empty()) {
    MeshLoader::UnloadGameObject(mMeshes, cached_load_path);
    mMeshes.clear();
  }
}

/// <summary>
/// if this doesn't throw, assume load successful
/// </summary>
/// <param name="path">path to model to load and cache</param>
void Prop::Load(const std::string& path) {
  if (!mMeshes.empty())
    throw(std::_Xruntime_error, "Meshes are loaded already. Remove cache first.");

  int load_code = MeshLoader::LoadGameObjectFromFile(*this, path);
  if (load_code == 0) {  // loaded from path
    cached_load_path = path;
  } else if (load_code == 1) {  // reused already loaded cache
    cached_load_path = path;
  } else if (load_code == -1) {
    throw(std::_Xruntime_error, "scene was null");
  } else if (load_code == -2) {
    throw(std::_Xruntime_error, "scene has incomplete flags");
  } else if (load_code == -3) {
    throw(std::_Xruntime_error, "scene has incomplete flags");
  }
}

const std::vector<MeshInfo>& Prop::GetMeshes() const {
  return mMeshes;
}

const glm::mat4 Prop::GetFMM() const {
  return spacial_data.mFinalModelMatrix;
}

const bool Prop::IsStenciled() const {
  return stenciled;
}

const bool Prop::IsStenciledWithNormals() const {
  return stenciled_with_normals;
}

const float Prop::GetStencilScale() const {
  return stencil_scale;
}

const glm::vec3 Prop::GetStencilColor() const {
  return stencil_color;
}

void Prop::SetLocation(const glm::vec3& loc) {
  spacial_data.MoveTo(loc);
}

void Prop::SetScale(const glm::vec3& scale) {
  spacial_data.ScaleTo(scale);
}

void Prop::SetRotation(const glm::vec3& rot) {
  spacial_data.RotateTo(rot);
}

void Prop::SetStencil(const bool& tf) {
  stenciled = tf;
}

void Prop::SetStencilColor(const glm::vec3& color) {
  stencil_color = color;
}

void Prop::SetStencilWithNormals(const bool& tf) {
  stenciled_with_normals = tf;
}

void Prop::SetStencilScale(const float& scale) {
  stencil_scale = scale;
}

}  // end namespace AA