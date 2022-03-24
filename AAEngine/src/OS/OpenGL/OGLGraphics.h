#pragma once
#include "../Vertex.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace AA {

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

  static GLuint CreateDepthMap(GLuint shadow_width, GLuint shadow_height);

  static void SetSamplerCube(int which, const int& cubetexID);
  static void SetTexture(int which, const int& textureID);
  static void RenderElements(unsigned int vao, unsigned int numElements);
  static void RenderStrip(unsigned int vao, const int& count);
  static void SetViewportSize(GLint x, GLint y, GLsizei w, GLsizei h);
  static void SetViewportClearColor(glm::vec3 color) noexcept;

  static void ClearScreen() noexcept;

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

private:
  OGLGraphics() = delete;
};
}  // end namespace AA