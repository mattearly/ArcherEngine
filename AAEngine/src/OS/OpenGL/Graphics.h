#pragma once
#include "../Vertex.h"
#include "../../Scene/Lights.h"
#include "../../../include/AAEngine/Scene/Viewport.h"



#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace AA {

class Prop;
class AnimProp;
class Skybox;

namespace OpenGL {

GLuint UploadStatic3DMesh(const std::vector<LitVertex>& verts, const std::vector<GLuint>& elems);
GLuint UploadStatic3DMesh(const std::vector<TanVertex>& verts, const std::vector<GLuint>& elems);
GLuint UploadStatic3DMesh(const std::vector<AnimVertex>& verts, const std::vector<GLuint>& elems);
GLuint Upload3DPositionsMesh(const float* points, const int num_points, const GLuint* indices, const int ind_count);
GLuint Upload2DVerts(const std::vector<glm::vec2>& points);
void DeleteMesh(const GLuint& VAO);

GLuint Upload2DTex(const unsigned char* tex_data, int width, int height, int format);
GLuint UploadCubeMapTex(std::vector<unsigned char*> tex_data, int width, int height, int format);
void DeleteTex(const GLuint& id);

GLuint CreateDepthMap(GLuint shadow_width, GLuint shadow_height, GLuint& out_depth_map);

void SetSamplerCube(int which, const int& cubetexID);
void SetTexture(int which, const int& textureID);
void DrawElements(unsigned int vao, unsigned int numElements);
void DrawStrip(unsigned int vao, const int& count);
void SetViewportSize(GLint x, GLint y, GLsizei w, GLsizei h);
void SetViewportClearColor(glm::vec3 color) noexcept;

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

void Proc(void* proc);


//
// render graph stuff
//
void ResetToDefault();

/// <summary>
/// render depth of scene to texture from light perspective
/// </summary>
/// <param name="dir_light"></param>
/// <param name="render_objects"></param>
/// <param name="animated_render_objects"></param>
/// <param name="depthMapFBO"></param>
void BatchRenderShadows(
  const DirectionalLight& dir_light,
  const std::vector<std::shared_ptr<AA::Prop> >& render_objects,
  const std::vector<std::shared_ptr<AA::AnimProp> >& animated_render_objects,
  GLuint depthMapFBO);

void BatchRenderToViewport(
  const std::vector<std::shared_ptr<AA::Prop> >& render_objects,
  const std::vector<std::shared_ptr<AA::AnimProp> >& animated_render_objects,
  const Viewport& vp, const GLuint& shadow_depth_map_tex);

// no changes to shaders before rendering the meshes of the object
void RenderAsIs(const std::shared_ptr<AA::Prop>& render_object);

void RenderProp(const std::shared_ptr<AA::Prop>& render_object);

void RenderSkybox(const Skybox* skybox_target, const Viewport& vp);

void RenderStenciled(const std::shared_ptr<AA::Prop>& render_object);

/// <summary>
/// Debug
/// </summary>
void RenderDebugCube(glm::vec3 loc);

/// <summary>
/// Debug
/// todo:: test and fix
/// </summary>
void RenderDirectionalLightArrowIcon(glm::vec3 dir_from_00);

/// <summary>
/// Debug RenderSpotLightIcon
/// todo: test and fix
/// </summary>
void RenderSpotLightIcon(glm::vec3 location, glm::vec3 direction);

namespace Primitives {

unsigned int load_cone(unsigned int& out_num_elements);
void unload_cone();
unsigned int load_cube();
void unload_cube();
unsigned int load_plane();
void unload_plane();
void unload_all();

}  // end namespace Primitives
}  // end namespace OpenGL
}  // end namespace AA