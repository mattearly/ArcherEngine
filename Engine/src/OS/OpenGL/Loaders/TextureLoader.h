#pragma once
#include "Cache.h"
#include <assimp/material.h>
#include <assimp/texture.h>
#include <assimp/scene.h>
#include <unordered_map>
#include <forward_list>
#include <string>

namespace AA {


// pretty specific to opengl currently
class TextureLoader {
public:

  static TextureMapType LoadAllTextures(const aiScene* scene, const aiMaterial* ai_material, const std::string& orig_filepath);

  static unsigned int LoadCubeMapTexture(const std::vector<std::string>& six_texture_paths);

  static void UnloadTexture(const TextureMapType& textures_to_remove);

private:

  TextureLoader() = delete;

};

}// end namespace AA
