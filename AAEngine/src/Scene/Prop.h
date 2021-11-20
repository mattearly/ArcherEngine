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
namespace AA {
struct Spacial3D {
  Spacial3D();
  glm::vec3 curr_loc;
  glm::vec3 curr_scale;
  glm::vec3 curr_rot;  // expressed as 3 radians that corrispond to xyz rotation amounts
  bool modified;
  void ProcessModifications();
  glm::mat4 mFinalModelMatrix;
  void MoveTo(glm::vec3 location);
  void ScaleTo(glm::vec3 scale);
  void RotateTo(glm::vec3 rot);
};

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
