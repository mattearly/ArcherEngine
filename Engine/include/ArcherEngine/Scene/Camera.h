#pragma once
#include "../../../src/Scene/Skybox.h"
#include "../../../src/Base/UniqueInstance.h"
#include <glm/glm.hpp>
#include "Viewport.h"
#include <vector>
#include <memory>
#include <string>

namespace AA {
class Skybox;
enum class ProjectionType { ORTHO, PERSPECTIVE };
class Camera : public UniqueInstance {
public:
  Camera(int width, int height);
  ~Camera();

  
  void ProjectionChanged();

  /// <summary>
  /// Sets where the viewport starts starting from the bottom left.
  /// </summary>
  /// <param name="x">x location</param>
  /// <param name="y">y location</param>
  void SetBottomLeft(const int& x, const int& y);

  /// <summary>
  /// Set the Render Distance on a camera.
  /// </summary>
  /// <param name="amt">a positive value to set the render distance to. negatives will be treated as a positive via absolute value.</param>
  void SetMaxRenderDistance(const float& amt);

  /// <summary>
  /// Change camera to perspective view mode.
  /// </summary>
  void SetToPerspective();

  /// <summary>
  /// Change camera to orthographic mode.
  /// Note that this doesn't work as expect sometimes and needs work.
  /// </summary>
  void SetToOrtho_testing();

  /// <summary>
  /// Changes the field of view of a camera.
  /// </summary>
  /// <param name="new_fov">positive value field of view to set. Bound between 1 and 360.</param>
  void SetFOV(float new_fov);

  /// <summary>
  /// Sets the camera width and height.
  /// <param name="w">desired width</param>
  /// <param name="h">desired height</param>
  void SetDimensions(int w, int h);

  /// <summary>
  /// Moves the camera to the coordinates
  /// </summary>
  /// <param name="new_loc">desired location</param>
  void SetPosition(glm::vec3 new_loc);

  /// <summary>
  /// Set the pitch of a camera
  /// </summary>
  /// <param name="new_pitch_degrees">pitch in degrees</param>
  void SetPitch(float new_pitch_degrees);

  /// <summary>
  /// Set the yaw of a camera
  /// </summary>
  /// <param name="new_yaw_degrees">yaw in degrees</param>
  void SetYaw(float new_yaw_degrees);

  /// <summary>
  /// Moves the camera to an offset of its current location
  /// </summary>
  /// <param name="offset">offset to apply</param>
  void ShiftPosition(glm::vec3 offset);

  /// <summary>
  /// Rotates the Camera by an offset.
  /// </summary>
  /// <param name="pitch_offset"></param>
  /// <param name="yaw_offset"></param>
  void ShiftPitchAndYaw(double pitch_offset, double yaw_offset);

  /// <summary>
  /// set to true to make the camera auto-snap its viewport to the size of the window
  /// </summary>
  /// <param name="tf">true or false</param>
  void SetKeepCameraToWindowSize(bool tf);

  /// <summary>
  /// Sets the render order.
  /// </summary>
  /// <param name="new_depth">depth of the cam</param>
  void SetRenderDepth(int new_depth);

  /// <summary>
  /// Sets a skybox with 6 textures
  /// </summary>
  /// <param name="incomingSkymapFiles">6 textures</param>
  /// <param name="has_alpha">true is images have and alpha channel, false otherwise</param>
  void SetSkybox(std::vector<std::string> incomingSkymapFiles) noexcept;

  /// <summary>
  /// Removes current skybox. You will see the clear screen color instead.
  /// Call SetSkybox with new parameters to set up the skybox again.
  /// </summary>
  void RemoveSkybox() noexcept;

  [[nodiscard]] const Skybox* GetSkybox() const;

  /// <summary>
  /// gets the render order number
  /// </summary>
  /// <returns>render depth variable</returns>
  const int GetRenderDepth() const;

  const float GetRenderDistance() const;

  /// <summary>
  /// Gets the snapping current option
  /// </summary>
  /// <returns>true or false</returns>
  const bool GetIsAlwaysScreenSize() const;

  /// <summary>
  /// Gets a copy of the front vec3
  /// </summary>
  /// <returns>front vec3 copy</returns>
  const glm::vec3 GetFront() const;

  /// <summary>
  /// Gets a copy of the right vec3
  /// </summary>
  /// <returns>right vec3 copy</returns>
  const glm::vec3 GetRight() const;

  /// <summary>
  /// Gets a copy of the cam position vec3
  /// </summary>
  /// <returns>location vec3 copy</returns>
  const glm::vec3 GetPosition() const;

  /// <summary>
  /// Get a copy of the cam pitch.
  /// </summary>
  /// <returns>camera pitch copy</returns>
  const float GetPitch() const;

  /// <summary>
  /// Get a copy of the camera yaw
  /// </summary>
  /// <returns>camera yaw copy</returns>
  const float GetYaw() const;

  /// <summary>
  /// Get a vec2 of the pitch and yaw.
  /// </summary>
  /// <returns>vec2 pitch,yaw copy</returns>
  const glm::vec2 GetPitchAndYaw() const;

  /// <summary>
  /// Get the projection matrix of a camera
  /// </summary>
  /// <returns>mat4 copy of the projection</returns>
  const glm::mat4 GetProjectionMatrix() const;

  /// <summary>
  /// Get the view matrix of a camera
  /// </summary>
  /// <returns>mat4 copy of the view</returns>
  const glm::mat4 GetViewMatrix() const;

  const Viewport GetViewport() const;

  /// <summary>
  /// Sets camera to default init values.
  /// </summary>
  void ResetToDefault();

  /// <summary>
  /// Call this once per frame before using this camera's values for rendering.
  /// </summary>
  void NewFrame();


protected:
  Viewport         mViewport;

  glm::vec3        Front;
  glm::vec3        Right;
  glm::vec3        Up;
  glm::vec3        Position;

  glm::mat4        mViewMatrix;

  float            FOV;
  float            Yaw;
  float            Pitch;
  float            MaxRenderDistance;
  
  glm::mat4        mProjectionMatrix;
  
  ProjectionType   mProjectionType;
  
  bool             isAlwaysScreenSize;
  int              RenderDepth;  // lower renders first

  std::unique_ptr<Skybox> mSkybox;

private:

  bool camera_projection_changed;
  void update_cached_projection_matrix();

  bool camera_vectors_changed;
  void update_camera_vectors_tick();
  void update_cached_view_matrix();

};
} // end namespace AA
