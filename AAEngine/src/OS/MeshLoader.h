#pragma once
#include "../Mesh/Prop.h"
#include "MeshInfo.h"
#include "TextureInfo.h"
#include <assimp/scene.h>
#include <vector>
#include <forward_list>

namespace AA {



// ModelLoader is pretty specific to opengl currently
class MeshLoader {
public:
  static int LoadGameObjectFromFile(Prop& out_model, const std::string& path, const bool load_alpha = false);
  static void UnloadGameObject(const std::vector<MeshInfo>& toUnload);
private:
  static void recursive_processNode(aiNode* node, const aiScene* scene, Prop& out_model);
  static bool IsAlreadyLoaded(Prop& out_model, const std::string& path);

  MeshLoader() = delete;
};

} // end namespace AA