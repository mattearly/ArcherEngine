#pragma once
#include "../Vertex.h"
#include "../MeshInfo.h"
#include "../../../include/AAEngine/Scene/Viewport.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "OGLShader.h"
#include "InternalShaders/Uber.h"
#include "InternalShaders/Stencil.h"
#include "InternalShaders/Skycube.h"
#include "InternalShaders/Shadow.h"
#include "../../../include/AAEngine/Mesh/Prop.h"
#include <cstddef>
#include <glm/ext/matrix_transform.hpp>
#include "../../Scene/Skybox.h"
#include "../../../include/AAEngine/Mesh/AnimProp.h"

namespace AA {

class Prop;
class AnimProp;
class Skybox;

class OGLGraphics final {
public:
  static GLuint UploadStatic3DMesh(const std::vector<LitVertex>& verts, const std::vector<GLuint>& elems);
  static GLuint UploadStatic3DMesh(const std::vector<TanVertex>& verts, const std::vector<GLuint>& elems);
  static GLuint UploadStatic3DMesh(const std::vector<AnimVertex>& verts, const std::vector<GLuint>& elems);
  static GLuint Upload3DPositionsMesh(const float* points, const int num_points, const GLuint* indices, const int ind_count);
  static GLuint Upload2DVerts(const std::vector<glm::vec2>& points);
  static void DeleteMesh(const GLuint& VAO);

  static GLuint Upload2DTex(const unsigned char* tex_data, int width, int height, int format);
  static GLuint UploadCubeMapTex(std::vector<unsigned char*> tex_data, int width, int height, int format);
  static void DeleteTex(const GLuint& id);

  static GLuint CreateDepthMap(GLuint shadow_width, GLuint shadow_height, GLuint& out_depth_map);

  static void SetSamplerCube(int which, const int& cubetexID);
  static void SetTexture(int which, const int& textureID);
  static void DrawElements(unsigned int vao, unsigned int numElements);
  static void DrawStrip(unsigned int vao, const int& count);
  static void SetViewportSize(GLint x, GLint y, GLsizei w, GLsizei h);
  static void SetViewportClearColor(glm::vec3 color) noexcept;

  static void ClearScreen() noexcept;

  static void NewFrame() noexcept;

  static void SetBlend(const bool enabled);
  static void SetCullFace(const bool enabled);
  static void SetCullMode(int mode);
  static void SetDepthTest(const bool enabled);
  static void SetDepthMode(int mode);
  static void SetMultiSampling(const bool enabled);
  static void Proc(void* proc);

  static void SetDepthMask(const bool enabled);
  static void SetStencil(const bool enabled);
  static void SetStencilMask(const bool enabled);
  static void SetStencilOpDepthPassToReplace();
  static void SetStencilFuncToAlways();
  static void SetStencilFuncToNotEqual();

  //
  // render graph stuff
  //

  static void BatchRenderToViewport(
    const std::vector<std::shared_ptr<AA::Prop> >& render_objects, 
    const std::vector<std::shared_ptr<AA::AnimProp> >& animated_render_objects,
    const Viewport& vp) {


    glViewport(vp.BottomLeft[0], vp.BottomLeft[1], vp.Width, vp.Height);
    for (const auto& render_object : render_objects) {
      if (render_object->IsStenciled()) {
        OGLGraphics::RenderStenciled(render_object);
      } else {
        OGLGraphics::RenderNormal(render_object);
      }
    }

    for (const auto& render_object : animated_render_objects) {
      if (render_object->mAnimator) {
        InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
        InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
        auto transforms = render_object->mAnimator->GetFinalBoneMatrices();
        for (unsigned int i = 0; i < transforms.size(); ++i) {
          InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
          InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
        }
      }
      if (render_object->IsStenciled()) {
        OGLGraphics::RenderStenciled(std::dynamic_pointer_cast<AA::Prop>(render_object));
      } else {
        OGLGraphics::RenderNormal(std::dynamic_pointer_cast<AA::Prop>(render_object));
      }
    }

  }

  static void RenderSkybox(const Skybox* skybox_target) {
    if (!skybox_target) { return; }
 
    glDepthFunc(GL_LEQUAL);

    auto skybox_shader = InternalShaders::Skycube::Get();
    
    SetSamplerCube(0, skybox_target->GetCubeMapTexureID());
    DrawElements(skybox_target->GetVAO(), 36);

    ResetToDefault();
  }

  static void ResetToDefault() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glDepthFunc(GL_LESS);
    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // reset all things changed
    //glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);

    //glStencilMask(0x00); // disable writes
    //glStencilMask(0xFF); // enable writes

    if (InternalShaders::Uber::IsActive()) {
      OGLShader* uber_shader = InternalShaders::Uber::Get();
      uber_shader->SetBool("u_has_albedo_tex", false);
      uber_shader->SetBool("u_has_specular_tex", false);
      uber_shader->SetBool("u_has_normal_tex", false);
      uber_shader->SetBool("u_has_emission_tex", false);
      uber_shader->SetBool("u_is_animating", false);

      /*
      uniform mat4 u_projection_matrix;
      uniform mat4 u_view_matrix;
      uniform mat4 u_model_matrix;

      uniform mat4 u_final_bone_mats[MAX_BONES];
      uniform int u_is_animating;

      uniform vec3 u_view_pos;
      uniform int u_has_albedo_tex;
      uniform int u_has_specular_tex;
      uniform int u_has_normal_tex;
      uniform int u_has_emission_tex;
      uniform Material u_material;
      uniform int u_is_dir_light_on;
      uniform DirectionalLight u_dir_light;
      uniform PointLight u_point_lights[MAXPOINTLIGHTS];
      uniform SpotLight u_spot_lights[MAXSPOTLIGHTS];
      uniform int u_num_point_lights_in_use;
      uniform int u_num_spot_lights_in_use;

      */
    }

    if (InternalShaders::Stencil::IsActive()) {
      OGLShader* stencil_shader = InternalShaders::Stencil::Get();
      stencil_shader->SetBool("u_is_animating", false);
      stencil_shader->SetBool("u_stencil_with_normals", false);

      /*
      uniform mat4 u_projection_matrix;
      uniform mat4 u_view_matrix;
      uniform mat4 u_model_matrix;

      uniform mat4 u_final_bone_mats[MAX_BONES];
      uniform int u_stencil_with_normals;
      uniform float u_stencil_scale;
      uniform int u_is_animating;
      */
    }
  }

  static void RenderStenciled(const std::shared_ptr<AA::Prop>& render_object) {
    // 1st pass: render to stencil buffer with normal draw
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    OGLGraphics::SetStencilFuncToAlways();
    OGLGraphics::SetStencilMask(true);
    auto uber_shader = InternalShaders::Uber::Get();
    uber_shader->SetMat4("u_model_matrix", render_object->GetFMM());
    for (const MeshInfo& m : render_object->GetMeshes()) {
      for (const auto& texture : m.textureDrawIds) {
        const std::string texType = texture.second;  // get the texture type
        if (texType == "Albedo") {
          uber_shader->SetBool("u_has_albedo_tex", true);
          OGLGraphics::SetTexture(0, texture.first);
          uber_shader->SetInt(("u_material." + texType).c_str(), 0);
        } else if (texType == "Specular") {
          uber_shader->SetBool("u_has_specular_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 1);
          uber_shader->SetFloat("u_material.Shininess", m.shininess);
          OGLGraphics::SetTexture(1, texture.first);
        } else if (texType == "Normal") {
          uber_shader->SetBool("u_has_normal_tex", true);
          OGLGraphics::SetTexture(2, texture.first);
          uber_shader->SetInt(("u_material." + texType).c_str(), 2);
        } else if (texType == "Emission") {
          uber_shader->SetBool("u_has_emission_tex", true);
          OGLGraphics::SetTexture(3, texture.first);
          uber_shader->SetInt(("u_material." + texType).c_str(), 3);
        }
      }
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::DrawElements(m.vao, m.numElements);
    }
    // 2nd pass
    OGLGraphics::SetStencilFuncToNotEqual();
    OGLGraphics::SetStencilMask(false);
    OGLGraphics::SetDepthTest(false);
    OGLShader* stencil_shader = InternalShaders::Stencil::Get();
    if (render_object->IsStenciledWithNormals()) {
      stencil_shader->SetBool("u_stencil_with_normals", true);
      stencil_shader->SetFloat("u_stencil_scale", render_object->GetStencilScale());
      stencil_shader->SetMat4("u_model_matrix", render_object->GetFMM());
    } else {
      stencil_shader->SetBool("u_stencil_with_normals", false);
      // if stencil scale is less than 1.0 in this case, you won't be able to see it.
      glm::mat4 scaled_stencil_model_matrix = glm::scale(render_object->GetFMM(), glm::vec3(render_object->GetStencilScale()));
      stencil_shader->SetMat4("u_model_matrix", scaled_stencil_model_matrix);
    }
    stencil_shader->SetVec3("u_stencil_color", render_object->GetStencilColor());
    for (const MeshInfo& m : render_object->GetMeshes()) {
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::DrawElements(m.vao, m.numElements);
    }
    OGLGraphics::SetStencilMask(true);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);  // todo: abstract
    OGLGraphics::SetDepthTest(true);
    ResetToDefault();
  }

  static void RenderNormal(const std::shared_ptr<AA::Prop>& render_object) {
    OGLShader* uber_shader = InternalShaders::Uber::Get();
    uber_shader->SetMat4("u_model_matrix", render_object->GetFMM());
    for (const MeshInfo& m : render_object->GetMeshes()) {
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
      OGLGraphics::DrawElements(m.vao, m.numElements);
    }
    ResetToDefault();
  }

private:
  OGLGraphics() = delete;
};
}  // end namespace AA