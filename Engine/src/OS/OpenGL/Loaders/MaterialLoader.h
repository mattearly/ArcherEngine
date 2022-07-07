#pragma once
#include "Cache.h"
#include <assimp/scene.h>
#include <assimp/material.h>
namespace AA {

class MaterialLoader {
public:

  static Material LoadAll(const aiScene* scene, const aiMaterial* ai_material);

private:

  MaterialLoader() = delete;

};
}