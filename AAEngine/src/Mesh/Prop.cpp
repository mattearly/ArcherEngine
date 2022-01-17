#include "Prop.h"
#include "../OS/OpenGL/OGLGraphics.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/MeshLoader.h"
#include "../OS/LoadCube.h"
#include "../Math/Conversions.h"
#include "../DefaultShaders.h"

namespace AA {

Prop::Prop() {}

Prop::Prop(const char* path) {
  if (MeshLoader::LoadGameObjectFromFile(*this, path) == -1)
    throw ("failed to load path");
  stenciled = false;
  stencil_color = glm::vec3(0.1f, 0.87f, 0.1f);
}

void Prop::Draw() {
  if (stenciled) {
    // 1st pass: render to stencil buffer with normal draw
    OGLGraphics::SetStencilFuncToAlways();
    OGLGraphics::SetStencilMask(true);
    OGLShader* uber_shader = DefaultShaders::get_ubershader();
    uber_shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);
    for (MeshInfo& m : mMeshes) {
      uber_shader->SetBool("hasAlbedo", false);
      uber_shader->SetBool("hasSpecular", false);
      uber_shader->SetBool("hasNormal", false);
      uber_shader->SetBool("hasEmission", false);
      for (const auto& texture : m.textureDrawIds) {
        const std::string texType = texture.second;  // get the texture type
        if (texType == "Albedo") {
          uber_shader->SetBool("hasAlbedo", true);
          OGLGraphics::SetTexture(0, texture.first);
          uber_shader->SetInt(("material." + texType).c_str(), 0);
        }
        if (texType == "Specular") {
          uber_shader->SetBool("hasSpecular", true);
          uber_shader->SetInt(("material." + texType).c_str(), 1);
          uber_shader->SetFloat("material.Shininess", m.shininess);
          OGLGraphics::SetTexture(1, texture.first);
        }
        if (texType == "Normal") {
          uber_shader->SetBool("hasNormal", true);
          OGLGraphics::SetTexture(2, texture.first);
          uber_shader->SetInt(("material." + texType).c_str(), 2);
        }
        if (texType == "Emission") {
          uber_shader->SetBool("hasEmission", true);
          OGLGraphics::SetTexture(3, texture.first);
          uber_shader->SetInt(("material." + texType).c_str(), 3);
        }
      }
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::RenderElements(m.vao, m.numElements);
    }

    // 2nd pass
    OGLGraphics::SetStencilFuncToNotEqual();
    OGLGraphics::SetStencilMask(false);
    OGLGraphics::SetDepthTest(false);
    OGLShader* stencil_shader = DefaultShaders::get_stencilshader();
    float scale = 1.1f;
    glm::mat4 model_matrix = glm::scale(spacial_data.mFinalModelMatrix, glm::vec3(scale));
    stencil_shader->SetMat4("u_model_matrix", model_matrix);
    stencil_shader->SetVec3("u_stencil_color", stencil_color);
    for (MeshInfo& m : mMeshes) {
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::RenderElements(m.vao, m.numElements);
    }

    OGLGraphics::SetStencilMask(true);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);  // todo: abstract
    OGLGraphics::SetDepthTest(true);

  } else {
    OGLShader* shader = DefaultShaders::get_ubershader();
    shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);
    for (MeshInfo& m : mMeshes) {
      shader->SetBool("hasAlbedo", false);
      shader->SetBool("hasSpecular", false);
      shader->SetBool("hasNormal", false);
      shader->SetBool("hasEmission", false);
      for (const auto& texture : m.textureDrawIds) {
        const std::string texType = texture.second;  // get the texture type
        if (texType == "Albedo") {
          shader->SetBool("hasAlbedo", true);
          OGLGraphics::SetTexture(0, texture.first);
          shader->SetInt(("material." + texType).c_str(), 0);
        }
        if (texType == "Specular") {
          shader->SetBool("hasSpecular", true);
          shader->SetInt(("material." + texType).c_str(), 1);
          shader->SetFloat("material.Shininess", m.shininess);
          OGLGraphics::SetTexture(1, texture.first);
        }
        if (texType == "Normal") {
          shader->SetBool("hasNormal", true);
          OGLGraphics::SetTexture(2, texture.first);
          shader->SetInt(("material." + texType).c_str(), 2);
        }
        if (texType == "Emission") {
          shader->SetBool("hasEmission", true);
          OGLGraphics::SetTexture(3, texture.first);
          shader->SetInt(("material." + texType).c_str(), 3);
        }
      }
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::RenderElements(m.vao, m.numElements);
      //todo: we should be setting textures back to 0 or the defaults for that type
    }
  }
}

void Prop::RemoveCache() {
  if (!mMeshes.empty()) {
    MeshLoader::UnloadGameObject(mMeshes);
    mMeshes.clear();
  }
}

void Prop::Load(const std::string& path) {
  if (mMeshes.empty()) {
    if (MeshLoader::LoadGameObjectFromFile(*this, path) == -1) {
      throw ("failed to load path");
    }
  } else {
    throw("Meshes are loaded already. Remove cache first.");
  }
}

}  // end namespace AA