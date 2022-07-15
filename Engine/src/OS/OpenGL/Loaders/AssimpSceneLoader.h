#pragma once
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <assimp/material.h>
#include "Scene/Scene.h"
#include "Cache.h"
#include <string>
#include <vector>
#include <assimp/cimport.h> // for aiLogStream

namespace AA {
class AssimpSceneLoader {
public:
  static void Logging(const bool& on_off, const bool& to_file) noexcept;
  static TextureMapType LoadAllTextures(const aiScene* scene, const aiMaterial* ai_material, const std::string& orig_filepath);
  static unsigned int LoadCubeMapTexture(const std::vector<std::string>& six_texture_paths);
  //static void UnloadTexture(const TextureMapType& textures_to_remove);
  static Material LoadAll(const aiScene* scene, const aiMaterial* ai_material);
  static int Load_MeshesTexturesMaterials(Scene& out_model, const std::string& path_to_load);
  static int Load_MeshesTexturesMaterialsBones(Scene& out_model, const std::string& path_to_load);
  static void Unload(const std::string& path_to_unload);
private:
  static bool doLogging;
};
}