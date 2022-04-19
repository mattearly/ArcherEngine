#include "AnimMeshLoader.h"
#include "TextureLoader.h"
#include "OpenGL/OGLGraphics.h"
#include "../../include/AAEngine/Mesh/AnimProp.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace AA {

/// <summary>
/// Holds info for reusing models that are already loaded.
/// </summary>
struct AnimRefModelInfo {
  uint32_t vao = 0;
  uint32_t numElements = 0;
  std::string path = "";
  std::unordered_map<uint32_t, std::string> textureDrawIds{};   // id:type
  Skeleton skelly;
  int ref_count = 1;
};

// keeps track of all the models we have loaded so fars
static std::forward_list<AnimRefModelInfo> AllLoadedAnimModels;

/// <summary>
/// Checks the AllLoadedAnimModels reference list and reuses models that have already been loaded.
/// </summary>
/// <param name="out_model">the model to be populated if successful</param>
/// <param name="path">full original path</param>
/// <returns>true if out_model was populated, false if not</returns>
bool local_helper_reuse_anim_model_if_already_loaded(std::vector<MeshInfo>& out_meshes, Skeleton& skel, const std::string& path) {
  for (auto& ref_model_info : AllLoadedAnimModels) {
    if (ref_model_info.path == "")
      continue;
    if (path == ref_model_info.path.data()) {
      ref_model_info.ref_count++;
      TextureLoader::increment_given_texture_ids(ref_model_info.textureDrawIds);
      out_meshes.emplace_back(MeshInfo(ref_model_info.vao, ref_model_info.numElements, ref_model_info.textureDrawIds, glm::mat4(1)));
      skel = ref_model_info.skelly;
      return true;  // already loaded
    }
  }
  return false; // not already loaded
}

// notes: this is not a very efficient way of doing this, but alas it does work
// todo: update and improve
void local_helper_decrement_all_loaded_anim_models_ref(const std::string& path_to_remove) {
  // step1: decrement loaded count
  for (auto& ref_model_info : AllLoadedAnimModels) {
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
  auto before = AllLoadedAnimModels.before_begin();
  for (auto it = AllLoadedAnimModels.begin(); it != AllLoadedAnimModels.end(); ) {
    if (it->ref_count < 1) {
      it = AllLoadedAnimModels.erase_after(before);
    } else {
      before = it;
      ++it;
    }
  }
}

static std::string LastLoadedAnimPath;

void AnimMeshLoader::UnloadGameObject(const std::vector<MeshInfo>& toUnload, const std::string& path_to_unload) {
  for (const auto& a_mesh : toUnload) {
    local_helper_decrement_all_loaded_anim_models_ref(path_to_unload);
    OGLGraphics::DeleteMesh(a_mesh.vao);
    TextureLoader::UnloadTexture(a_mesh.textureDrawIds);
  }
}

int AnimMeshLoader::LoadGameObjectFromFile(AnimProp& out_model, const std::string& path_to_load) {
  int return_code = 0;
  return_code = local_helper_reuse_anim_model_if_already_loaded(out_model.mMeshes, out_model.m_Skeleton, path_to_load);

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
      LastLoadedAnimPath = path_to_load;

      // save global inverse transform for bone animations later
      out_model.mGlobalInverseTransform = glm::inverse(aiMat4_to_glmMat4(scene->mRootNode->mTransformation));

      recursive_processNode(out_model, scene->mRootNode, scene);

      // cache for later reloads of the same model
      AnimRefModelInfo temp_mesh_info;
      temp_mesh_info.vao = out_model.mMeshes.front().vao;
      temp_mesh_info.numElements = out_model.mMeshes.front().numElements;
      temp_mesh_info.path = path_to_load;
      temp_mesh_info.textureDrawIds = out_model.mMeshes.front().textureDrawIds;   // id:type
      temp_mesh_info.skelly = out_model.m_Skeleton;
      AllLoadedAnimModels.push_front(temp_mesh_info);
    }
  }
  return return_code;
}

// local helper
void local_helper_setVertexBoneDataToDefault(AnimVertex& vertex) {
  for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
    vertex.m_BoneIDs[i] = -1;
    vertex.m_Weights[i] = 0.0f;
  }
}

// local helper
void local_helper_setVertexBoneData(AnimVertex& vertex, int boneID, float weight) {
  for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
    if (vertex.m_BoneIDs[i] < 0) {
      vertex.m_Weights[i] = weight;
      vertex.m_BoneIDs[i] = boneID;
      break;
    }
  }
}

// helper
void AnimMeshLoader::extractBoneWeightForVertices(AnimProp& out_model, std::vector<AnimVertex>& vertices, aiMesh* mesh, const aiScene* scene) {
  for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
    int boneID = -1;
    std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

    if (out_model.m_Skeleton.m_BoneInfoMap.find(boneName) == out_model.m_Skeleton.m_BoneInfoMap.end()) {
      // didn't find bone, create a new one
      BoneInfo newBoneInfo{};
      newBoneInfo.id = out_model.m_Skeleton.m_BoneCounter;
      newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
      out_model.m_Skeleton.m_BoneInfoMap[boneName] = newBoneInfo;
      boneID = out_model.m_Skeleton.m_BoneCounter;
      out_model.m_Skeleton.m_BoneCounter++;
    } else {
      boneID = out_model.m_Skeleton.m_BoneInfoMap[boneName].id;
    }

    assert(boneID != -1);

    auto weights = mesh->mBones[boneIndex]->mWeights;
    int numWeights = mesh->mBones[boneIndex]->mNumWeights;

    for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
      int vertexId = weights[weightIndex].mVertexId;
      float weight = weights[weightIndex].mWeight;
      assert(vertexId < vertices.size());
      local_helper_setVertexBoneData(vertices[vertexId], boneID, weight);
    }
  }
}

// local helper
// returns the number of elements
void local_helper_processMesh(std::vector<AnimVertex>& out_loaded_verts, std::vector<GLuint>& out_loaded_indices, std::unordered_map<unsigned int, std::string>& out_loaded_textures, aiMesh* mesh, const aiScene* scene) {
  unsigned int num_of_vertices_on_mesh = mesh->mNumVertices;
  if (num_of_vertices_on_mesh == 0)
    throw("mesh has no vertices");

  // Get the vertices
  if (mesh->mTextureCoords[0]) {  // textured
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      out_loaded_verts.emplace_back(AnimVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y), aiVec3_to_glmVec3(mesh->mNormals[i])));
      local_helper_setVertexBoneDataToDefault(out_loaded_verts.back());
    }
  } else {  // not textured
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      out_loaded_verts.emplace_back(AnimVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(0), aiVec3_to_glmVec3(mesh->mNormals[i])));
      local_helper_setVertexBoneDataToDefault(out_loaded_verts.back());
    }
  }

  // Get the Indices to draw triangle faces with
  unsigned int num_faces = mesh->mNumFaces;
  for (unsigned int i = 0; i < num_faces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j) {
      out_loaded_indices.push_back(face.mIndices[j]);
    }
  }

  // get the materials
  const aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];
  out_loaded_textures = TextureLoader::LoadAllTextures(scene, ai_material, LastLoadedAnimPath);
}

// helper
void AnimMeshLoader::recursive_processNode(AnimProp& out_model, aiNode* node, const aiScene* scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
    std::vector<AnimVertex> loaded_verts;
    std::vector<GLuint> loaded_indices;
    std::unordered_map<unsigned int, std::string> loaded_textures;

    MeshInfo tmp_mesh(0, 0);

    local_helper_processMesh(loaded_verts, loaded_indices, tmp_mesh.textureDrawIds, ai_mesh, scene);

    extractBoneWeightForVertices(out_model, loaded_verts, ai_mesh, scene);

    tmp_mesh.backface_culled = false;
    tmp_mesh.local_transform = aiMat4_to_glmMat4(node->mTransformation);
    tmp_mesh.shininess = 1.f;
    tmp_mesh.vao = OGLGraphics::UploadStatic3DMesh(loaded_verts, loaded_indices);  // upload for rendering capabilites
    tmp_mesh.numElements = static_cast<unsigned int>(loaded_indices.size());

    out_model.mMeshes.push_back(tmp_mesh);  // populate our our model file
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    recursive_processNode(out_model, node->mChildren[i], scene);
  }
}
}