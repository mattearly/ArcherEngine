#include "../../include/AAEngine/Scene/Camera.h"
#include "../DefaultShaders.h"
#include <glm\ext\matrix_clip_space.hpp>
#include <glm\gtx\transform.hpp>
namespace AA {

#define UP glm::vec3(0,1,0)

Camera::Camera(int width, int height) {
  resetViewportVars();
  Width = width;
  Height = height;
  updateProjectionMatrix();
}

void Camera::SetBottomLeft(int x, int y)
{
  BottomLeft = glm::vec2(x, y);
}

void Camera::SetMaxRenderDistance(float amt)
{
  if (amt < 0.f)
    amt = abs(amt);
  MaxRenderDistance = amt;
  updateProjectionMatrix();
}

void Camera::SetToPerspective()
{
  mProjectionType = ProjectionType::PERSPECTIVE;
  updateProjectionMatrix();
}

void Camera::SetToOrtho_testing()
{
  mProjectionType = ProjectionType::ORTHO;
  updateProjectionMatrix();
}

void Camera::SetFOV(float new_fov)
{
  if (new_fov < 1.f) new_fov = 1.f;
  if (new_fov > 360.f) new_fov = 360.f;
  FOV = new_fov;
  updateProjectionMatrix();
}

void Camera::SetDimensions_testing(int w, int h)
{
  if (w < 1 || h < 1)
    throw("invalid cam dimensions");
  Width = w;
  Height = h;
  updateProjectionMatrix();
}

void Camera::SetPosition(glm::vec3 new_loc)
{
  Position = new_loc;
  updateCameraVectors();
}

void Camera::SetPitch(float new_pitch_degrees)
{
  if (new_pitch_degrees > 89.9f)
    new_pitch_degrees = 89.9f;
  else if (new_pitch_degrees < -89.9f)
    new_pitch_degrees = -89.9f;
  Pitch = new_pitch_degrees;
  updateCameraVectors();
}

void Camera::SetYaw(float new_yaw_degrees)
{
  if (new_yaw_degrees > 360.0f)
    new_yaw_degrees -= 360.f;
  else if (new_yaw_degrees < 0.f)
    new_yaw_degrees += 360.f;
  Yaw = new_yaw_degrees;
  updateCameraVectors();
}

void Camera::ShiftPosition(glm::vec3 offset)
{
  Position += offset;
  updateCameraVectors();
}

void Camera::ShiftPitchAndYaw(double pitch_offset_degrees, double yaw_offset_degrees)
{
  double new_pitch_degrees = Pitch + pitch_offset_degrees;
  if (new_pitch_degrees > 89.9f)
    new_pitch_degrees = 89.9f;
  else if (new_pitch_degrees < -89.9f)
    new_pitch_degrees = -89.9f;
  Pitch = static_cast<float>(new_pitch_degrees);

  double new_yaw_degrees = Yaw + yaw_offset_degrees;
  if (new_yaw_degrees > 360.0f)
    new_yaw_degrees -= 360.f;
  else if (new_yaw_degrees < 0.f)
    new_yaw_degrees += 360.f;
  Yaw = static_cast<float>(new_yaw_degrees);

  updateCameraVectors();
}

void Camera::SetKeepCameraToWindowSize(bool tf)
{
  isAlwaysScreenSize = tf;
}

bool Camera::GetIsAlwaysScreenSize() {
  return isAlwaysScreenSize;
}

glm::vec3 Camera::GetFront()
{
  return Front;
}

glm::vec3 Camera::GetRight()
{
  return Right;
}

glm::vec3 Camera::GetPosition()
{
  return Position;
}

float Camera::GetPitch()
{
  return Pitch;
}

float Camera::GetYaw()
{
  return Yaw;
}

glm::vec2 Camera::GetPitchAndYaw()
{
  return glm::vec2(Pitch, Yaw);
}

glm::mat4 Camera::GetProjectionMatrix()
{
  return mProjectionMatrix;
}

glm::mat4 Camera::GetViewMatrix()
{
  return mViewMatrix;
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
  BottomLeft = glm::vec2(0, 0);
  Position = glm::vec3(0.f);
  FOV = 45.f;
  Yaw = -90.f;  // look down the -z axis
  Pitch = 0.f;
  MaxRenderDistance = 3000.f;
  mProjectionType = ProjectionType::PERSPECTIVE;
  isAlwaysScreenSize = false;
  updateCameraVectors();
}

// updates the cached projection
void Camera::updateProjectionMatrix() {
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
    DefaultShaders::get_stencilshader()->SetMat4("u_projection_matrix", mProjectionMatrix);
  }
  if (mViewChanged) {
    DefaultShaders::get_ubershader()->SetMat4("u_view_matrix", mViewMatrix);
    DefaultShaders::get_stencilshader()->SetMat4("u_view_matrix", mViewMatrix);
  }
  mProjectionChanged = mViewChanged = false;
}

}  // end namespace AA