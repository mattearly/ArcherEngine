#pragma once
#include "../Base/UniqueInstance.h"
#include "../../../src/Mesh/Spacial3D.h"
#include "../../../src/OS/MeshInfo.h"
#include <vector>
#include <string>

namespace AA {

/// <summary>
/// AKA Static Mesh
/// </summary>
class Prop : public UniqueInstance {
public:
  Prop();
  Prop(const char* path);

  /// <summary>
  /// Read only access to mesh data
  /// </summary>
  /// <returns>const mesh vec</returns>
  const std::vector<MeshInfo>& GetMeshes() const;

  const glm::mat4 GetFMM() const;

  const bool IsStenciled() const;

  const bool IsStenciledWithNormals() const;

  const float GetStencilScale() const;

  const glm::vec3 GetStencilColor() const;

  /// <summary>
  /// Moves a prop to a location
  /// </summary>
  /// <param name="loc">desired location</param>
  void SetLocation(const glm::vec3& loc);

  /// <summary>
  /// Scales a prop
  /// </summary>
  /// <param name="scale">desired scale</param>
  void SetScale(const glm::vec3& scale);

  /// <summary>
  /// Rotates a prop
  /// </summary>
  /// <param name="rot">rotation axis values, x, y, z should be radians -PI to PI</param>
  void SetRotation(const glm::vec3& rot);

  /// <summary>
  /// Sets whether the prop should have a stencil around it.
  /// </summary>
  /// <param name="tf">true or false for on or off</param>
  void SetStencil(const bool& tf);

  /// <summary>
  /// Sets the color of the stencil.
  /// </summary>
  /// <param name="color">stencil color in rgb</param>
  void SetStencilColor(const glm::vec3& color);

  /// <summary>
  /// Calculate stencil with Normals or Not. Normals work better for objects that do not have a centered origin.
  /// </summary>
  /// <param name="tf">true to calculate stencil with normal, false to calculate normally. If the stencil is offcentered, try true.</param>
  void SetStencilWithNormals(const bool& tf);

  /// <summary>
  /// Scale of the stencil. These scale differently depending on whether calculated with Normals or not.
  /// </summary>
  /// <param name="scale">scale should be > 1 or you probably won't see the stencil</param>
  void SetStencilScale(const float& scale);

protected:

  // mesh loader helps set all these
  friend class MeshLoader;
  friend class OGLGraphics;
  bool stenciled;
  glm::vec3 stencil_color;
  bool stenciled_with_normals;
  float stencil_scale;

  Spacial3D spacial_data;
  
  std::vector<MeshInfo> mMeshes;

  std::string cached_load_path;

  // only intferace calls draw, removecache, and load
  friend class Interface;
  virtual void RemoveCache();
  virtual void Load(const std::string& path);

};
}  // end namespace AA
