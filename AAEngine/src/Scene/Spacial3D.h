#pragma once
#include <glm/glm.hpp>
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

}