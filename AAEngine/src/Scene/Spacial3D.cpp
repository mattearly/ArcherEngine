#include "Spacial3D.h"
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>

namespace AA {

Spacial3D::Spacial3D() {
  curr_loc = glm::vec3(0);
  curr_rot = glm::vec3(0);
  curr_scale = glm::vec3(1);
  mFinalModelMatrix = glm::mat4(1);
  modified = true;
}
void Spacial3D::MoveTo(glm::vec3 location) {
  curr_loc = location;
  modified = true;
}

void Spacial3D::ScaleTo(glm::vec3 scale) {
  curr_scale = scale;
  modified = true;
}

// rotates on respective xyz axii by radian amounts given in rot
void Spacial3D::RotateTo(glm::vec3 rot) {
  curr_rot = rot;
  modified = true;
}
void Spacial3D::ProcessModifications() {
  mFinalModelMatrix = glm::mat4(1);
  mFinalModelMatrix = glm::translate(mFinalModelMatrix, curr_loc);
  mFinalModelMatrix = glm::scale(mFinalModelMatrix, curr_scale);

  // best order to avoid gimble lock the majority of the time https://youtu.be/zc8b2Jo7mno?t=449
  mFinalModelMatrix = glm::rotate(mFinalModelMatrix, curr_rot.y, glm::vec3(0, 1, 0));
  mFinalModelMatrix = glm::rotate(mFinalModelMatrix, curr_rot.z, glm::vec3(0, 0, 1));
  mFinalModelMatrix = glm::rotate(mFinalModelMatrix, curr_rot.x, glm::vec3(1, 0, 0));

  modified = false;
}


}