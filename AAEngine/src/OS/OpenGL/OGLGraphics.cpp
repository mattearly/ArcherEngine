#include "OGLGraphics.h"
#include "../MeshInfo.h"
#include "OGLShader.h"
#include "InternalShaders/Uber.h"
#include "InternalShaders/Stencil.h"
#include "InternalShaders/Skycube.h"
#include "InternalShaders/Shadow.h"
#include "InternalShaders/Uber.h"
#include "../../../include/AAEngine/Mesh/Prop.h"
#include "../../Scene/Skybox.h"

#include <glm/ext/matrix_transform.hpp>
#include "glm/glm.hpp"

#include <cstddef>

namespace AA {

void OGLGraphics::SetSamplerCube(int which, const int& cubetexID) {
  glActiveTexture(GL_TEXTURE0 + which);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubetexID);
}

void OGLGraphics::SetTexture(int which, const int& textureID) {
  glActiveTexture(GL_TEXTURE0 + which);
  glBindTexture(GL_TEXTURE_2D, textureID);
}

void OGLGraphics::DrawElements(unsigned int vao, unsigned int numElements) {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void OGLGraphics::DrawStrip(unsigned int vao, const int& count) {
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, count);
  glBindVertexArray(0);
}

void OGLGraphics::SetViewportSize(GLint x, GLint y, GLsizei w, GLsizei h) {
  glViewport(x, y, w, h);
}

/// <summary>
/// sets the color of the opengl viewport background
/// </summary>
/// <param name="vec3">rgb floats between 0 and 1</param>
/// <returns></returns>
void OGLGraphics::SetViewportClearColor(glm::vec3 color) noexcept {
  glClearColor(color.x, color.y, color.z, 0.0f);
}

/// <summary>
/// pre-render function that clears the current context
/// </summary>
void OGLGraphics::ClearScreen() noexcept {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OGLGraphics::NewFrame() noexcept {
  ClearScreen();
  OGLGraphics::ResetToDefault();
}

GLuint OGLGraphics::UploadStatic3DMesh(const std::vector<LitVertex>& verts, const std::vector<GLuint>& elems) {
  bool use_43 = false;
  if (use_43) {
    // 4.3
    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts), &verts[0], GL_STATIC_DRAW);

    GLuint vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    GLuint stride_size = sizeof(LitVertex);

    /* REFERENCE
    void glVertexAttribFormat(	GLuint attribindex,
                  GLint size,
                  GLenum type,
                  GLboolean normalized,
                  GLuint relativeoffset);

    void glVertexAttribBinding(	GLuint attribindex,
                  GLuint bindingindex);

    void glBindVertexBuffer(	GLuint bindingindex,
                  GLuint buffer,
                  GLintptr offset,
                  GLsizei stride);

    void glEnableVertexAttribArray(	GLuint index);
    END REFERENCE */

    // POSITION
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(LitVertex, Position));
    glVertexAttribBinding(0, 0);
    glBindVertexBuffer(0, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(0);

    // TEXCOORDS
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(LitVertex, TexCoords));
    glVertexAttribBinding(1, 1);
    glBindVertexBuffer(1, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(1);

    // NORMAL
    glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, offsetof(LitVertex, Normal));
    glVertexAttribBinding(2, 2);
    glBindVertexBuffer(2, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(2);

    // INDICIES
    GLuint elementBufferHandle;
    glGenBuffers(1, &elementBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(GLuint), &elems[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return vaoHandle;

  } else {
    // old version 3.3
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(LitVertex), &verts[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LitVertex), (const void*)offsetof(LitVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(LitVertex), (const void*)offsetof(LitVertex, TexCoords));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(LitVertex), (const void*)offsetof(LitVertex, Normal));


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(GLuint), &elems[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    return VAO;
  }
}

/// <summary>
/// sends the mesh data to the graphics card
/// </summary>
/// <param name="verts">vertices to upload</param>
/// <param name="elems">relevant indicies</param>
/// <returns>the VAO</returns>
GLuint OGLGraphics::UploadStatic3DMesh(const std::vector<TanVertex>& verts, const std::vector<GLuint>& elems) {
  bool use_43 = false;
  if (use_43) {
    // 4.3
    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts), &verts[0], GL_STATIC_DRAW);

    GLuint vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    GLuint stride_size = sizeof(TanVertex);

    // POSITION
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(TanVertex, Position));
    glVertexAttribBinding(0, 0);
    glBindVertexBuffer(0, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(0);

    // TEXCOORDS
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(TanVertex, TexCoords));
    glVertexAttribBinding(1, 1);
    glBindVertexBuffer(1, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(1);

    // NORMAL
    glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, offsetof(TanVertex, Normal));
    glVertexAttribBinding(2, 2);
    glBindVertexBuffer(2, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(2);

    // TANGENT
    glVertexAttribFormat(3, 3, GL_FLOAT, GL_FALSE, offsetof(TanVertex, Tangent));
    glVertexAttribBinding(3, 3);
    glBindVertexBuffer(3, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(3);

    // BITANGENT
    glVertexAttribFormat(4, 3, GL_FLOAT, GL_FALSE, offsetof(TanVertex, BiTangent));
    glVertexAttribBinding(4, 4);
    glBindVertexBuffer(4, vboHandle, 0, stride_size);
    glEnableVertexAttribArray(4);

    // INDICIES
    GLuint elementBufferHandle;
    glGenBuffers(1, &elementBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(GLuint), &elems[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return vaoHandle;
  } else {
    // old version 3.3
    GLuint VAO, VBO, EBO;
    glGenBuffers(1, &VBO);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(TanVertex), &verts[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TanVertex), (const void*)offsetof(TanVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TanVertex), (const void*)offsetof(TanVertex, TexCoords));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TanVertex), (const void*)offsetof(TanVertex, Normal));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TanVertex), (const void*)offsetof(TanVertex, Tangent));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(TanVertex), (const void*)offsetof(TanVertex, BiTangent));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(GLuint), &elems[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    return VAO;
  }
}

/// <summary>
/// sends the mesh data to the graphics card
/// </summary>
/// <param name="verts">vertices to upload</param>
/// <param name="elems">relevant indicies</param>
/// <returns>the VAO</returns>
GLuint OGLGraphics::UploadStatic3DMesh(const std::vector<AnimVertex>& verts, const std::vector<GLuint>& elems) {
  GLuint VAO, VBO, EBO;
  glGenBuffers(1, &VBO);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(AnimVertex), &verts[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimVertex), (const void*)offsetof(AnimVertex, Position));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(AnimVertex), (const void*)offsetof(AnimVertex, TexCoords));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AnimVertex), (const void*)offsetof(AnimVertex, Normal));
  glVertexAttribIPointer(3, MAX_BONE_INFLUENCE, GL_INT, sizeof(AnimVertex), (void*)offsetof(AnimVertex, m_BoneIDs));
  glVertexAttribPointer(4, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(AnimVertex), (const void*)offsetof(AnimVertex, m_Weights));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(GLuint), &elems[0], GL_STATIC_DRAW);

  glBindVertexArray(0);

  //glDeleteBuffers(1, &VBO);
  //glDeleteBuffers(1, &EBO);

  return VAO;
}

GLuint OGLGraphics::Upload3DPositionsMesh(const float* points, const int num_points, const GLuint* indices, const int ind_count) {
  //4.3 version
  GLuint vaoHandle;
  GLuint vboHandles[1];
  glGenBuffers(1, vboHandles);
  GLuint positionBufferHandle = vboHandles[0];
  glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
  glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(float), points, GL_STATIC_DRAW);
  glGenVertexArrays(1, &vaoHandle);
  glBindVertexArray(vaoHandle);
  glEnableVertexAttribArray(0);
  glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat) * 3);
  glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexAttribBinding(0, 0);
  GLuint elementBufferHandle;
  glGenBuffers(1, &elementBufferHandle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

  glBindVertexArray(0);

  return vaoHandle;

  // old ver 3.3
  //GLuint VAO, VBO, EBO;
  //glGenBuffers(1, &VBO);

  //glGenVertexArrays(1, &VAO);
  //glBindVertexArray(VAO);
  //glBindBuffer(GL_ARRAY_BUFFER, VBO);
  //glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(float), points, GL_STATIC_DRAW);

  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

  //glEnableVertexAttribArray(0);

  //glGenBuffers(1, &EBO);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

  //glBindVertexArray(0);

  //glDeleteBuffers(1, &VBO);
  //glDeleteBuffers(1, &EBO);

  //return VAO;
}

GLuint OGLGraphics::Upload2DVerts(const std::vector<glm::vec2>& points) {
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  std::vector<float> data;
  data.reserve(points.size() * 2);
  for (const auto& p : points) {
    data.push_back(p.x);
    data.push_back(p.y);
  }
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
  glBindVertexArray(0);

  return VAO;
}

/// <summary>
/// removes the mesh from our GPU memory
/// </summary>
/// <param name="VAO">vao to delete</param>
void OGLGraphics::DeleteMesh(const GLuint& VAO) {
  glDeleteBuffers(1, &VAO);
}

GLuint OGLGraphics::Upload2DTex(const unsigned char* tex_data, int width, int height, int format) {
  unsigned int out_texID = 0;
  glGenTextures(1, &out_texID);
  glBindTexture(GL_TEXTURE_2D, out_texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //try: https://stackoverflow.com/questions/23150123/loading-png-with-stb-image-for-opengl-texture-gives-wrong-colors
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // internal format for GL_RGBA should be GL_RGBA8 for reasons https://youtu.be/n4k7ANAFsIQ?t=910
  auto internalformat = (format == GL_RGBA) ? GL_RGBA8 : GL_RGB;
  if (format == GL_RED)
    internalformat = GL_RED;

  if (format == GL_RGBA) {
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, /*border*/0, format, GL_UNSIGNED_BYTE, tex_data);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, /*border*/0, format, GL_UNSIGNED_BYTE, tex_data);
  }

  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  return out_texID;
}

GLuint OGLGraphics::UploadCubeMapTex(std::vector<unsigned char*> tex_data, int width, int height, int format) {
  assert(tex_data.size() == 6);
  GLuint out_texID;
  glGenTextures(1, &out_texID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, out_texID);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  //https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_Cubemap_Texture
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

  auto internalformat = (format == GL_RGBA) ? GL_RGBA8 : GL_RGB;
  if (format == GL_RED)
    internalformat = GL_RED;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data[0]);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data[1]);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data[2]);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data[3]);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data[4]);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data[5]);

  return out_texID;
}

void OGLGraphics::DeleteTex(const GLuint& id) {
  glDeleteTextures(1, &id);
}

void OGLGraphics::SetBlend(const bool enabled) {
  if (enabled) {
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  } else {
    glDisable(GL_BLEND);
  }
}

void OGLGraphics::SetCullFace(const bool enabled) {
  if (enabled) {
    glEnable(GL_CULL_FACE);
  } else {
    glDisable(GL_CULL_FACE);
  }
}

void OGLGraphics::SetCullMode(int mode) {
  glCullFace(static_cast<GLenum>(mode));
}

void OGLGraphics::SetDepthTest(const bool enabled) {
  if (enabled) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
}

void OGLGraphics::SetDepthMode(int mode) {
  glDepthFunc(static_cast<GLenum>(mode));
}

void OGLGraphics::SetMultiSampling(const bool enabled) {
  if (enabled) {
    glEnable(GL_MULTISAMPLE);
  } else {
    glDisable(GL_MULTISAMPLE);
  }
}

void OGLGraphics::SetGammaCorrection(const bool enabled) {
  if (enabled)
    glEnable(GL_FRAMEBUFFER_SRGB);
  else
    glDisable(GL_FRAMEBUFFER_SRGB);
}

void OGLGraphics::Proc(void* proc) {
  if (!gladLoadGLLoader((GLADloadproc)proc)) {
    throw("contexting window to OpenGL failed\n");
  }
}

/// <summary>
/// Buffers and depth map of the specified width and height.
/// derived from https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
/// </summary>
/// <param name="shadow_width"></param>
/// <param name="shadow_height"></param>
/// <returns>depth map FBO for rendering</returns>
GLuint OGLGraphics::CreateDepthMap(GLuint shadow_width, GLuint shadow_height, GLuint& out_depth_map) {
  // create framebuffer object for rendering
  GLuint depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);

  // create 2d texture to use as framebuffer's depth buffer
  const GLuint SHADOW_WIDTH = shadow_width, SHADOW_HEIGHT = shadow_height;
  //GLuint depth_map;
  glGenTextures(1, &out_depth_map);
  glBindTexture(GL_TEXTURE_2D, out_depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // attach to framebuffers depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, out_depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return depthMapFBO;
}

//Note that this only has effect if depth testing is enabled. 
void OGLGraphics::SetDepthMask(const bool enabled) {
  if (enabled)
    glDepthMask(GL_FALSE);
  else
    glDepthMask(GL_TRUE);
}

void OGLGraphics::SetStencil(const bool enabled) {
  if (enabled)
    glEnable(GL_STENCIL_TEST);
  else
    glDisable(GL_STENCIL_TEST);
}

// enable or disable writes
void OGLGraphics::SetStencilMask(const bool enabled) {
  if (enabled)
    glStencilMask(0xFF); // enable writes
  else
    glStencilMask(0x00); // disable writes
}

void OGLGraphics::SetStencilOpDepthPassToReplace() {
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void OGLGraphics::SetStencilFuncToAlways() {
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
}

void OGLGraphics::SetStencilFuncToNotEqual() {
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
}

namespace Primatives {

static unsigned int vao_to_the_cube = 0;

// returns the vao to a -1 by 1 cube
unsigned int load_cube() {
  if (vao_to_the_cube != 0) {
    return vao_to_the_cube;
  }

  const float cubeVertices[] = {
    // front
    -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
    1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // back
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0
  };

  const unsigned int faces[] = {
    // front
    0, 1, 2,
    2, 3, 0,
    // right
    1, 5, 6,
    6, 2, 1,
    // back
    7, 6, 5,
    5, 4, 7,
    // left
    4, 0, 3,
    3, 7, 4,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // top
    3, 2, 6,
    6, 7, 3 };

  vao_to_the_cube = OGLGraphics::Upload3DPositionsMesh(cubeVertices, sizeof(cubeVertices) / sizeof(cubeVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));

  return vao_to_the_cube;
}

void unload_cube() {
  if (vao_to_the_cube != 0) {
    OGLGraphics::DeleteMesh(vao_to_the_cube);
    vao_to_the_cube = 0;
  }
}

static unsigned int vao_to_the_plane = 0;

unsigned int load_plane() {

  if (vao_to_the_plane != 0) {
    return vao_to_the_plane;
  }
  const float SIZE = 1.f;
  const float planeVertices[] = {
    // positions
    -SIZE, 0, SIZE,
    SIZE, 0, SIZE,
    -SIZE, 0, -SIZE,
    SIZE, 0, -SIZE
  };

  unsigned int faces[] = {   //indices
    1,2,0,1,3,2
  };

  vao_to_the_plane = OGLGraphics::Upload3DPositionsMesh(planeVertices, sizeof(planeVertices) / sizeof(planeVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));
  return vao_to_the_plane;
}

void unload_plane() {
  if (vao_to_the_plane != 0) {
    OGLGraphics::DeleteMesh(vao_to_the_plane);
    vao_to_the_plane = 0;
  }
}

void unload_all() {
  unload_cone();
  unload_cube();
  unload_plane();
}


static unsigned int vao_to_the_cone = 0;

unsigned int load_cone(unsigned int& out_num_elements) {

  // todo: fix?
  unsigned int faces[] = {   //indices
    1,1,1,33,2,1,2,3,1,
2,3,2,33,2,2,3,4,2,
3,4,3,33,2,3,4,5,3,
4,5,4,33,2,4,5,6,4,
5,6,5,33,2,5,6,7,5,
6,7,6,33,2,6,7,8,6,
7,8,7,33,2,7,8,9,7,
8,9,8,33,2,8,9,10,8,
9,10,9,33,2,9,10,11,9,
10,11,10,33,2,10,11,12,10,
11,12,11,33,2,11,12,13,11,
12,13,12,33,2,12,13,14,12,
13,14,13,33,2,13,14,15,13,
14,15,14,33,2,14,15,16,14,
15,16,15,33,2,15,16,17,15,
16,17,16,33,2,16,17,18,16,
17,18,17,33,2,17,18,19,17,
18,19,18,33,2,18,19,20,18,
19,20,19,33,2,19,20,21,19,
20,21,20,33,2,20,21,22,20,
21,22,21,33,2,21,22,23,21,
22,23,22,33,2,22,23,24,22,
23,24,23,33,2,23,24,25,23,
24,25,24,33,2,24,25,26,24,
25,26,25,33,2,25,26,27,25,
26,27,26,33,2,26,27,28,26,
27,28,27,33,2,27,28,29,27,
28,29,28,33,2,28,29,30,28,
29,30,29,33,2,29,30,31,29,
30,31,30,33,2,30,31,32,30,
1,33,31,2,34,31,3,35,31,4,36,31,5,37,31,6,38,31,7,39,31,8,40,31,9,41,31,10,42,31,11,43,31,12,44,31,13,45,31,14,46,31,15,47,31,16,48,31,17,49,31,18,50,31,19,51,31,20,52,31,21,53,31,22,54,31,23,55,31,24,56,31,25,57,31,26,58,31,27,59,31,28,60,31,29,61,31,30,62,31,31,63,31,32,64,31,
31,32,32,33,2,32,32,65,32,
32,65,33,33,2,33,1,1,33


  };

  out_num_elements = sizeof(faces) / sizeof(faces[0]);

  if (vao_to_the_cone == 0) {
    // needs loaded
    const float coneVertices[] = {
      // positions
1.000000,0.000007,-1.000000,
0.999999,0.195098,-0.980785,
0.999997,0.382691,-0.923880,
0.999996,0.555578,-0.831470,
0.999995,0.707114,-0.707107,
0.999994,0.831477,-0.555570,
0.999993,0.923887,-0.382683,
0.999993,0.980793,-0.195090,
0.999993,1.000007,0.000000,
0.999993,0.980793,0.195090,
0.999993,0.923887,0.382683,
0.999994,0.831477,0.555570,
0.999995,0.707114,0.707107,
0.999996,0.555578,0.831470,
0.999997,0.382691,0.923880,
0.999999,0.195098,0.980785,
1.000000,0.000007,1.000000,
1.000001,-0.195083,0.980785,
1.000003,-0.382676,0.923880,
1.000004,-0.555563,0.831470,
1.000005,-0.707099,0.707107,
1.000006,-0.831462,0.555570,
1.000007,-0.923872,0.382684,
1.000007,-0.980778,0.195090,
1.000007,-0.999993,-0.000000,
1.000007,-0.980778,-0.195090,
1.000007,-0.923872,-0.382684,
1.000006,-0.831462,-0.555570,
1.000005,-0.707099,-0.707107,
1.000004,-0.555563,-0.831470,
1.000003,-0.382676,-0.923880,
1.000001,-0.195083,-0.980785,
-1.000000,-0.000007,0.000000

    };
    unsigned int num_verts = sizeof(coneVertices) / sizeof(coneVertices[0]);
    vao_to_the_cone = OGLGraphics::Upload3DPositionsMesh(coneVertices, sizeof(coneVertices) / sizeof(coneVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));
  }

  return vao_to_the_cone;
}

void unload_cone() {
  if (vao_to_the_cone != 0) {
    OGLGraphics::DeleteMesh(vao_to_the_cone);
    vao_to_the_cone = 0;
  }
}

}  // end namespace Primatives

}  // end namespace AA