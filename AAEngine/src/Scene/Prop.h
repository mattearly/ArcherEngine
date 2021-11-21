#pragma once
#include "../Renderer/MeshInfo.h"
#include "../Base/UniqueInstance.h"
#include "Camera.h"
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <PxRigidDynamic.h>
#include <vector>
#include <memory>
#include <string>
#include "Spacial3D.h"

namespace AA {

/// <summary>
/// AKA Static Mesh
/// </summary>
class Prop : public UniqueInstance {
public:
  Prop();
  Prop(const char* path);

  virtual void Draw(const std::shared_ptr<Camera>& cam);

  virtual void RemoveCache();
  virtual void Load(const std::string& path);

protected:

  bool mLit;

  Spacial3D spacial_data;

  std::vector<MeshInfo> mMeshes;

  friend class AncientArcher;
  friend class ModelLoader;

};
}  // end namespace AA
