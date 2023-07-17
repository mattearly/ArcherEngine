#pragma once
#include "../../../src/Base/UniqueInstance.h"
#include "../../../src/Mesh/Spacial3D.h"
#include "../../../src/Mesh/MeshInfo.h"
#include "../../../src/Mesh/Skeleton.h"
#include "../../../src/OS/OpenGL/Loaders/AssimpSceneLoader.h"
#include <vector>
#include <string>
#include <memory>

namespace AA {

class Spacial3D;

class UniqueInstance;

class Animation;

struct SceneData;

struct AnimationData;

/// <summary>
/// AKA Static Mesh
/// </summary>
class Scene : public UniqueInstance {
public:
  Scene();
  Scene(const char* path, const bool& get_bones);
  Scene(const char* path);
  ~Scene();

  const glm::mat4 GetFMM() const;

  const bool IsStenciled() const;

  const bool IsStenciledWithNormals() const;

  const bool IsAnimated() const;
  
  const bool HasAnimation() const;

  const float GetStencilScale() const;

  const glm::vec3 GetStencilColor() const;

  const bool GetRenderShadows() const;

  const bool GetCullFrontFaceForShadows() const;

  const bool GetBackFaceCull() const;

  const glm::vec3& GetLocation() const;

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

  void SetRenderShadows(const bool tf);

  void SetFrontFaceCullForShadows(const bool tf);

  void SetBackfaceCull(const bool tf);

  void UpdateAnim(float dt);

  const std::vector<unsigned int> GetVAOList() noexcept;

private:

  // mesh loader helps set all these
  friend class AssimpSceneLoader;
  friend class Interface;
  friend class OpenGL;

  void Update();
  const std::vector<MeshInfo>& GetMeshes();

  void ClearAnimator();
  void SetAnimator(std::shared_ptr<Animation>& anim);
  void SetGlobalInverseTransform(const glm::mat4& inv_trans);
  void SetMeshes(const std::vector<MeshInfo>& meshes);
  void SetPathID(const std::string& path);
  void SetSkeleton(const Skeleton& skel);
  Skeleton* GetSkeleton();
  std::vector<glm::mat4> GetFinalBoneMatrices() const;
  
  void Load(const std::string& path, const bool& getanimdata);

  std::unique_ptr<SceneData> scenedata_;
  std::unique_ptr<AnimationData> animdata_;
};
}  // end namespace AA
