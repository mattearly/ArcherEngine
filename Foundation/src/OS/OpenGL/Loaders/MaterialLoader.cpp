#include "AssimpSceneLoader.h"
namespace AA {
Material AssimpSceneLoader::LoadAll(const aiScene* scene, const aiMaterial* ai_material) {
  aiString name{};
  ai_material->Get(AI_MATKEY_NAME, name);

  // find color/tint/albedo
  aiColor3D color(0.f, 0.f, 0.f);
  ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

  // find ambient
  aiColor3D ambient(0.f, 0.f, 0.f);
  ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

  // find specular
  aiColor3D specular(0.f, 0.f, 0.f);
  ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

  // find emission
  aiColor3D emission(0.f, 0.f, 0.f);
  ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, emission);

  Material out_mat;
  out_mat.Diffuse.x = color.r;
  out_mat.Diffuse.y = color.g;
  out_mat.Diffuse.z = color.b;

  out_mat.Ambient.x = ambient.r;
  out_mat.Ambient.y = ambient.g;
  out_mat.Ambient.z = ambient.b;

  out_mat.SpecularColor.x = specular.r;
  out_mat.SpecularColor.y = specular.g;
  out_mat.SpecularColor.z = specular.b;

  out_mat.Emission.x = emission.r;
  out_mat.Emission.y = emission.g;
  out_mat.Emission.z = emission.b;

  return out_mat;
}
} // end namespace AA
