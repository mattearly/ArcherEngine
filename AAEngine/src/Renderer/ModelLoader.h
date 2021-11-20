#pragma once
#include "../Scene/Prop.h"
#include "MeshInfo.h"
#include "TextureInfo.h"
#include <assimp/scene.h>
#include <vector>
#include <forward_list>

namespace AA {



// ModelLoader is pretty specific to opengl currently
class ModelLoader {
public:
  static int LoadGameObjectFromFile(Prop& out_model, const std::string& path);
  static void UnloadGameObject(const std::vector<MeshInfo>& toUnload);
private:
  static void recursive_processNode(aiNode* node, const aiScene* scene, Prop& out_model);
  static bool CheckIfModelIsAlreadyLoaded(Prop& out_model, const std::string& path);

  ModelLoader() = delete;
};

} // end namespace AA