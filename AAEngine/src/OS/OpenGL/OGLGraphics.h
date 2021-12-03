#pragma once
#include "../Vertex.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace AA {

class OGLGraphics {
public:
  static void SetSamplerCube(int which, const int& cubetexID);
  static void SetTexture(int which, const int& textureID);
  static void RenderElements(unsigned int vao, unsigned int numElements);
  static void RenderStrip(unsigned int vao, const int& count);
  static void SetViewportSize(int x, int y, int w, int h);
  static void SetViewportClearColor(glm::vec3 color) noexcept;
  static void ClearScreen() noexcept;
  static GLuint UploadStatic3DMesh(const std::vector<LitVertex>& verts, const std::vector<GLuint>& elems);
  static GLuint UploadStatic3DMesh(const std::vector<TanVertex>& verts, const std::vector<GLuint>& elems);
  static GLuint UploadStatic3DMesh(const std::vector<AnimVertex>& verts, const std::vector<GLuint>& elems);
  static GLuint Upload3DPositionsMesh(const float* points, const int num_points, const GLuint* indices, const int ind_count);
  static GLuint Upload2DVerts(const std::vector<glm::vec2>& points);
  static void DeleteMesh(const GLuint& VAO);
  static GLuint Upload2DTex(const unsigned char* tex_data, int width, int height, bool hasAlpha = true);
  static GLuint UploadCubeMapTex(std::vector<unsigned char*> tex_data, int width, int height, bool hasAlpha = true);
  static void DeleteTex(const GLuint& id);
  static void SetBlend(const bool enabled);
  static void SetCullFace(const bool enabled);
  static void SetCullMode(int mode);
  static void SetDepthTest(const bool enabled);
  static void SetDepthMode(int mode);
  static void SetMultiSampling(const bool enabled);
  static void Proc(void* proc);
private:
  OGLGraphics() = delete;
};
}  // end namespace AA