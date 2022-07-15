#include "Graphics.h"
#include "InternalShaders/Init.h"
#include <Scene/Scene.h>
#include "PrimativeMaker.h"
#include <glm/gtc/matrix_transform.hpp>
namespace AA {

//
// render graph stuff
//
void OpenGL::ResetToDefault() {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDepthFunc(GL_LESS);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void OpenGL::BatchRenderShadows(
  const glm::vec3& view_pos,
  const SunLight& dir_light,
  const std::vector<std::shared_ptr<AA::Scene> >& render_objects) {
  if (!dir_light.Shadows) {
    InternalShaders::Uber::Get()->SetInt("u_dir_light.Shadows", 0);
    return;
  }
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;

  lightProjection = glm::ortho(
    -dir_light.ShadowOrthoSize,
    dir_light.ShadowOrthoSize,
    -dir_light.ShadowOrthoSize,
    dir_light.ShadowOrthoSize,
    dir_light.ShadowNearPlane,
    dir_light.ShadowFarPlane);

  lightView = glm::lookAt(
    view_pos + (200.f * -dir_light.Direction),
    view_pos + glm::vec3(0.0f),
    glm::vec3(0.0, 1.0, 0.0));

  lightSpaceMatrix = lightProjection * lightView;

  // render scene from light's point of view
  auto* depth_shadow_renderer = InternalShaders::Shadow::Get();
  depth_shadow_renderer->SetMat4("u_light_space_matrix", lightSpaceMatrix);

  glViewport(0, 0, dir_light.ShadowWidth, dir_light.ShadowWidth);
  glBindFramebuffer(GL_FRAMEBUFFER, dir_light.GetFBO());
  glClear(GL_DEPTH_BUFFER_BIT);

  bool assume_shadows = false;
  for (const auto& prop : render_objects) {
    if (prop->GetRenderShadows()) {
      assume_shadows = true;  // at least 1 thing has shadows
      depth_shadow_renderer->SetMat4("u_model_matrix", prop->GetFMM());
      bool front_cull = prop->GetCullFrontFaceForShadows();
      if (front_cull) { glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); }
      const auto& meshes = prop->GetMeshes();
      for (const auto& m : meshes) { DrawElements(m.vao, m.numElements); }
      if (front_cull) { glCullFace(GL_BACK); glDisable(GL_CULL_FACE); }
    }
  }

  //for (const auto& anim_prop : animated_render_objects) {
  //  if (anim_prop->GetRenderShadows()) {
  //    assume_shadows = true;  // at least 1 thing has shadows
  //    depth_shadow_renderer->SetMat4("u_model_matrix", anim_prop->GetFMM());
  //    bool front_cull = anim_prop->GetCullFrontFaceForShadows();
  //    if (front_cull) { glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); }
  //    const auto& meshes = anim_prop->GetMeshes();
  //    for (const auto& m : meshes) { DrawElements(m.vao, m.numElements); }
  //    if (front_cull) { glCullFace(GL_BACK); glDisable(GL_CULL_FACE); }
  //  }
  //}

  if (assume_shadows) {  // at least 1 object needs dir light rendered shadows
    auto* uber_shadows = InternalShaders::Uber::Get();
    uber_shadows->SetInt("u_dir_light.Shadows", 1);
    uber_shadows->SetMat4("u_light_space_matrix", lightSpaceMatrix);
    SetTexture(4, dir_light.GetTexID());
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL::BatchRenderToViewport(
  const std::vector<std::shared_ptr<AA::Scene> >& render_objects,
  const Viewport& vp) {
  glViewport(vp.BottomLeft[0], vp.BottomLeft[1], vp.Width, vp.Height);

  for (const auto& render_object : render_objects) {
    if (render_object->IsStenciled()) continue;  // skip, doing stenciled last
    if (render_object->animdata_) RenderAnimProp(render_object);
    else RenderProp(render_object);
  }

  // stencils LAST
  for (const auto& render_object : render_objects) {
    if (!render_object->IsStenciled()) continue;  // skip, already rendered
    if (render_object->animdata_) RenderAnimStenciled(render_object);
    else RenderStenciled(render_object);
  }

  //for (const auto& render_object : animated_render_objects) {
  //  if (render_object->IsStenciled()) continue;  // skip, doing stenciled last
  //  if (render_object->mAnimator) {
  //    InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
  //    InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
  //    auto transforms = render_object->mAnimator->GetFinalBoneMatrices();
  //    for (unsigned int i = 0; i < transforms.size(); ++i) {
  //      InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
  //      InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
  //    }
  //  }
  //  RenderProp(std::dynamic_pointer_cast<AA::Scene>(render_object));
  //  InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
  //  InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
  //}



  //for (const auto& render_object : animated_render_objects) {
  //  if (!render_object->IsStenciled()) continue;
  //  if (render_object->mAnimator) {
  //    InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
  //    InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
  //    auto transforms = render_object->mAnimator->GetFinalBoneMatrices();
  //    for (unsigned int i = 0; i < transforms.size(); ++i) {
  //      InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
  //      InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
  //    }
  //  }
  //  RenderStenciled(std::dynamic_pointer_cast<AA::Scene>(render_object));
  //  InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
  //  InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
  //}
}

// no changes to shaders before rendering the meshes of the object
void OpenGL::RenderAsIs(const std::shared_ptr<AA::Scene>& render_object) {
  const auto& meshes = render_object->GetMeshes();
  for (const auto& m : meshes) {
    DrawElements(m.vao, m.numElements);
  }
}

void OpenGL::RenderProp(const std::shared_ptr<AA::Scene>& render_object) {

  // shader for this render
  OGLShader* uber_shader = InternalShaders::Uber::Get();

  // is the prop set to have shadows on
  if (render_object->GetRenderShadows()) {
    uber_shader->SetInt("u_mesh_does_shadow", 1);
    uber_shader->SetInt("u_shadow_map", 4);
  }

  if (render_object->GetBackFaceCull()) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

  uber_shader->SetMat4("u_model_matrix", render_object->GetFMM());
  for (const MeshInfo& m : render_object->GetMeshes()) {
    for (const auto& texture : m.textureDrawIds) {  // set all the textures on the shader
      switch (texture.textureType) {
      case TextureType::ALBEDO:
        uber_shader->SetBool("u_material.has_albedo_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 0);
        SetTexture(0, texture.accessId);
        break;
      case TextureType::SPECULAR:
        uber_shader->SetBool("u_material.has_specular_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 1);
        uber_shader->SetFloat("u_material.Shininess", m.shininess);
        uber_shader->SetBool("u_reflection_model.BlinnPhong", true);
        SetTexture(1, texture.accessId);
        break;
      case TextureType::NORMAL:
        uber_shader->SetBool("u_material.has_normal_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 2);
        SetTexture(2, texture.accessId);
        break;
      case TextureType::EMISSION:
        uber_shader->SetBool("u_material.has_emission_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 3);
        SetTexture(3, texture.accessId);
        break;
      case TextureType::UNKNOWN:
        break;
      }
    }

    // set non-image-texture materials
    uber_shader->SetVec3("u_material.Tint", m.material.Diffuse);
    uber_shader->SetVec3("u_material.Ambient", m.material.Ambient);
    uber_shader->SetVec3("u_material.SpecularColor", m.material.SpecularColor);
    uber_shader->SetVec3("u_material.EmissionColor", m.material.Emission);

    // draw after setting all these
    DrawElements(m.vao, m.numElements);

    // reset this shader
    uber_shader->SetBool("u_material.has_albedo_tex", false);
    uber_shader->SetBool("u_material.has_specular_tex", false);
    uber_shader->SetBool("u_material.has_normal_tex", false);
    uber_shader->SetBool("u_material.has_emission_tex", false);
    uber_shader->SetFloat("u_material.Shininess", 0.0f);
    uber_shader->SetBool("u_reflection_model.Phong", false);
    uber_shader->SetBool("u_reflection_model.BlinnPhong", false);
    uber_shader->SetBool("u_mesh_does_shadow", false);

  }
  ResetToDefault();
}

void OpenGL::RenderAnimProp(const std::shared_ptr<AA::Scene>& render_object) {
  if (render_object->HasAnimation()) {
    InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
    InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
    auto transforms = render_object->GetFinalBoneMatrices();
    for (unsigned int i = 0; i < transforms.size(); ++i) {
      InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
      InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
    }
  }

  RenderProp(render_object);

  InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
  InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
}

void OpenGL::RenderSkybox(const Skybox* skybox_target, const Viewport& vp) {
  if (!skybox_target) { return; }
  glViewport(vp.BottomLeft[0], vp.BottomLeft[1], vp.Width, vp.Height);
  glDepthFunc(GL_LEQUAL);
  auto skybox_shader = InternalShaders::Skycube::Get();
  SetSamplerCube(0, skybox_target->GetCubeMapTexureID());
  DrawElements(skybox_target->GetVAO(), 36);
  ResetToDefault();
}

void OpenGL::RenderStenciled(const std::shared_ptr<AA::Scene>& render_object) {
  // 1st pass: render to stencil buffer with normal draw

  if (render_object->GetBackFaceCull()) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0xFF);

  auto uber_shader = InternalShaders::Uber::Get();
  uber_shader->SetMat4("u_model_matrix", render_object->GetFMM());
  for (const MeshInfo& m : render_object->GetMeshes()) {

    for (const auto& texture : m.textureDrawIds) {  // set all the textures on the shader
      switch (texture.textureType) {
      case TextureType::ALBEDO:
        uber_shader->SetBool("u_material.has_albedo_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 0);
        SetTexture(0, texture.accessId);
        break;
      case TextureType::SPECULAR:
        uber_shader->SetBool("u_material.has_specular_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 1);
        uber_shader->SetFloat("u_material.Shininess", m.shininess);
        uber_shader->SetBool("u_reflection_model.BlinnPhong", true);
        SetTexture(1, texture.accessId);
        break;
      case TextureType::NORMAL:
        uber_shader->SetBool("u_material.has_normal_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 2);
        SetTexture(2, texture.accessId);
        break;
      case TextureType::EMISSION:
        uber_shader->SetBool("u_material.has_emission_tex", true);
        uber_shader->SetInt("u_material." + toString(texture.textureType), 3);
        SetTexture(3, texture.accessId);
        break;
      case TextureType::UNKNOWN:
        break;
      }
    }

    // set non-image-texture materials
    uber_shader->SetVec3("u_material.Tint", m.material.Diffuse);
    uber_shader->SetVec3("u_material.Ambient", m.material.Ambient);
    uber_shader->SetVec3("u_material.SpecularColor", m.material.SpecularColor);
    uber_shader->SetVec3("u_material.EmissionColor", m.material.Emission);

    //SetCullFace(m.backface_culled);
    DrawElements(m.vao, m.numElements);

    // reset this shader
    uber_shader->SetBool("u_material.has_albedo_tex", false);
    uber_shader->SetBool("u_material.has_specular_tex", false);
    uber_shader->SetBool("u_material.has_normal_tex", false);
    uber_shader->SetBool("u_material.has_emission_tex", false);
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
    DrawElements(m.vao, m.numElements);
  }

  SetStencilMask(true);
  glStencilMask(0xFF);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glEnable(GL_DEPTH_TEST);
  stencil_shader->SetBool("u_stencil_with_normals", false);
  ResetToDefault();
}

void OpenGL::RenderAnimStenciled(const std::shared_ptr<AA::Scene>& render_object) {
  if (render_object->HasAnimation()) {
    InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
    InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
    auto transforms = render_object->GetFinalBoneMatrices();
    for (unsigned int i = 0; i < transforms.size(); ++i) {
      InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
      InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
    }
  }
  RenderStenciled(render_object);
  InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
  InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
}

/// <summary>
/// Debug
/// </summary>
void OpenGL::RenderDebugCube(glm::vec3 loc) {
  glm::mat4 model_matrix = glm::mat4(1);
  model_matrix = glm::translate(model_matrix, loc);
  InternalShaders::Basic::Get()->SetMat4("u_model_matrix", model_matrix);
  DrawElements(MakeCube(), 36);
  ResetToDefault();
}

/// <summary>
/// Debug
/// todo:: test and fix
/// </summary>
void OpenGL::RenderSunLightArrowIcon(glm::vec3 dir_from_00) {

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
  // figure out how to draw an arrow better
  unsigned int cone_vao = MakeCone();
  DrawElements(cone_vao, 65);

  ResetToDefault();
}

/// <summary>
/// Debug RenderSpotLightIcon
/// todo: test and fix
/// </summary>
void OpenGL::RenderSpotLightIcon(glm::vec3 location, glm::vec3 direction) {

  // location and direction 
  // need to check that this is correct
  glm::mat4 model_matrix = glm::mat4(1);
  model_matrix = glm::translate(model_matrix, location);

  model_matrix = glm::rotate(model_matrix, direction.x, glm::vec3(0, 1, 0));
  model_matrix = glm::rotate(model_matrix, direction.y, glm::vec3(0, 0, 1));
  model_matrix = glm::rotate(model_matrix, direction.z, glm::vec3(1, 0, 0));

  InternalShaders::Basic::Get()->SetMat4("u_model_matrix", model_matrix);
  // needs a better icon than a cube
  DrawElements(MakeCube(), 36);
  ResetToDefault();
}

}
