#pragma once
#include <glm/glm.hpp>
#include <PxPhysics.h>
namespace AA {
class Spacial3D final {
public:
  Spacial3D();
  void MoveTo(glm::vec3 location);
  void ScaleTo(glm::vec3 scale);
  void RotateTo(glm::vec3 rot);
  void ProcessModifications();
  void ApplyPhysx(glm::mat4 changes);
private: 
  physx::PxRigidDynamic* mRigidBody;
  glm::vec3 mCurrentLocation;
  glm::vec3 mCurrentScale;
  glm::vec3 mCurrentRot;  // expressed as 3 radians that corrispond to xyz rotation amounts
  bool has_unprocessed_modifications;
  glm::mat4 mFinalModelMatrix;
  friend class AncientArcher;
  friend class Prop;
};

}