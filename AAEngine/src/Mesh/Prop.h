#pragma once
#include "../Base/UniqueInstance.h"
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

  Spacial3D spacial_data;

  std::vector<MeshInfo> mMeshes;

  friend class Interface;
  friend class MeshLoader;

};
}  // end namespace AA
