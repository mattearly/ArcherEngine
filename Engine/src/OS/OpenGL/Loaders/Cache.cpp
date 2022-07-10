#include "Cache.h"
#include "../../../Mesh/Skeleton.h"
namespace AA {

[[nodiscard]] Cache* Cache::Instance() {
  static Cache* cache = new Cache();
  return cache;
}

int Cache::try_load_from_cache(std::vector<MeshInfo>& out_meshes, const std::string& path) {
  for (auto& ref_model_info : all_loaded_scenes_) {
    if (ref_model_info.path == "")
      continue;
    if (path == ref_model_info.path.data()) {
      ref_model_info.ref_count++;
      for (const auto& m : ref_model_info.meshes) {
        add(m.textureDrawIds);
        out_meshes.emplace_back(MeshInfo(m.vao, m.numElements, m.textureDrawIds, m.material, m.local_transform));
      }
      return 1;  // already loaded
    }
  }
  return 0; // not already loaded
}

void Cache::remove_scene_at_path(const std::string& path_to_remove) {
  // step1: decrement loaded count
  for (auto& ref_model_info : all_loaded_scenes_) {
    if (ref_model_info.path == "")
      continue;
    if (path_to_remove == ref_model_info.path.data()) {
      if (ref_model_info.ref_count > 0) {
        ref_model_info.ref_count--;
      }
    }
  }

  // step2: remove if ref count is 0 (or less, somehow)

  // a) if we were using a vector instead of a forward_list 
  //AllLoadedModels.erase(std::remove_if(
  //  AllLoadedModels.begin(), AllLoadedModels.end(),
  //  [](const RefModelInfo& ref) {
  //    return ref.ref_count < 1;
  //  }), AllLoadedModels.end());

  // b) C++20 method that should work on a forward texture_list, but we are using C++17
  //std::erase_if(AllLoadedModels, [](const RefModelInfo& ref) {
  //  return ref.ref_count < 1;
  //  });

  // c) C++17 (and maybe some older) method to remove from foward_list
  auto before = all_loaded_scenes_.before_begin();
  for (auto it = all_loaded_scenes_.begin(); it != all_loaded_scenes_.end(); ) {
    if (it->ref_count < 1) {
      it = all_loaded_scenes_.erase_after(before);
    } else {
      before = it;
      ++it;
    }
  }
}

int Cache::try_load_from_cache(std::vector<MeshInfo>& out_meshes, Skeleton& skel, glm::mat4& inv_trans, const std::string& path_key) {
  for (auto& ref_model_info : all_loaded_scenes_) {
    if (ref_model_info.path == "")
      continue;
    if (path_key == ref_model_info.path.data()) {
      ref_model_info.ref_count++;

      for (const auto& m : ref_model_info.meshes) {
        add(m.textureDrawIds);
        out_meshes.emplace_back(MeshInfo(m.vao, m.numElements, m.textureDrawIds, m.material, m.local_transform));
      }
      skel = ref_model_info.skelly;
      inv_trans = ref_model_info.inverse_transform;
      return true;  // already loaded
    }
  }
  return false; // not already loaded
}
//
int Cache::try_load_from_cache(TextureMapType& out_textures, const std::string& path_key) {
  for (auto& tex_info : all_loaded_2d_textures_) {
    if (tex_info.path == "")
      continue;
    if (path_key == tex_info.path.data()) {
      tex_info.ref_count++;
      out_textures.emplace_back(tex_info);
      return 1;  // already loaded
    }
  }
  return 0; // not already loaded
}

void Cache::add(const SceneInfo& si) {
  for (auto& cached_scene : all_loaded_scenes_) // up ref count if scene exists already
    if (cached_scene.path == si.path) {
      cached_scene.ref_count++;
      return;
    }

  all_loaded_scenes_.push_front(si);
}

void Cache::add(const Texture2DInfo& ti) {
  all_loaded_2d_textures_.push_front(ti);
}

void Cache::add(const TextureMapType& texture_list) {
  for (const auto& tex : texture_list) {
    bool added = false;
    for (auto& t : all_loaded_2d_textures_) {
      if (tex.accessId == t.accessId) {
        t.ref_count++;
        added = true;
        continue;
      }
    }
    if (!added) {
      all_loaded_2d_textures_.push_front(tex);
      added = true;
      continue;
    }
  }
}


}



