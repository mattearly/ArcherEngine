#pragma once
#include <Scene/SunLight.h>
#include <Scene/Viewport.h>
#include "../../Mesh/Vertex.h"
#include "../../Scene/Lights.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/material.h>
#include <vector>
#include <memory>
#include "../../Scene/Skybox.h"

namespace AA {

class Scene;
//class AnimProp;
//class Skybox;

class OpenGL {
public:
  static OpenGL* GetGL();

public:
  GLuint UploadStatic3DMesh(const std::vector<LitVertex>& verts, const std::vector<GLuint>& elems);
  GLuint UploadStatic3DMesh(const std::vector<TanVertex>& verts, const std::vector<GLuint>& elems);
  GLuint UploadStatic3DMesh(const std::vector<AnimVertex>& verts, const std::vector<GLuint>& elems);
  GLuint Upload3DPositionsMesh(const float* points, const int num_points, const GLuint* indices, const int ind_count);
  GLuint Upload2DVerts(const std::vector<glm::vec2>& points);
  void DeleteMesh(const GLsizei num_to_del, const GLuint& VAO);

  GLuint Upload2DTex(const unsigned char* tex_data, int width, int height, int format, aiTextureMapMode map_mode);
  GLuint UploadCubeMapTex(std::vector<unsigned char*> tex_data, int width, int height, int format);
  void DeleteTex(const GLsizei num_to_del, const GLuint& tex_d);

  GLuint CreateDepthMapFBO(GLuint shadow_width, GLuint shadow_height, GLuint& out_depth_map_tex_id);
  void DeleteFramebuffer(const GLsizei num_to_del, GLuint& fbo);

  void SetSamplerCube(int which, const unsigned int& cubetexID);
  void SetTexture(int which, const unsigned int& textureID);
  void DrawElements(unsigned int vao, unsigned int numElements);
  void DrawStrip(unsigned int vao, const int& count);
  void SetViewportSize(GLint x, GLint y, GLsizei w, GLsizei h);
  void SetViewportClearColor(glm::vec3 color) noexcept;
  void SetLogStream(const bool& on_or_off, const bool& file_or_stdout);

  void ClearScreen() noexcept;

  void NewFrame() noexcept;

  void SetBlend(const bool enabled);
  void SetCullFace(const bool enabled);
  void SetCullMode(int mode);
  void SetDepthTest(const bool enabled);
  void SetDepthMode(int mode);
  void SetMultiSampling(const bool enabled);
  void SetGammaCorrection(const bool enabled);

  void SetDepthMask(const bool enabled);
  void SetStencil(const bool enabled);
  void SetStencilMask(const bool enabled);
  void SetStencilOpDepthPassToReplace();
  void SetStencilFuncToAlways();
  void SetStencilFuncToNotEqual();

  unsigned int MakeCube();

  unsigned int MakeCone();

  void DeletePrimatives();

//   void Proc( void(* (*)(const char*))() );
  void Proc(void* proc);

  //
  // render graph stuff
  //
  void ResetToDefault();

  /// <summary>
  /// render depth of scene to texture from light perspective
  /// </summary>
  /// <param name="view_pos"></param>
  /// <param name="dir_light"></param>
  /// <param name="render_objects"></param>
  /// <param name="animated_render_objects"></param>
  /// <param name="depthMapFBO"></param>
  void BatchRenderShadows(
    const glm::vec3& view_pos,
    const SunLight& dir_light,
    const std::vector<std::shared_ptr<AA::Scene> >& render_objects);

  void BatchRenderToViewport(
    const std::vector<std::shared_ptr<AA::Scene> >& render_objects,
    const Viewport& vp);

  // no changes to shaders before rendering the meshes of the object
  void RenderAsIs(const std::shared_ptr<AA::Scene>& render_object);

  void RenderProp(const std::shared_ptr<AA::Scene>& render_object);

  void RenderAnimProp(const std::shared_ptr<AA::Scene>& render_object);

  void RenderSkybox(const Skybox* skybox_target, const Viewport& vp);

  void RenderStenciled(const std::shared_ptr<AA::Scene>& render_object);

  void RenderAnimStenciled(const std::shared_ptr<AA::Scene>& render_object);

  /// <summary>
  /// Debug
  /// </summary>
  void RenderDebugCube(glm::vec3 loc);

  /// <summary>
  /// Debug
  /// todo:: test and fix
  /// </summary>
  void RenderSunLightArrowIcon(glm::vec3 dir_from_00);

  /// <summary>
  /// Debug RenderSpotLightIcon
  /// todo: test and fix
  /// </summary>
  void RenderSpotLightIcon(glm::vec3 location, glm::vec3 direction);

private:
  OpenGL();
};
}  // end namespace AA
