#pragma once
#include "Cache.h"
#include "MaterialLoader.h"
#include <Scene/Scene.h>
#include <vector>
#include <forward_list>
#include <assimp/scene.h>

namespace AA {

// SceneLoader is pretty specific to OpenGL currently
class SceneLoader {
public:
  static int Load_MeshesTexturesMaterials(Scene& out_model, const std::string& path_to_load);
  static int Load_MeshesTexturesMaterialsBones(Scene& out_model, const std::string& path_to_load);
  static void Unload(const std::string& path_to_unload);
private:
  SceneLoader() = delete;
};

} // end namespace AA