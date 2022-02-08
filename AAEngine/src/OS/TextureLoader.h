#pragma once
#include "TextureInfo.h"
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

  static std::unordered_map<unsigned int, std::string> LoadAllTextures(const aiScene* scene, const aiMaterial* ai_material, const std::string& orig_filepath);

  static void increment_given_texture_ids(const std::unordered_map<uint32_t, std::string>& list);

  static unsigned int LoadTexture(const std::string& texture_path);

  static unsigned int LoadCubeMapTexture(const std::vector<std::string>& six_texture_paths, bool has_alpha = true);

  static void UnloadTexture(const std::unordered_map<unsigned int, std::string>& texture_draw_ids);

  static int loadMaterialTextures(const aiScene* scn, const aiMaterial* mat, aiTextureType type, std::string typeName, std::string orginalFilePath, std::unordered_map<unsigned int, std::string>& out_texInfo);

private:

  TextureLoader() = delete;

};

}// end namespace AA
