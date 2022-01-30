#pragma once
#include "../Base/UniqueInstance.h"
#include <glm/glm.hpp>

namespace AA {
enum class ProjectionType { ORTHO, PERSPECTIVE };
class Camera : public UniqueInstance {
public:
  Camera(int width, int height);

  /// <summary>
  /// Set the Render Distance on a camera.
  /// </summary>
  /// <param name="amt">a positive value to set the render distance to. negatives will be treated as a positive via absolute value.</param>
  void SetMaxRenderDistance(float amt);

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
  void SetDimensions_testing(int w, int h);

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
  /// Gets a copy of the front vec3
  /// </summary>
  /// <returns>front vec3 copy</returns>
  glm::vec3 GetFront();

  /// <summary>
  /// Gets a copy of the right vec3
  /// </summary>
  /// <returns>right vec3 copy</returns>
  glm::vec3 GetRight();

  /// <summary>
  /// Gets a copy of the cam position vec3
  /// </summary>
  /// <returns>location vec3 copy</returns>
  glm::vec3 GetPosition();

  /// <summary>
  /// Get a copy of the cam pitch.
  /// </summary>
  /// <returns>camera pitch copy</returns>
  float GetPitch();

  /// <summary>
  /// Get a copy of the camera yaw
  /// </summary>
  /// <returns>camera yaw copy</returns>
  float GetYaw();

  /// <summary>
  /// Get a vec2 of the pitch and yaw.
  /// </summary>
  /// <returns>vec2 pitch,yaw copy</returns>
  glm::vec2 GetPitchAndYaw();

  /// <summary>
  /// Get the projection matrix of a camera
  /// </summary>
  /// <returns>mat4 copy of the projection</returns>
  glm::mat4 GetProjectionMatrix();

  /// <summary>
  /// Get the view matrix of a camera
  /// </summary>
  /// <returns>mat4 copy of the view</returns>
  glm::mat4 GetViewMatrix();

protected:
  int              Width;
  int              Height;
  glm::vec3        Front;
  glm::vec3        Right;
  glm::vec3        Up;
  glm::vec3        Position;
  float            FOV;
  float            Yaw;
  float            Pitch;
  float            MaxRenderDistance;
  glm::mat4        mProjectionMatrix;
  glm::mat4        mViewMatrix;
  ProjectionType   mProjectionType;

private:
  void resetViewportVars();
  bool mViewChanged;
  bool mProjectionChanged;

  // auto managed by InterfaceRuntime
  friend class Interface;
  void updateCameraVectors();
  void updateProjectionMatrix();
  void updateViewMatrix();
  virtual void shaderTick();
};
} // end namespace AA
