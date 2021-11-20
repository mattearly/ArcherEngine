#pragma once
#include "../Scene/AnimProp.h"
namespace AA {

// ModelLoader is pretty specific to opengl currently
class AnimModelLoader {
public:
  static int LoadGameObjectFromFile(AnimProp& out_model, const std::string& path);
  static void UnloadGameObject(const std::vector<MeshInfo>& toUnload);
private:
  static void recursive_processNode(AnimProp& out_model, aiNode* node, const aiScene* scene);
  static void extractBoneWeightForVertices(AnimProp& out_model, std::vector<AnimVertex>& vertices, aiMesh* mesh, const aiScene* scene);
  AnimModelLoader() = delete;
};

}
