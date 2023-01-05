#include "AssimpSceneLoader.h"
#include "../Graphics.h"
#include <assimp/Importer.hpp>
#include <string>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION

// png, jpegs, and bmp only

//#define STBI_NO_JPEG
//#define STBI_NO_PNG
//#define STBI_NO_BMP
#define STBI_NO_PSD
//#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM  (.ppm and .pgm)

#define STBI_WINDOWS_UTF8

#define STBI_NO_FAILURE_STRINGS

//#define STBI_ONLY_PNG

//#define STBI_ASSERT(x)

//#define STBI_MALLOC malloc
//#define STBI_REALLOC realloc
//#define STBI_FREE free

#ifdef __linux__
#include <stb/stb_image.h>
#elif _WIN32
#include <stb_image.h>
#endif

namespace AA {

inline int helper_get_ogl_of_stb_internal_format(int stbi_internal_format) {
  int ogl_internal_format(0);
  switch (stbi_internal_format) {
  case 1:
    ogl_internal_format = GL_RED;
    break;
  case 2:
    ogl_internal_format = GL_RG;
    break;
  case 3:
    ogl_internal_format = GL_RGB;
    break;
  case 4:
    ogl_internal_format = GL_RGBA;
    break;
  }
  return ogl_internal_format;
}

// caches textures that are loaded
static int load_textures_from_scene(
  const aiScene* scn,
  const aiMaterial* mat,
  aiTextureType textureType,
  TextureType typeName,
  std::string orginalFilePath,
  TextureMapType& out_texInfo) {

  unsigned int num_textures = mat->GetTextureCount(textureType);
  if (num_textures == 0) { return -3; }

  int width(0), height(0), nrComponents(0);

  for (unsigned int i = 0; i < num_textures; ++i) {
    // make sure texture exists
    aiString aiTmpStr;
    auto tex_success = mat->GetTexture(textureType, i, &aiTmpStr);
    switch (tex_success) {
    case aiReturn_SUCCESS:
      break;
    case aiReturn_FAILURE:
      return -1;
      break;
    case aiReturn_OUTOFMEMORY:
      return -2;
      break;
    }

    // store the uv map mode
    aiTextureMapMode enum_map_mode{};
    if (AI_SUCCESS != mat->Get(AI_MATKEY_MAPPING(textureType, i), enum_map_mode)) {
      // no mapping mode - probably default
    }

    // pathing options we use later to help finding textures based on model path
    const std::size_t the_last_slash = orginalFilePath.find_last_of("/\\") + 1;
    const std::size_t the_last_dot = orginalFilePath.find_last_of(".");
    const std::string model_dir = orginalFilePath.substr(0, the_last_slash);  // path to filename's dir
    const std::string model_file_extension = orginalFilePath.substr(the_last_dot + 1u);  // get the file extension (textureType of file)
    const std::string model_file_name = orginalFilePath.substr(the_last_slash, the_last_dot - the_last_slash);  // get the name of the file

    // Begin: attempts at loading textures from embedded or path
    // Method 1. try from embedded
    const aiTexture* ai_embedded_texture = scn->GetEmbeddedTexture(aiTmpStr.C_Str());
    // when this happens we don't need to load it from a texture path, but from the memory
    if (ai_embedded_texture) {
      std::string embedded_filename = ai_embedded_texture->mFilename.C_Str();

      // fix path for glb (and fbx?) embedded textures and not having to reload them, as path is often our reuse already loaded key
      if (embedded_filename == "") { embedded_filename = model_file_name + "." + model_file_extension + "/" + toString(typeName); }

      // see if it has already been loaded previously to reuse
      auto reused_tex_info = Cache::Get()->try_load_from_cache(out_texInfo, embedded_filename);

      // not already loaded, lets try from embedded, this should succeed if it gets here
      if (reused_tex_info == 0) {
        Texture2DInfo a_new_texture_info;
        //bool compressed = (ai_embedded_texture->mHeight == 0) ? true : false;
        // for unflipped opengl coords, flip vertically on load to true
        stbi_set_flip_vertically_on_load(0);
        int texture_size = ai_embedded_texture->mWidth * std::max(ai_embedded_texture->mHeight, 1u);
        unsigned char* data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(ai_embedded_texture->pcData), texture_size, &width, &height, &nrComponents, 0);
        if (data) {
          int format = helper_get_ogl_of_stb_internal_format(nrComponents);
          a_new_texture_info.accessId = OpenGL::GetGL()->Upload2DTex(data, width, height, format, enum_map_mode);
          if (a_new_texture_info.accessId != 0) {
            // add the new one to our list of loaded textures
            a_new_texture_info.path = embedded_filename;
            a_new_texture_info.textureType = typeName;
            // update our list of loaded textures
            Cache::Get()->add(a_new_texture_info);
            // to return for draw info on this current mesh
            out_texInfo.emplace_back(a_new_texture_info);
          }
        }
        stbi_image_free(data);
      }
    }
    // ELSE: textures are not embedded
    // try from 3 most likely paths as detailed below
    // regular file, check if it exists and read it
    // the 3 paths to try 
    else {
      std::vector<std::string> potential_paths;
      // 1. the direct string (will probably fail)
      potential_paths.emplace_back(aiTmpStr.C_Str());
      // 2. the path based on where the model was loaded from (might work)
      std::string literal_path = model_dir + aiTmpStr.C_Str();
      potential_paths.emplace_back(literal_path);
      // 3. the last part of the given path (after '/' or '\\') appended to the path based on were the model was loaded from
      std::string from_model_path = model_dir + literal_path.substr(literal_path.find_last_of("/\\") + 1);  // all the way to the end
      potential_paths.emplace_back(from_model_path);
      // routine to see if we already have this texture loaded

      for (const auto& a_path : potential_paths) {
        auto already_loaded = Cache::Get()->try_load_from_cache(out_texInfo, a_path);
        if (already_loaded == 0)
          continue;
        else
          return 0; // success
      }

      // wasn't already loaded, lets try to load it
      Texture2DInfo a_new_texture_info;
      // for unflipped opengl coords, flip vertically on load to true
      stbi_set_flip_vertically_on_load(0);
      // try
      unsigned char* data = nullptr;
      for (const auto& a_path : potential_paths) {
        data = stbi_load(a_path.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
          // we have data that goes to the graphics card
          int format = helper_get_ogl_of_stb_internal_format(nrComponents);
          a_new_texture_info.accessId = OpenGL::GetGL()->Upload2DTex(data, width, height, format, enum_map_mode);
          if (a_new_texture_info.accessId != 0) {
            // add the new one to our list of loaded textures for management
            a_new_texture_info.path = a_path;
            a_new_texture_info.textureType = typeName;
            Cache::Get()->add(a_new_texture_info);
            // to return for draw info on this current mesh
            out_texInfo.emplace_back(a_new_texture_info);
            break;  // break out of for loop
          }
        }
      }
      stbi_image_free(data);
    }
  }

  // went through the above loop without error, AllLoadedTextures & out_texInfo should be updated
  return 0;
}

TextureMapType AssimpSceneLoader::LoadAllTextures(const aiScene* scene, const aiMaterial* ai_material, const std::string& orig_filepath) {
  TextureMapType all_loaded_textures;

  // get the albedo (diffuse) textures
  TextureMapType albedo_textures;
  if (load_textures_from_scene(scene, ai_material, aiTextureType_DIFFUSE, TextureType::ALBEDO, orig_filepath, albedo_textures) == 0) {
    for (auto& a_tex : albedo_textures) {
      all_loaded_textures.insert(all_loaded_textures.end(), a_tex);
    }
  }

  TextureMapType specular_textures;
  if (load_textures_from_scene(scene, ai_material, aiTextureType_SPECULAR, TextureType::SPECULAR, orig_filepath, specular_textures) == 0) {
    for (auto& s_tex : specular_textures) {
      all_loaded_textures.insert(all_loaded_textures.end(), s_tex);
    }
  }

  TextureMapType normal_textures;
  if (load_textures_from_scene(scene, ai_material, aiTextureType_NORMALS, TextureType::NORMAL, orig_filepath, normal_textures) == 0) {
    for (auto& n_tex : normal_textures) {
      all_loaded_textures.insert(all_loaded_textures.end(), n_tex);
    }
  }

  // if finding normal textures failed, see if it is actually called a heightmap (.obj files soemtimes have this according to https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
  if (normal_textures.empty()) {
    if (load_textures_from_scene(scene, ai_material, aiTextureType_HEIGHT, TextureType::NORMAL, orig_filepath, normal_textures) == 0) {
      for (auto& n_tex : normal_textures) {
        all_loaded_textures.insert(all_loaded_textures.end(), n_tex);
      }
    }
  }

  // emissive textures will glow on the lit shader
  TextureMapType emissive_textures;
  if (load_textures_from_scene(scene, ai_material, aiTextureType_EMISSIVE, TextureType::EMISSION, orig_filepath, emissive_textures) == 0) {
    for (auto& e_tex : emissive_textures) {
      all_loaded_textures.insert(all_loaded_textures.end(), e_tex);
    }
  }

  return all_loaded_textures;
}

unsigned int AssimpSceneLoader::LoadCubeMapTexture(const std::vector<std::string>& six_texture_paths) {
  if (six_texture_paths.size() != 6)
    throw("wrong number of textures");
  int width(0), height(0), nrChannel(0);
  unsigned int return_id(0u);
  std::vector<unsigned char*> data;
  data.resize(6);
  stbi_set_flip_vertically_on_load(0); // tell stb_image.h to not flip loaded texture's on the y-axis.
  for (auto i = 0; i < 6; ++i) {
    data[i] = stbi_load(six_texture_paths[i].c_str(), &width, &height, &nrChannel, 0);
  }
  if (data[0] && data[5]) {  // ensure first and last data pics are there (middle not checked but assumed)
    int format = helper_get_ogl_of_stb_internal_format(nrChannel);
    return_id = OpenGL::GetGL()->UploadCubeMapTex(data, width, height, format);
  }
  for (auto i = 0; i < 6; ++i) {
    stbi_image_free(data[i]);
  }
  return return_id;
}

} // end namespace AA
