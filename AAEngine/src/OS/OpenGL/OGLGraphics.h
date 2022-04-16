#pragma once
#include "../../../include/AAEngine/Scene/Viewport.h"
#include "../../../include/AAEngine/Mesh/Prop.h"
#include "../../../include/AAEngine/Mesh/AnimProp.h"
#include "../../Scene/Skybox.h"
#include "../Vertex.h"
#include "../MeshInfo.h"
#include "OGLShader.h"

#include "InternalShaders/Uber.h"
#include "InternalShaders/Stencil.h"
#include "InternalShaders/Skycube.h"
#include "InternalShaders/Shadow.h"
#include "InternalShaders/Basic.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>
#include <vector>
#include <memory>
#include <cmath>

namespace AA {

class Prop;
class AnimProp;
class Skybox;

namespace Primatives {
unsigned int load_cone(unsigned int& out_num_elements);
void unload_cone();
unsigned int load_cube();
void unload_cube();
unsigned int load_plane();
void unload_plane();
void unload_all();
}

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
  static void ResetToDefault() {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  }

  static void BatchRenderToViewport(const std::vector<std::shared_ptr<AA::Prop> >& render_objects, const std::vector<std::shared_ptr<AA::AnimProp> >& animated_render_objects, const Viewport& vp) {
    glViewport(vp.BottomLeft[0], vp.BottomLeft[1], vp.Width, vp.Height);
    for (const auto& render_object : render_objects) {
      if (render_object->IsStenciled()) continue;  // skip, doing stenciled last
      OGLGraphics::RenderProp(render_object);
    }
    for (const auto& render_object : animated_render_objects) {
      if (render_object->IsStenciled()) continue;  // skip, doing stenciled last
      if (render_object->mAnimator) {
        InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
        InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
        auto transforms = render_object->mAnimator->GetFinalBoneMatrices();
        for (unsigned int i = 0; i < transforms.size(); ++i) {
          InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
          InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
        }
      }
      OGLGraphics::RenderProp(std::dynamic_pointer_cast<AA::Prop>(render_object));
      InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
      InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
    }

    // stencils LAST
    for (const auto& render_object : render_objects) {
      if (render_object->IsStenciled()) {
        OGLGraphics::RenderStenciled(render_object);
      }
    }

    for (const auto& render_object : animated_render_objects) {
      if (render_object->IsStenciled()) {
        if (render_object->mAnimator) {
          InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
          InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
          auto transforms = render_object->mAnimator->GetFinalBoneMatrices();
          for (unsigned int i = 0; i < transforms.size(); ++i) {
            InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
            InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
          }
        }
        OGLGraphics::RenderStenciled(std::dynamic_pointer_cast<AA::Prop>(render_object));
        InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
        InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
      }
    }
  }

  static void RenderProp(const std::shared_ptr<AA::Prop>& render_object) {
    OGLShader* uber_shader = InternalShaders::Uber::Get();
    uber_shader->SetMat4("u_model_matrix", render_object->GetFMM());
    for (const MeshInfo& m : render_object->GetMeshes()) {
      for (const auto& texture : m.textureDrawIds) {
        const std::string texType = texture.second;  // get the texture type
        if (texType == "Albedo") {
          uber_shader->SetBool("u_has_albedo_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 0);
          OGLGraphics::SetTexture(0, texture.first);
        } else if (texType == "Specular") {
          uber_shader->SetBool("u_has_specular_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 1);
          uber_shader->SetFloat("u_material.Shininess", m.shininess);
          uber_shader->SetBool("u_reflection_model.BlinnPhong", true);
          OGLGraphics::SetTexture(1, texture.first);
        } else if (texType == "Normal") {
          uber_shader->SetBool("u_has_normal_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 2);
          OGLGraphics::SetTexture(2, texture.first);
        } else if (texType == "Emission") {
          uber_shader->SetBool("u_has_emission_tex", true);
          uber_shader->SetInt(("u_material." + texType).c_str(), 3);
          OGLGraphics::SetTexture(3, texture.first);
        }
      }
      OGLGraphics::SetCullFace(m.backface_culled);
      OGLGraphics::DrawElements(m.vao, m.numElements);

      // reset this shader
      uber_shader->SetBool("u_has_albedo_tex", false);
      uber_shader->SetBool("u_has_specular_tex", false);
      uber_shader->SetBool("u_has_normal_tex", false);
      uber_shader->SetBool("u_has_emission_tex", false);
      uber_shader->SetFloat("u_material.Shininess", 0.0f);
      uber_shader->SetBool("u_reflection_model.Phong", false);
      uber_shader->SetBool("u_reflection_model.BlinnPhong", false);
    }
    ResetToDefault();
  }

  static void RenderSkybox(const Skybox* skybox_target) {
    if (!skybox_target) { return; }
    glDepthFunc(GL_LEQUAL);
    auto skybox_shader = InternalShaders::Skycube::Get();
    SetSamplerCube(0, skybox_target->GetCubeMapTexureID());
    DrawElements(skybox_target->GetVAO(), 36);
    ResetToDefault();
  }

  static void RenderStenciled(const std::shared_ptr<AA::Prop>& render_object) {
    // 1st pass: render to stencil buffer with normal draw

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

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
          uber_shader->SetBool("u_reflection_model.BlinnPhong", true);
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

      // reset this shader
      uber_shader->SetBool("u_has_albedo_tex", false);
      uber_shader->SetBool("u_has_specular_tex", false);
      uber_shader->SetBool("u_has_normal_tex", false);
      uber_shader->SetBool("u_has_emission_tex", false);
      uber_shader->SetFloat("u_material.Shininess", 0.0f);
      uber_shader->SetBool("u_reflection_model.Phong", false);
      uber_shader->SetBool("u_reflection_model.BlinnPhong", false);
    }

    // 2nd pass
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // disable writing to the stencil buffer
    glDisable(GL_DEPTH_TEST);
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
      OGLGraphics::DrawElements(m.vao, m.numElements);
    }

    OGLGraphics::SetStencilMask(true);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
    stencil_shader->SetBool("u_stencil_with_normals", false);
    ResetToDefault();
  }
  
  /// <summary>
  /// Debug
  /// </summary>
  static void RenderWhiteCubeAt(glm::vec3 loc) {
    glm::mat4 model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, loc);
    InternalShaders::Basic::Get()->SetMat4("u_model_matrix", model_matrix);
    OGLGraphics::DrawElements(Primatives::load_cube(), 36);
    ResetToDefault();
  }

  /// <summary>
  /// Debug
  /// todo:: test and fix
  /// </summary>
  static void RenderDirectionalLightArrowIcon(glm::vec3 dir_from_00) {

    // todo: math to spin the arrow icon & keep it at some portion of the screen
    // the below math is WRONG!
    glm::vec3 rot_angles_in_radians = glm::vec3(0);

    rot_angles_in_radians.x = -dir_from_00.x;
    rot_angles_in_radians.y = -dir_from_00.y;
    rot_angles_in_radians.z = -dir_from_00.z;

    glm::mat4 model_matrix = glm::mat4(1);
    model_matrix = glm::rotate(model_matrix, rot_angles_in_radians.x, glm::vec3(0, 1, 0));
    model_matrix = glm::rotate(model_matrix, rot_angles_in_radians.y, glm::vec3(0, 0, 1));
    model_matrix = glm::rotate(model_matrix, rot_angles_in_radians.z, glm::vec3(1, 0, 0));

    // update and draw
    InternalShaders::Basic::Get()->SetMat4("u_model_matrix", model_matrix);
    unsigned int num_elements;
    // figure out how to draw an arrow better
    unsigned int cone_vao = Primatives::load_cone(num_elements);
    OGLGraphics::DrawElements(cone_vao, num_elements);

    ResetToDefault();
  }

  /// <summary>
  /// Debug RenderSpotLightIcon
  /// todo: test and fix
  /// </summary>
  static void RenderSpotLightIcon(glm::vec3 location, glm::vec3 direction) {
   
    // location and direction 
    // need to check that this is correct
    glm::mat4 model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, location);

    model_matrix = glm::rotate(model_matrix, direction.x, glm::vec3(0, 1, 0));
    model_matrix = glm::rotate(model_matrix, direction.y, glm::vec3(0, 0, 1));
    model_matrix = glm::rotate(model_matrix, direction.z, glm::vec3(1, 0, 0));
    
    InternalShaders::Basic::Get()->SetMat4("u_model_matrix", model_matrix);
    // needs a better icon than a cube
    OGLGraphics::DrawElements(Primatives::load_cube(), 36);
    ResetToDefault();
  }

  
private:
  OGLGraphics() = delete;
};
}  // end namespace AA