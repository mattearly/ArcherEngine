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
  std::vector<MeshInfo> meshes;
  std::string path = "";
  int ref_count = 1;
};

/// <summary>
/// Holds info for reusing models that are already loaded.
/// </summary>
struct AnimSceneInfo : public SceneInfo {
  std::string path = "";
  Skeleton skelly;
  int ref_count = 1;
};

/// <summary>
/// Holds models and textures that are loaded onto graphics memory.
/// </summary>
class Cache {
public:
  static [[nodiscard]] Cache* Instance();

  // normal scene
  int try_load_from_cache(std::vector<MeshInfo>& out_meshes, const std::string& path);
  void remove_scene_at_path(const std::string& path_to_remove);

  // animated scene
  int try_load_from_cache(std::vector<MeshInfo>& out_meshes, Skeleton& skel, const std::string& path);
  void remove_anim_scene_at_path(const std::string& path_to_remove);

  int try_load_from_cache(TextureMapType& out_textures, const std::string& path);
  void remove_textures(const TextureMapType& to_remove);

  void add(const SceneInfo& si);
  void add(const AnimSceneInfo& asi);
  void add(const Texture2DInfo& ti);
  void add(const TextureMapType& list);


private:

  std::forward_list<SceneInfo> all_loaded_scenes_{};
  std::forward_list<Texture2DInfo> all_loaded_2d_textures_{};
  std::forward_list<AnimSceneInfo> all_loaded_animated_scenes_{};

  Cache() = default;
};
}
