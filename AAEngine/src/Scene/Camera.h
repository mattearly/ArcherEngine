#pragma once
#include "../Base/UniqueInstance.h"
#include <glm/glm.hpp>
#include <memory>

namespace AA {
enum class ProjectionType { ORTHO, PERSPECTIVE };
class Camera : public UniqueInstance {
public:
  Camera(int width, int height);
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
  void updateCameraVectors();
  void updateProjectionMatrix(int width, int height);
  void updateViewMatrix();
  void changeProjection(ProjectionType new_type);
private:
  void resetViewportVars();
};
} // end namespace AA
