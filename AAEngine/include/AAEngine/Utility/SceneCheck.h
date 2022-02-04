#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
namespace AA {
struct SceneHeader {
  std::string sceneLoadPath;

  int numNodes = 0;

  int numMeshes = 0;
  int numMeshesWithNormals = 0;
  int numTextures = 0;
  int numMaterials = 0;
  int numAnimations = 0;
  std::vector<std::string> animNames;

  int numVertices = 0;
  int numFaces = 0;
  int numBones = 0;

  int numLights = 0;
  int numCameras = 0;

};


inline void errorCheck(const aiScene* scene) {
  if (!scene) {
    throw("scene is null");
  }
  if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    std::cout << "flags incomplete\n";
    if (scene->mNumMeshes == 0) {
      std::cout << "animations only\n";
    } else {
      throw("flags incomplete");
    }
  }
  if (!scene->mRootNode) {
    throw("no nodes");
  }
}

inline void getVertCounts(aiNode* node, const aiScene* scene, SceneHeader& out_) {
  out_.numNodes++;
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    if (mesh->HasPositions())
      out_.numVertices += mesh->mNumVertices;
    if (mesh->HasBones())
      out_.numBones += mesh->mNumBones;
    if (mesh->HasFaces())
      out_.numFaces += mesh->mNumFaces;
    if (mesh->HasNormals())
      out_.numMeshesWithNormals++;
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    getVertCounts(node->mChildren[i], scene, out_);
  }
}

inline SceneHeader extractSceneInfo(const std::string& path) {
  SceneHeader ret;
  ret.sceneLoadPath = path;

  Assimp::Importer* importer = new Assimp::Importer();
  const aiScene* scene = importer->ReadFile(path, aiProcess_Triangulate);
  errorCheck(scene);
  if (scene->mRootNode) {
    getVertCounts(scene->mRootNode, scene, ret);
  }
  if (scene->HasMeshes()) { ret.numMeshes = scene->mNumMeshes; }
  if (scene->HasTextures()) { ret.numTextures = scene->mNumTextures; }
  if (scene->HasMaterials()) { ret.numMaterials = scene->mNumMaterials; }
  if (scene->HasAnimations()) {
    ret.numAnimations = scene->mNumAnimations;
    for (auto i = 0; i < ret.numAnimations; ++i) {
      ret.animNames.push_back(scene->mAnimations[i]->mName.C_Str());
    }
  }
  if (scene->HasLights()) { ret.numLights = scene->mNumLights; }
  if (scene->HasCameras()) { ret.numCameras = scene->mNumCameras; }
  delete importer;
  importer = nullptr;
  return ret;
}

inline void printSceneHeaderInfo(const SceneHeader& header) {
  std::cout
    << "--A Scene File--\n"
    << "File Path: " << header.sceneLoadPath << '\n'
    << "Nodes: " << header.numNodes << '\n'
    << "Meshes: " << header.numMeshes << '\n'
    << " - WithNorms: " << header.numMeshesWithNormals << '\n'
    << " - Verts: " << header.numVertices << '\n'
    << " - Faces: " << header.numFaces << '\n'
    << " - Bones: " << header.numBones << '\n'
    << "Textures: " << header.numTextures << '\n'
    << "Materials: " << header.numMaterials << '\n'
    << "Animations: " << header.numAnimations << '\n';
  for (auto i = 0; i < header.numAnimations; ++i) {
    printf(" -Anim %d: %s\n", i, header.animNames[i].c_str());
  }
  std::cout
    << "Lights: " << header.numLights << '\n'
    << "Cameras: " << header.numCameras << '\n';

}

} // end namespace AA