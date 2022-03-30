#include "Prop.h"
#include "../OS/OpenGL/OGLGraphics.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/OpenGL/InternalShaders/Uber.h"
#include "../OS/OpenGL/InternalShaders/Stencil.h"
#include "../OS/MeshLoader.h"
#include <glm/gtx/transform.hpp>

namespace AA {

Prop::Prop() {
  stenciled = false;
  stencil_color = glm::vec3(0.1f, 0.87f, 0.1f);
  stenciled_with_normals = false;
  stencil_scale = 1.1f;
  cached_load_path.clear();
}

Prop::Prop(const char* path) {
  Load(path);
  stenciled = false;
  stencil_color = glm::vec3(0.1f, 0.87f, 0.1f);
  stenciled_with_normals = false;
  stencil_scale = 1.1f;
}

void Prop::Draw() {
    OGLShader* uber_shader = InternalShaders::Uber::Get();

  if (stenciled) {
    // 1st pass: render to stencil buffer with normal draw
    OGLGraphics::SetStencilFuncToAlways();
    OGLGraphics::SetStencilMask(true);
    uber_shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);
    for (MeshInfo& m : mMeshes) {
      for (const auto& texture : m.textureDrawIds) {
        const std::string texType = texture.second;  // get the texture type
        if (texType == "Albedo") {
          uber_shader->SetBool("u_has_albedo_tex", true);
          OGLGraphics::SetTexture(0, texture.first);
          uber_shader->SetInt(("u_material." + texType).c_str(), 0);
        }
        if (texType == "Specular") {
          uber_shader->SetBool("u_has_specular_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 1);
          uber_shader->SetFloat("u_material.Shininess", m.shininess);
          OGLGraphics::SetTexture(1, texture.first);
        }
        if (texType == "Normal") {
          uber_shader->SetBool("u_has_normal_tex", true);
          OGLGraphics::SetTexture(2, texture.first);
          uber_shader->SetInt(("u_material." + texType).c_str(), 2);
        }
        if (texType == "Emission") {
          uber_shader->SetBool("u_has_emission_tex", true);
          OGLGraphics::SetTexture(3, texture.first);
          uber_shader->SetInt(("u_material." + texType).c_str(), 3);
        }
      }
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::RenderElements(m.vao, m.numElements);
      OGLGraphics::SetTexture(0, 0);
      OGLGraphics::SetTexture(1, 0);
      OGLGraphics::SetTexture(2, 0);
      OGLGraphics::SetTexture(3, 0);
      uber_shader->SetBool("u_has_albedo_tex", false);
      uber_shader->SetBool("u_has_specular_tex", false);
      uber_shader->SetBool("u_has_normal_tex", false);
      uber_shader->SetBool("u_has_emission_tex", false);
    }
    // 2nd pass
    OGLGraphics::SetStencilFuncToNotEqual();
    OGLGraphics::SetStencilMask(false);
    OGLGraphics::SetDepthTest(false);
    OGLShader* stencil_shader = InternalShaders::Stencil::Get();
    if (stenciled_with_normals) {
      stencil_shader->SetBool("u_stencil_with_normals", true);
      stencil_shader->SetFloat("u_stencil_scale", stencil_scale);
      stencil_shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);
    } else {
      stencil_shader->SetBool("u_stencil_with_normals", false);
      // if stencil scale is less than 1.0 in this case, you won't be able to see it.
      glm::mat4 scaled_stencil_model_matrix = glm::scale(spacial_data.mFinalModelMatrix, glm::vec3(stencil_scale));
      stencil_shader->SetMat4("u_model_matrix", scaled_stencil_model_matrix);
    }
    stencil_shader->SetVec3("u_stencil_color", stencil_color);
    for (MeshInfo& m : mMeshes) {
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::RenderElements(m.vao, m.numElements);
    }
    OGLGraphics::SetStencilMask(true);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);  // todo: abstract
    OGLGraphics::SetDepthTest(true);
  } else {
    OGLGraphics::SetStencilMask(false);
    uber_shader->SetMat4("u_model_matrix", spacial_data.mFinalModelMatrix);
    for (MeshInfo& m : mMeshes) {
      for (const auto& texture : m.textureDrawIds) {
        const std::string texType = texture.second;  // get the texture type
        if (texType == "Albedo") {  //todo, improve comparison performance
          uber_shader->SetBool("u_has_albedo_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 0);
          OGLGraphics::SetTexture(0, texture.first);
        }
        if (texType == "Specular") {
          uber_shader->SetBool("u_has_specular_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 1);
          uber_shader->SetFloat("u_material.Shininess", m.shininess);
          OGLGraphics::SetTexture(1, texture.first);
        }
        if (texType == "Normal") {
          uber_shader->SetBool("u_has_normal_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 2);
          OGLGraphics::SetTexture(2, texture.first);
        }
        if (texType == "Emission") {
          uber_shader->SetBool("u_has_emission_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 3);
          OGLGraphics::SetTexture(3, texture.first);
        }
      }
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::RenderElements(m.vao, m.numElements);
      //set textures back to 0 or the defaults for that type
      OGLGraphics::SetTexture(0, 0);
      OGLGraphics::SetTexture(1, 0);
      OGLGraphics::SetTexture(2, 0);
      OGLGraphics::SetTexture(3, 0);
      uber_shader->SetBool("u_has_albedo_tex", false);
      uber_shader->SetBool("u_has_specular_tex", false);
      uber_shader->SetBool("u_has_normal_tex", false);
      uber_shader->SetBool("u_has_emission_tex", false);
    }
  }
}

void Prop::RemoveCache() {
  if (!mMeshes.empty()) {
    MeshLoader::UnloadGameObject(mMeshes, cached_load_path);
    mMeshes.clear();
  }
}

/// <summary>
/// if this doesn't throw, assume load successful
/// </summary>
/// <param name="path">path to model to load and cache</param>
void Prop::Load(const std::string& path) {
  if (!mMeshes.empty())
    throw(std::_Xruntime_error, "Meshes are loaded already. Remove cache first.");

  int load_code = MeshLoader::LoadGameObjectFromFile(*this, path);
  if (load_code == 0) {  // loaded from path
    cached_load_path = path;
  } else if (load_code == 1) {  // reused already loaded cache
    cached_load_path = path;
  } else if (load_code == -1) {
    throw(std::_Xruntime_error, "scene was null");
  } else if (load_code == -2) {
    throw(std::_Xruntime_error, "scene has incomplete flags");
  } else if (load_code == -3) {
    throw(std::_Xruntime_error, "scene has incomplete flags");
  }
}

}  // end namespace AA