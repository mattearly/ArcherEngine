#include "MeshLoader.h"
#include "TextureLoader.h"
#include "Vertex.h"
#include "../Math/Conversions.h"
#include "OpenGL/OGLGraphics.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stb_image.h>

#include <sstream>
#include <utility>
#include <forward_list>
#include <unordered_map>
#include <map>

namespace AA {

/// <summary>
/// Holds info for reusing models that are already loaded.
/// </summary>
struct RefModelInfo {
  uint32_t vao = 0;
  uint32_t numElements = 0;
  std::string path = "";
  std::unordered_map<uint32_t, std::string> textureDrawIds{};   // id:type
  int ref_count = 1;
};

// keeps track of all the models we have loaded so fars
static std::forward_list<RefModelInfo> AllLoadedModels;

/// <summary>
/// Checks the AllLoadedModels reference list and reuses models that have already been loaded.
/// </summary>
/// <param name="out_model">the model to be populated if successful</param>
/// <param name="path">full original path</param>
/// <returns>true if out_model was populated, false if not</returns>
bool local_helper_reuse_if_already_loaded(std::vector<MeshInfo>& out_meshes, const std::string& path) {
  for (auto& ref_model_info : AllLoadedModels) {
    if (ref_model_info.path == "")
      continue;
    if (path == ref_model_info.path.data()) {
      ref_model_info.ref_count++;
      TextureLoader::increment_given_texture_ids(ref_model_info.textureDrawIds);
      out_meshes.emplace_back(MeshInfo(ref_model_info.vao, ref_model_info.numElements, ref_model_info.textureDrawIds, glm::mat4(1)));
      return true;  // already loaded
    }
  }
  return false; // not already loaded
}

// notes: this is not a very efficient way of doing this, but alas it does work
// todo: update and improve
void local_helper_decrement_all_loaded_models_ref(const std::string& path_to_remove) {
  // step1: decrement loaded count
  for (auto& ref_model_info : AllLoadedModels) {
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

  // b) C++20 method that should work on a forward list, but we are using C++17
  //std::erase_if(AllLoadedModels, [](const RefModelInfo& ref) {
  //  return ref.ref_count < 1;
  //  });

  // c) C++17 (and maybe some older) method to remove from foward_list
  auto before = AllLoadedModels.before_begin();
  for (auto it = AllLoadedModels.begin(); it != AllLoadedModels.end(); ) {
    if (it->ref_count < 1) {
      it = AllLoadedModels.erase_after(before);
    } else {
      before = it;
      ++it;
    }
  }
}

static std::string LastLoadedPath;

// Loads the data from the model at path, and saves it to out_model
// returns true (negative int) on fail to open:
//   error codes: 
//    -1 = scene was null after attempt to load
//    -2 = scene has incomplete flag from assimp after attempted to load
//    -3 = there is no root node on the model
// returns true (positive int) on model already loaded and reused mesh info reference
//    1 = model is already loaded/cached
// retursn false (0) on model loaded
//    0 = model loaded from given path
// otherwise returns 0 if the import is successful
int MeshLoader::LoadGameObjectFromFile(Prop& out_model, const std::string& path_to_load) {
  int return_code = 0;
  return_code = local_helper_reuse_if_already_loaded(out_model.mMeshes, path_to_load);

  if (return_code != 1) {
    static Assimp::Importer importer;
    int post_processing_flags = 0;

    //post processing -> http://assimp.sourceforge.net/lib_html/postprocess_8h.html
    post_processing_flags |= aiProcess_JoinIdenticalVertices |
      aiProcess_Triangulate |
      aiProcess_FlipUVs |
      //#ifdef D3D
      //	aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | 
      //#endif
      //aiProcess_PreTransformVertices |
      //aiProcess_CalcTangentSpace |
      //aiProcess_GenNormals |  // can't be used with gensmoothnormals
      aiProcess_GenSmoothNormals |
      //aiProcess_FixInfacingNormals |
      //aiProcess_FindInvalidData |
      aiProcess_ValidateDataStructure
      ;

    const aiScene* scene = importer.ReadFile(path_to_load, post_processing_flags);

    // check if errors on load
    if (!scene)
      return_code = -1;
    else if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
      return_code = -2;
    else if (!scene->mRootNode)
      return_code = -3;

    if (scene && return_code == 0) {
      LastLoadedPath = path_to_load;
      recursive_processNode(scene->mRootNode, scene, out_model);
      // if it loaded correctly, save a ref to this loaded up model (certainly a new entry at this point because we checked for copy at the beginning of this function LoadGameObjectFromFile)
      RefModelInfo temp_mesh_info;
      temp_mesh_info.vao = out_model.mMeshes.front().vao;
      temp_mesh_info.numElements = out_model.mMeshes.front().numElements;
      temp_mesh_info.path = path_to_load;
      temp_mesh_info.textureDrawIds = out_model.mMeshes.front().textureDrawIds;   // id:type
      AllLoadedModels.push_front(temp_mesh_info);
    }
  }

  return return_code;
}


// unloads all textures and vao's from a list of nodeinfos
void MeshLoader::UnloadGameObject(const std::vector<MeshInfo>& toUnload, const std::string& path_to_unload) {
  for (const auto& a_mesh : toUnload) {
    // remove reference of path
    local_helper_decrement_all_loaded_models_ref(path_to_unload);

    // delete mesh data from graphics card
    OGLGraphics::DeleteMesh(a_mesh.vao);

    // delete texture (or reduce reference count of them if others still in use)
    TextureLoader::UnloadTexture(a_mesh.textureDrawIds);
  }
}

/// <summary>
/// 4 step helper function for LoadGameObjectFromFile & recursive_processNode
/// </summary>
/// <param name="mesh"></param>
/// <param name="scene"></param>
/// <param name="trans"></param>
/// <returns>MeshInfo: data for drawing a Mesh</returns>
MeshInfo local_helper_processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4* trans) {
  // Sanity Check
  unsigned int num_of_vertices_on_mesh = mesh->mNumVertices;
  if (num_of_vertices_on_mesh == 0)
    throw("mesh has no vertices");

  // Step1: Get the Vertices
  std::vector<LitVertex> loaded_vertices;
  if (mesh->mTextureCoords[0]) { // case for when texture coordinantes exist
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      loaded_vertices.emplace_back(LitVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y), aiVec3_to_glmVec3(mesh->mNormals[i])));
    }
  } else {  // case for when texture coordinantes do not exist
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      loaded_vertices.emplace_back(LitVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(0), aiVec3_to_glmVec3(mesh->mNormals[i])));
    }
  }

  // Step2: Get the Indices to draw triangle faces with
  std::vector<unsigned int> loaded_elements;
  unsigned int num_faces = mesh->mNumFaces;
  for (unsigned int i = 0; i < num_faces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j) {
      loaded_elements.push_back(face.mIndices[j]);
    }
  }

  // Step3: Send the vertex data to the graphic memory
  unsigned int vao = 0;
  vao = OGLGraphics::UploadStatic3DMesh(loaded_vertices, loaded_elements);

  // Step4: Return this draw data to the user
  return MeshInfo(
    vao,
    (unsigned int)loaded_elements.size(),
    TextureLoader::LoadAllTextures(scene, scene->mMaterials[mesh->mMaterialIndex], LastLoadedPath),  // get all the textures that belong with this mesh
    aiMat4_to_glmMat4(*trans)
  );
}


// recursive helper function for LoadGameObjectFromFile
void MeshLoader::recursive_processNode(aiNode* node, const aiScene* scene, Prop& out_model) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    out_model.mMeshes.push_back(local_helper_processMesh(mesh, scene, &node->mTransformation));
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    recursive_processNode(node->mChildren[i], scene, out_model);
  }
}

} // end namespace AA
