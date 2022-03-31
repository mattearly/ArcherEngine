#pragma once
#include "../Base/UniqueInstance.h"
#include "../../../src/Mesh/Spacial3D.h"
#include "../../../src/OS/MeshInfo.h"
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

  std::vector<MeshInfo> mMeshes;
protected:

  bool stenciled;
  glm::vec3 stencil_color;
  bool stenciled_with_normals;
  float stencil_scale;

  Spacial3D spacial_data;


  std::string cached_load_path;

  friend class Interface;
  friend class MeshLoader;

};
}  // end namespace AA
