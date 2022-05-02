#pragma once
#include "../../../../include/AAEngine/Mesh/Prop.h"
#include "../../../Mesh/MeshInfo.h"
#include "TextureInfo.h"
#include <assimp/scene.h>
#include <vector>
#include <forward_list>

namespace AA {

// ModelLoader is pretty specific to OpenGL currently
class MeshLoader {
public:
  static int LoadGameObjectFromFile(Prop& out_model, const std::string& path_to_load);
  static void UnloadGameObject(const std::vector<MeshInfo>& toUnload, const std::string& path_to_unload);
private:
  static void recursive_processNode(aiNode* node, const aiScene* scene, Prop& out_model);
  MeshLoader() = delete;
};

} // end namespace AA