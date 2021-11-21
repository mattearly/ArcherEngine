#include "Prop.h"
#include "Camera.h"
#include "../Renderer/OpenGL/OGLGraphics.h"
#include "../Renderer/OpenGL/OGLShader.h"
#include "../Renderer/ModelLoader.h"
#include "../Renderer/LoadCube.h"
#include "../Utility/Conversions.h"
#include "../DefaultShaders.h"

namespace AA {

Prop::Prop() {
  mLit = true;
}

Prop::Prop(const char* path) {
  mLit = true;
  if (ModelLoader::LoadGameObjectFromFile(*this, path) == -1)
    throw ("failed to load path");
}

void Prop::Draw(const std::shared_ptr<Camera>& cam) {
  OGLShader* shader = NULL;
  shader = (mLit) ? DefaultShaders::get_phong_3d() : DefaultShaders::get_diffuse_3d();
  shader->Use();
  shader->SetMat4("u_projection_matrix", cam->mProjectionMatrix);
  shader->SetMat4("u_view_matrix", cam->mViewMatrix);
  shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);

  for (MeshInfo& m : mMeshes) {
    shader->SetBool("hasAlbedo", false);
    if (mLit) {
      shader->SetBool("hasSpecular", false);
      shader->SetBool("hasNormal", false);
      shader->SetBool("hasEmission", false);
    }

    //if (m.physicsBody) {
    //  physxTransform_to_glmMat4(m.physicsBody->getGlobalPose(), m.local_transform);
    //}

    for (const auto& texture : m.textureDrawIds) {
      const std::string texType = texture.second;  // get the texture type

      if (texType == "Albedo") {
        shader->SetBool("hasAlbedo", true);
        OGLGraphics::SetTexture(0, texture.first);
        shader->SetInt(("material." + texType).c_str(), 0);
      }

      if (mLit) {
        if (texType == "Specular") {
          shader->SetBool("hasSpecular", true);
          shader->SetInt(("material." + texType).c_str(), 1);
          shader->SetFloat("material.Shininess", m.shininess);
          OGLGraphics::SetTexture(1, texture.first);
        }

        if (texType == "Normal") {
          OGLGraphics::SetTexture(2, texture.first);
          shader->SetInt(("material." + texType).c_str(), 2);
        }

        if (texType == "Emission") {
          shader->SetInt("hasEmission", 1);
          OGLGraphics::SetTexture(4, texture.first);
          shader->SetInt(("material." + texType).c_str(), 3);
        }
      }

    }

    //if (m.emission != vec4(0)) {
    //  DefaultShaders::get_phong_3d()->SetVec4("emission", m.emission);
    //}


    OGLGraphics::SetCullFace(m.backface_culled);
    OGLGraphics::RenderElements(m.vao, m.numElements);

    //todo: we should be setting textures back to 0 or the defaults for that type
  }
}

void Prop::RemoveCache() {
  if (!mMeshes.empty()) {
    ModelLoader::UnloadGameObject(mMeshes);
    mMeshes.clear();
  }
}

void Prop::Load(const std::string& path) {
  if (mMeshes.empty()) {
    if (ModelLoader::LoadGameObjectFromFile(*this, path) == -1) {
      throw ("failed to load path");
    }
  }
  else {
    throw("Meshes are loaded already. Remove cache first.");
  }
}


}  // end namespace AA