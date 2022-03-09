#pragma once
#include "../../include/AAEngine/Base/UniqueInstance.h"
#include "Spacial3D.h"
#include "../OS/MeshInfo.h"
#include <vector>
#include <string>

namespace AA {

/// <summary>
/// AKA Static Mesh
/// </summary>
class Prop : public UniqueInstance {
public:
  Prop();
  Prop(const char* path);

  virtual void Draw();

  virtual void RemoveCache();
  virtual void Load(const std::string& path);

protected:

  bool stenciled;
  glm::vec3 stencil_color;
  bool stenciled_with_normals;
  float stencil_scale;

  Spacial3D spacial_data;

  std::vector<MeshInfo> mMeshes;

  friend class Interface;
  friend class MeshLoader;

};
}  // end namespace AA
