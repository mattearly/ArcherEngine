//   The purpose of this singleton class is to keep track of the resources we have uploaded
// via OpenGL to the graphics card while supporting re-use were possible.

#pragma once
#include <forward_list>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../../Mesh/TextureTypes.h"
#include "../../../Mesh/Material.h"
#include "../../../Mesh/MeshInfo.h"
#include "../../../Mesh/Skeleton.h"

namespace AA {

/// <summary>
/// Holds info for reusing models that are already loaded.
/// </summary>
struct SceneInfo {
  std::vector<MeshInfo> meshes;  // vao and texture refs
  std::string path = "";  // load path and Unique Identifier Key
  int ref_count = 1;
  Skeleton* skelly = nullptr;
  glm::mat4 inverse_transform = glm::mat4(1);

  friend bool operator==(const SceneInfo& lhs, const SceneInfo& rhs);

  friend bool operator!=(const SceneInfo& lhs, const SceneInfo& rhs);
};

/// <summary>
/// Holds models and textures that are already loaded.
/// </summary>

class Cache {
public:
  static [[nodiscard]] Cache* Get();

  // scene
  int try_load_from_cache(std::vector<MeshInfo>& out_meshes, const std::string& path);

  // scene with skeleton
  int try_load_from_cache(std::vector<MeshInfo>& out_meshes, Skeleton& out_skel, glm::mat4& inv_trans, const std::string& path_key);

  // already loaded textures
  int try_load_from_cache(TextureMapType& out_textures, const std::string& path);

  void remove_scene_at_path(const std::string& path_to_remove);

  void add(const SceneInfo& si);
  void add(const Texture2DInfo& ti);
  void add(const TextureMapType& list);

  void remove_sceneinfo(const SceneInfo& matching);
  void remove_texture2dinfo(const Texture2DInfo& matching);

private:

  std::forward_list<SceneInfo> all_loaded_scenes_{};
  std::forward_list<Texture2DInfo> all_loaded_2d_textures_{};

  Cache() = default;
};
}
