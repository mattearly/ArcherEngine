#pragma once
#include "../../../../AAEngine/include/AAEngine/Mesh/AnimProp.h"
namespace AA {

// ModelLoader is pretty specific to opengl currently
class AnimModelLoader {
public:
  static int LoadGameObjectFromFile(AnimProp& out_model, const std::string& path_to_load);
  static void UnloadGameObject(const std::vector<MeshInfo>& toUnload, const std::string& path_to_unload);
private:
  static void recursive_processNode(AnimProp& out_model, aiNode* node, const aiScene* scene);
  static void extractBoneWeightForVertices(AnimProp& out_model, std::vector<AnimVertex>& vertices, aiMesh* mesh, const aiScene* scene);
  AnimModelLoader() = delete;
};

}
