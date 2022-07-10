#pragma once
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <vector>
#include <assimp/material.h>
#include "Cache.h"
#include "Scene/Scene.h"

namespace AA {
class AssimpSceneLoader {
public:
  static TextureMapType LoadAllTextures(const aiScene* scene, const aiMaterial* ai_material, const std::string& orig_filepath);
  static unsigned int LoadCubeMapTexture(const std::vector<std::string>& six_texture_paths);
  //static void UnloadTexture(const TextureMapType& textures_to_remove);
  static Material LoadAll(const aiScene* scene, const aiMaterial* ai_material);
  static int Load_MeshesTexturesMaterials(Scene& out_model, const std::string& path_to_load);
  static int Load_MeshesTexturesMaterialsBones(Scene& out_model, const std::string& path_to_load);
  static void Unload(const std::string& path_to_unload);
private:

};

}