#include "AssimpSceneLoader.h"
#include "../Graphics.h"
#include "../../../Mesh/Vertex.h"
#include "../../../Math/Conversions.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sstream>
#include <utility>
#include <forward_list>
#include <unordered_map>
#include <map>

namespace AA {

// local helper to set a single AnimVertex with bone & weight
static void set_vertex_bone_data(AnimVertex& out_vertex, int boneID, float weight) {
  for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
    if (out_vertex.m_BoneIDs[i] < 0) {
      out_vertex.m_Weights[i] = weight;
      out_vertex.m_BoneIDs[i] = boneID;
      break;
    }
  }
}

// local helper to set the bone weights on verticies from aimesh and scene
static void extract_bone_weights_for_verts(aiMesh* mesh, const aiScene* scene, Skeleton& out_skel, std::vector<AnimVertex>& out_vertices) {
  auto num_bones = mesh->mNumBones;
  for (unsigned int boneIndex = 0; boneIndex < num_bones; ++boneIndex) {
    
    // name for finding in our map
    BONE_MAP_KEY boneName = mesh->mBones[boneIndex]->mName.C_Str();

    if (out_skel.m_BoneInfoMap.find(boneName) == out_skel.m_BoneInfoMap.end()) {  // didn't find this bone name in our current out_skel map
      BoneInfo boneinfo;
      boneinfo.id = out_skel.m_BoneCounter;
      boneinfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
      out_skel.m_BoneInfoMap[boneName] = boneinfo;
      out_skel.m_BoneCounter++;  // update
    }

    auto weights = mesh->mBones[boneIndex]->mWeights;
    int numWeights = mesh->mBones[boneIndex]->mNumWeights;

    for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
      int vertexId = weights[weightIndex].mVertexId;
      float weight = weights[weightIndex].mWeight;
      assert(vertexId < out_vertices.size());
      set_vertex_bone_data(out_vertices[vertexId], out_skel.m_BoneInfoMap[boneName].id, weight);
    }

  }
}

/// <summary>
/// 4 step helper function for Load_MeshesTexturesMaterials & recursive_process_node
/// </summary>
/// <returns>MeshInfo: data for drawing a Mesh</returns>
static MeshInfo extract_all_mesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4* trans, const std::string& in_load_path) {
  // Sanity Check
  unsigned int num_of_vertices_on_mesh = mesh->mNumVertices;
  if (num_of_vertices_on_mesh == 0)
    throw("mesh has no vertices");

  // Step1: Get the Vertices
  std::vector<LitVertex> loaded_vertices;
  if (mesh->mTextureCoords[0]) { // case for when texture coordinantes exist
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      loaded_vertices.emplace_back(LitVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(mesh->mTextureCoords[0][i].x, 1 - mesh->mTextureCoords[0][i].y), aiVec3_to_glmVec3(mesh->mNormals[i])));
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
  vao = OpenGL::GetGL()->UploadStatic3DMesh(loaded_vertices, loaded_elements);

  // Step4: GetTextures&Materials
  MeshInfo return_info(
    vao,
    static_cast<unsigned int>(loaded_elements.size()),
    AssimpSceneLoader::LoadAllTextures(scene, scene->mMaterials[mesh->mMaterialIndex], in_load_path),  // get all the textures that belong with this mesh
    AssimpSceneLoader::LoadAll(scene, scene->mMaterials[mesh->mMaterialIndex]),
    aiMat4_to_glmMat4(*trans));

  // Return this draw data to the user
  return return_info;
}

/// <summary>
/// 4 step helper function for Load_MeshesTexturesMaterialsBones & recursive_process_node
/// </summary>
/// <returns>MeshInfo: data for drawing a Mesh</returns>
static MeshInfo extract_all_mesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4* trans, Skeleton& out_skel, const std::string& in_load_path) {
  // Sanity Check
  unsigned int num_of_vertices_on_mesh = mesh->mNumVertices;
  if (num_of_vertices_on_mesh == 0)
    throw("mesh has no vertices");

  // Step1: Get the Vertices
  std::vector<AnimVertex> loaded_vertices;

  if (mesh->mTextureCoords[0]) { // case for when texture coordinantes exist
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      loaded_vertices.emplace_back(AnimVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(mesh->mTextureCoords[0][i].x, 1 - mesh->mTextureCoords[0][i].y), aiVec3_to_glmVec3(mesh->mNormals[i])));
    }
  } else {  // case for when texture coordinantes do not exist
    for (unsigned int i = 0; i < num_of_vertices_on_mesh; ++i) {
      loaded_vertices.emplace_back(AnimVertex(aiVec3_to_glmVec3(mesh->mVertices[i]), glm::vec2(0), aiVec3_to_glmVec3(mesh->mNormals[i])));
    }
  }

  extract_bone_weights_for_verts(mesh, scene, out_skel, loaded_vertices);

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
  vao = OpenGL::GetGL()->UploadStatic3DMesh(loaded_vertices, loaded_elements);

  // Step4: GetTextures&Materials
  MeshInfo return_info(
    vao,
    static_cast<unsigned int>(loaded_elements.size()),
    AssimpSceneLoader::LoadAllTextures(scene, scene->mMaterials[mesh->mMaterialIndex], in_load_path),  // get all the textures that belong with this mesh
    AssimpSceneLoader::LoadAll(scene, scene->mMaterials[mesh->mMaterialIndex]),
    aiMat4_to_glmMat4(*trans));

  // Return this draw data to the user
  return return_info;
}

// recursive helper function for Load_MeshesTexturesMaterials
static void recursive_process_node(aiNode* node, const aiScene* scene, std::vector<MeshInfo>& out_mesh_info, const std::string& in_load_path) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    out_mesh_info.push_back(extract_all_mesh(mesh, scene, &node->mTransformation, in_load_path));
  }
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    recursive_process_node(node->mChildren[i], scene, out_mesh_info, in_load_path);
  }
}

// recursive helper function for Load_MeshesTexturesMaterialsBones
static void recursive_process_node(aiNode* node, const aiScene* scene, std::vector<MeshInfo>& out_mesh_info, Skeleton& out_skel, const std::string& in_load_path) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    out_mesh_info.push_back(extract_all_mesh(mesh, scene, &node->mTransformation, out_skel, in_load_path));
  }
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    recursive_process_node(node->mChildren[i], scene, out_mesh_info, out_skel, in_load_path);
  }
}

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
int AssimpSceneLoader::Load_MeshesTexturesMaterials(Scene& out_model, const std::string& in_path_to_load) {
  int load_status_code = 0;
  std::vector<MeshInfo> mesh_builder;

  load_status_code = Cache::Get()->try_load_from_cache(mesh_builder, in_path_to_load);

  if (load_status_code == 1) { // already loaded
    out_model.SetMeshes(mesh_builder);
    out_model.SetPathID(in_path_to_load);
  } else if (load_status_code == 0) { // not loaded
    Assimp::Importer importer;  // use assimp to import
    //post processing -> http://assimp.sourceforge.net/lib_html/postprocess_8h.html
    // join is okay on non-animated models
    int ai_post_processing_flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_ValidateDataStructure;
    const aiScene* scene = importer.ReadFile(in_path_to_load, ai_post_processing_flags);
    if (!scene)  // error check
      load_status_code = -1; // no scene
    else if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
      load_status_code = -2; // scene flags incomplete
    else if (!scene->mRootNode)
      load_status_code = -3; // no root node

    if (load_status_code == 0) {
      recursive_process_node(scene->mRootNode, scene, mesh_builder, in_path_to_load);
      out_model.SetMeshes(mesh_builder);
      out_model.SetPathID(in_path_to_load);

      // if it loaded correctly, save a ref to this loaded up model (certainly a new entry at this point because we checked for copy at the beginning of this function Load_MeshesTexturesMaterials)
      SceneInfo scene_to_cache;
      scene_to_cache.meshes = mesh_builder;
      scene_to_cache.path = in_path_to_load;
      Cache::Get()->add(scene_to_cache);
    }
  }

  return load_status_code;
}

int AssimpSceneLoader::Load_MeshesTexturesMaterialsBones(Scene& out_model, const std::string& in_path_to_load) {
  int return_code = 0;
  std::vector<MeshInfo> mesh_builder{};
  Skeleton skeleton_builder;
  glm::mat4 inv_trans{};

  return_code = Cache::Get()->try_load_from_cache(mesh_builder, skeleton_builder, inv_trans, in_path_to_load);

  if (return_code == 1) {
    out_model.SetMeshes(mesh_builder);
    out_model.SetSkeleton(skeleton_builder);
    out_model.SetPathID(in_path_to_load);
    out_model.SetGlobalInverseTransform(inv_trans);

  } else /*if (return_code != 1)*/ {

    Assimp::Importer importer;
    //post processing -> http://assimp.sourceforge.net/lib_html/postprocess_8h.html
    // do not use join on animated models
    int post_processing_flags =/* aiProcess_JoinIdenticalVertices | */aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_ValidateDataStructure;
    const aiScene* scene = importer.ReadFile(in_path_to_load, post_processing_flags);

    // check if errors on load
    if (!scene)
      return_code = -1;
    else if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
      return_code = -2;
    else if (!scene->mRootNode)
      return_code = -3;

    if (return_code == 0) {
      // put together the model
      recursive_process_node(scene->mRootNode, scene, mesh_builder, skeleton_builder, in_path_to_load);

      // set output
      out_model.SetMeshes(mesh_builder);
      out_model.SetPathID(in_path_to_load);
      out_model.SetSkeleton(skeleton_builder);  // copy save
      out_model.SetGlobalInverseTransform(glm::inverse(aiMat4_to_glmMat4(scene->mRootNode->mTransformation)));

      // cache for later reloads of the same model
      SceneInfo temp_mesh_info;
      temp_mesh_info.meshes = mesh_builder;
      temp_mesh_info.path = in_path_to_load;
      temp_mesh_info.skelly = new Skeleton(skeleton_builder);  // copy save
      temp_mesh_info.inverse_transform = glm::inverse(aiMat4_to_glmMat4(scene->mRootNode->mTransformation));
      Cache::Get()->add(temp_mesh_info);
    }
  }

  return return_code;
}

void AssimpSceneLoader::Unload(const std::string& path_to_unload) {
  Cache::Get()->remove_scene_at_path(path_to_unload);
}

} // end namespace AA
