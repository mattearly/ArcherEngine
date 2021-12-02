#include "Camera.h"
#include "../DefaultShaders.h"
#include <glm\ext\matrix_clip_space.hpp>
#include <glm\gtx\transform.hpp>
namespace AA {

#define UP glm::vec3(0,1,0)

Camera::Camera(int width, int height) {
  resetViewportVars();
  updateProjectionMatrix(width, height);
}

void Camera::updateCameraVectors() {
  glm::vec3 front{};
  front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  front.y = sin(glm::radians(Pitch));
  front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(front);
  Right = glm::normalize(glm::cross(Front, UP));
  Up = glm::normalize(glm::cross(Right, Front));
  updateViewMatrix();
}

void Camera::resetViewportVars() {
  Position = glm::vec3(0.f);
  FOV = 45.f;
  Yaw = -90.f;  // look down the -z axis
  Pitch = 0.f;
  MaxRenderDistance = 3000.f;
  mProjectionType = ProjectionType::PERSPECTIVE;
  updateCameraVectors();
}

// updates the cached projection
void Camera::updateProjectionMatrix(int width, int height) {
  Width = width;
  Height = height;
  if (Width < 1 || Height < 1) return;
  switch (mProjectionType) {
  case ProjectionType::PERSPECTIVE:
  {
    float aspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
    mProjectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.0167f, MaxRenderDistance);
  }
  break;
  case ProjectionType::ORTHO:
  {
    // we are going to use an aspect corrected view
    float aspect_ratio = (float)Width * Height;
    float ortho_height = (float)Height / 2.f;   // or ortho size
    float ortho_width = Width * ortho_height;
    mProjectionMatrix = glm::ortho(
      -ortho_width,
      ortho_width,
      -ortho_height,
      ortho_height,
      0.0167f,
      MaxRenderDistance
    );
  }
  break;
  default:
    break;
  }
  mProjectionChanged = true;
}

// update cached view matrix
// you need to update the shader somewhere else
void Camera::updateViewMatrix() {
  mViewMatrix = glm::lookAt(Position, Position + Front, Up);
  mViewChanged = true;
}

void Camera::shaderTick() {
  if (mProjectionChanged) {
    DefaultShaders::get_ubershader()->SetMat4("u_projection_matrix", mProjectionMatrix);
  }
  if (mViewChanged) {
    DefaultShaders::get_ubershader()->SetMat4("u_view_matrix", mViewMatrix);
  }
  mProjectionChanged = mViewChanged = false;
}

}  // end namespace AA