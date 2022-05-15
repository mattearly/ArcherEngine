#include "../../include/AAEngine/Scene/Camera.h"
#include "../OS/OpenGL/InternalShaders/Skycube.h"
#include "../OS/OpenGL/InternalShaders/Stencil.h"
#include "../OS/OpenGL/InternalShaders/Uber.h"
#include "../OS/OpenGL/InternalShaders/Basic.h"

#include <glm\ext\matrix_clip_space.hpp>
#include <glm\gtx\transform.hpp>
namespace AA {

#define UP glm::vec3(0,1,0)

Camera::Camera(int width, int height) {
  ResetToDefault();
  if (width == 0 || height == 0) {
    SetKeepCameraToWindowSize(true);
  } 
  mViewport.Width = width;
  mViewport.Height = height;
  static int LastRenderDepth = 0;
  RenderDepth = LastRenderDepth++;
}

Camera::~Camera() {
  if (mSkybox)
    mSkybox.reset();
}

void Camera::ProjectionChanged() {
  camera_projection_changed = true;
}

void Camera::SetBottomLeft(const int& x, const int& y) {
  mViewport.BottomLeft[0] = x;
  mViewport.BottomLeft[1] = y;
}

void Camera::SetMaxRenderDistance(const float& amt) {
  if (amt < 0.f)
    MaxRenderDistance = abs(amt);
  else
    MaxRenderDistance = amt;
  camera_projection_changed = true;
}

void Camera::SetToPerspective() {
  mProjectionType = ProjectionType::PERSPECTIVE;
  camera_projection_changed = true;
}

void Camera::SetToOrtho_testing() {
  mProjectionType = ProjectionType::ORTHO;
  camera_projection_changed = true;
}

void Camera::SetFOV(float new_fov) {
  if (new_fov < 1.f) new_fov = 1.f;
  if (new_fov > 360.f) new_fov = 360.f;
  FOV = new_fov;
  camera_projection_changed = true;
}

void Camera::SetDimensions(int w, int h) {
  if (w < 1 || h < 1)
    return;
  mViewport.Width = w;
  mViewport.Height = h;
  camera_projection_changed = true;
}

void Camera::SetPosition(glm::vec3 new_loc) {
  Position = new_loc;
  camera_vectors_changed = true;
}

void Camera::SetPitch(float new_pitch_degrees) {
  if (new_pitch_degrees > 89.9f)
    new_pitch_degrees = 89.9f;
  else if (new_pitch_degrees < -89.9f)
    new_pitch_degrees = -89.9f;
  Pitch = new_pitch_degrees;
  camera_vectors_changed = true;
}

void Camera::SetYaw(float new_yaw_degrees) {
  if (new_yaw_degrees > 360.0f)
    new_yaw_degrees -= 360.f;
  else if (new_yaw_degrees < 0.f)
    new_yaw_degrees += 360.f;
  Yaw = new_yaw_degrees;
  camera_vectors_changed = true;
}

void Camera::ShiftPosition(glm::vec3 offset) {
  Position += offset;
  camera_vectors_changed = true;
}

void Camera::ShiftPitchAndYaw(double pitch_offset_degrees, double yaw_offset_degrees) {
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

  camera_vectors_changed = true;
}

void Camera::SetKeepCameraToWindowSize(bool tf) {
  isAlwaysScreenSize = tf;
}

void Camera::SetRenderDepth(int new_depth) {
  RenderDepth = new_depth;
}

void Camera::SetSkybox(std::vector<std::string> incomingSkymapFiles) noexcept {
  if (incomingSkymapFiles.size() != 6)
    return;  // invalid size for a skybox

  if (mSkybox) RemoveSkybox();

  mSkybox = std::make_unique<Skybox>(incomingSkymapFiles);
  camera_projection_changed = true; // to update the skybox shader
}

void Camera::RemoveSkybox() noexcept {
  if (mSkybox)
    mSkybox.reset();
}

const Skybox* Camera::GetSkybox() const {
  return mSkybox.get();
}

const int Camera::GetRenderDepth() const {
  return RenderDepth;
}

const float Camera::GetRenderDistance() const {
  return MaxRenderDistance;
}

const bool Camera::GetIsAlwaysScreenSize() const {
  return isAlwaysScreenSize;
}

const glm::vec3 Camera::GetFront() const {
  return Front;
}

const glm::vec3 Camera::GetRight() const {
  return Right;
}

const glm::vec3 Camera::GetPosition() const {
  return Position;
}

const float Camera::GetPitch() const {
  return Pitch;
}

const float Camera::GetYaw() const {
  return Yaw;
}

const glm::vec2 Camera::GetPitchAndYaw() const {
  return glm::vec2(Pitch, Yaw);
}

const glm::mat4 Camera::GetProjectionMatrix() const {
  return mProjectionMatrix;
}

const glm::mat4 Camera::GetViewMatrix() const {
  return mViewMatrix;
}

const Viewport Camera::GetViewport() const {
  return mViewport;
}

void Camera::ResetToDefault() {
  mViewport.BottomLeft[0] = mViewport.BottomLeft[1] = 0;
  Position = glm::vec3(0.f);
  FOV = 45.f;
  Yaw = -90.f;  // look down the -z axis
  Pitch = 0.f;
  MaxRenderDistance = 3000.f;
  mProjectionType = ProjectionType::PERSPECTIVE;
  isAlwaysScreenSize = false;
  camera_vectors_changed = true;
  camera_projection_changed = true;
}

void Camera::NewFrame() {
  if (camera_vectors_changed) {
    update_camera_vectors_tick();
    if (InternalShaders::Uber::IsActive()) {
      InternalShaders::Uber::Get()->SetVec3("u_view_pos", Position);
      InternalShaders::Uber::Get()->SetMat4("u_view_matrix", mViewMatrix);
    }
    if (InternalShaders::Stencil::IsActive()) InternalShaders::Stencil::Get()->SetMat4("u_view_matrix", mViewMatrix);
    if (InternalShaders::Skycube::IsActive()) InternalShaders::Skycube::Get()->SetMat4("u_view_matrix", glm::mat4(glm::mat3(mViewMatrix)));
    if (InternalShaders::Basic::IsActive()) InternalShaders::Basic::Get()->SetMat4("u_view_matrix", glm::mat4(mViewMatrix));
    camera_vectors_changed = false;
  }
  if (camera_projection_changed) {
    update_cached_projection_matrix();
    if (InternalShaders::Uber::IsActive()) InternalShaders::Uber::Get()->SetMat4("u_projection_matrix",  mProjectionMatrix);
    if (InternalShaders::Stencil::IsActive()) InternalShaders::Stencil::Get()->SetMat4("u_projection_matrix", mProjectionMatrix);
    if (InternalShaders::Skycube::IsActive()) InternalShaders::Skycube::Get()->SetMat4("u_projection_matrix", mProjectionMatrix);
    if (InternalShaders::Basic::IsActive()) InternalShaders::Basic::Get()->SetMat4("u_projection_matrix", mProjectionMatrix);
    camera_projection_changed = false;
  }
}

void Camera::update_camera_vectors_tick() {
  glm::vec3 front{};
  front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  front.y = sin(glm::radians(Pitch));
  front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(front);
  Right = glm::normalize(glm::cross(Front, UP));
  Up = glm::normalize(glm::cross(Right, Front));
  update_cached_view_matrix();
}

// updates the cached projection
void Camera::update_cached_projection_matrix() {
  float w = static_cast<float>(mViewport.Width);
  float h = static_cast<float>(mViewport.Height);
  if (w < 1.f || h < 1.f)
    return;
  switch (mProjectionType) {
  case ProjectionType::PERSPECTIVE:
  {
    float aspectRatio = w / h;
    mProjectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.0167f, MaxRenderDistance);
  }
  break;
  case ProjectionType::ORTHO:
  {
    // we are going to use an aspect corrected view
    float aspect_ratio = w * h;
    float ortho_height = h / 2.f;   // or ortho size
    float ortho_width = w * ortho_height;
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
}

// update cached view matrix
// you need to update the shader somewhere else
void Camera::update_cached_view_matrix() {
  mViewMatrix = glm::lookAt(Position, Position + Front, Up);
}

}  // end namespace AA