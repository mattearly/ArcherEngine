#include "Spacial3D.h"
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>

namespace AA {

Spacial3D::Spacial3D() /*: mRigidBody() */{
  mCurrentLocation = glm::vec3(0);
  mCurrentRot = glm::vec3(0);
  mCurrentScale = glm::vec3(1);
  mFinalModelMatrix = glm::mat4(1);
  has_unprocessed_modifications = true;
}

void Spacial3D::MoveTo(const glm::vec3& location) {
  mCurrentLocation = location;
  has_unprocessed_modifications = true;
}

void Spacial3D::ScaleTo(const glm::vec3& scale) {
  mCurrentScale = scale;
  has_unprocessed_modifications = true;
}

// rotates on respective xyz axii by radian amounts given in rot
void Spacial3D::RotateTo(const glm::vec3& rot) {
  mCurrentRot = rot;
  has_unprocessed_modifications = true;
}

void Spacial3D::ProcessModifications() {
  if (has_unprocessed_modifications) {
    mFinalModelMatrix = glm::mat4(1);
    mFinalModelMatrix = glm::translate(mFinalModelMatrix, mCurrentLocation);
    mFinalModelMatrix = glm::scale(mFinalModelMatrix, mCurrentScale);

    // best order to avoid gimble lock the majority of the time https://youtu.be/zc8b2Jo7mno?t=449
    mFinalModelMatrix = glm::rotate(mFinalModelMatrix, mCurrentRot.y, glm::vec3(0, 1, 0));
    mFinalModelMatrix = glm::rotate(mFinalModelMatrix, mCurrentRot.z, glm::vec3(0, 0, 1));
    mFinalModelMatrix = glm::rotate(mFinalModelMatrix, mCurrentRot.x, glm::vec3(1, 0, 0));

    has_unprocessed_modifications = false;
  }
}

}