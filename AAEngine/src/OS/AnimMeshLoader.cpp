#include "AnimMeshLoader.h"
#include "TextureLoader.h"
#include "OpenGL/OGLGraphics.h"
#include "../Mesh/AnimProp.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#ifdef _DEBUG
#include <iostream>
#endif
namespace AA {

static std::string LastLoadedAnimPath;

//todo: add refinfo for reusing already loaded models

int AnimMeshLoader::LoadGameObjectFromFile(AnimProp& out_model, const std::string& path) {
#ifdef _DEBUG
  std::cout << "-> LOAD... ANIMATED PROP: " << path << std::endl;
#endif

  Assimp::Importer importer;
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
    //aiProcess_GenSmoothNormals |
    //aiProcess_FixInfacingNormals |
    //aiProcess_FindInvalidData |
    aiProcess_ValidateDataStructure
    ;

  const aiScene* scene = importer.ReadFile(path, post_processing_flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    if (!scene) {
#ifdef _DEBUG
      std::cout << "scene loading error: " << importer.GetErrorString() << std::endl;
#endif
      return -1;
    }
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
      return -2;
    if (!scene->mRootNode)
      return -3;
  }

  // save global inverse transform for bone animations later
  out_model.mGlobalInverseTransform = glm::inverse(aiMat4_to_glmMat4(scene->mRootNode->mTransformation));

  LastLoadedAnimPath = path;

  recursive_processNode(out_model, scene->mRootNode, scene);

  return 0;
}

void AnimMeshLoader::UnloadGameObject(const std::vector<MeshInfo>& toUnload) {
  for (const auto& a_mesh : toUnload) {
    OGLGraphics::DeleteMesh(a_mesh.vao);
    TextureLoader::UnloadTexture(a_mesh.textureDrawIds);
  }
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
    tmp_mesh.physicsBody = nullptr;
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