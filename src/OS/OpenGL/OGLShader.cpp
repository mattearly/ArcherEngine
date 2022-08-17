#include "OGLShader.h"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
namespace AA {



const char* OGLShader::get_glsl_version() {
  return (char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS);
}
OGLShader::OGLShader(const char* vert_src, const char* frag_src) {
  loadShader(vert_src, frag_src);
}
OGLShader::~OGLShader() {
  glDeleteProgram(ID);
}

void OGLShader::Use() const noexcept {
  glUseProgram(ID);
}

void OGLShader::stop() const noexcept {
  glUseProgram(0);
}

void OGLShader::SetBool(const std::string& name, bool value) const {
  glUniform1i(getAndCheckShaderUniform(name), (int)value);
}

void OGLShader::SetInt(const std::string& name, int value) const {
  glUniform1i(getAndCheckShaderUniform(name), value);
}

void OGLShader::SetUint(const std::string& name, unsigned int value) const {
  glUniform1ui(getAndCheckShaderUniform(name), value);
}

void OGLShader::SetFloat(const std::string& name, float value) const {
  glUniform1f(getAndCheckShaderUniform(name), value);
}

void OGLShader::SetVec2(const std::string& name, const glm::vec2& value) const {
  glUniform2fv(getAndCheckShaderUniform(name), 1, &value[0]);
}

void OGLShader::SetVec2(const std::string& name, float x, float y) const {
  glUniform2f(getAndCheckShaderUniform(name), x, y);
}

void OGLShader::SetVec3(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(getAndCheckShaderUniform(name), 1, &value[0]);
}

void OGLShader::SetVec3(const std::string& name, float x, float y, float z) const {
  glUniform3f(getAndCheckShaderUniform(name), x, y, z);
}

void OGLShader::SetVec4(const std::string& name, const glm::vec4& value) const {
  glUniform4fv(getAndCheckShaderUniform(name), 1, &value[0]);
}

void OGLShader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
  glUniform4f(getAndCheckShaderUniform(name), x, y, z, w);
}

void OGLShader::SetMat2(const std::string& name, const glm::mat2& mat) const {
  glUniformMatrix2fv(getAndCheckShaderUniform(name), 1, GL_FALSE, &mat[0][0]);
}

void OGLShader::SetMat3(const std::string& name, const glm::mat3& mat) const {
  glUniformMatrix3fv(getAndCheckShaderUniform(name), 1, GL_FALSE, &mat[0][0]);
}

void OGLShader::SetMat4(const std::string& name, const glm::mat4& mat) const {
  glUniformMatrix4fv(getAndCheckShaderUniform(name), 1, GL_FALSE, &mat[0][0]);
}

const int OGLShader::GetID() const {
  return ID;
}

int OGLShader::getAndCheckShaderUniform(const std::string& name) const {
  const int shader_var_id = glGetUniformLocation(ID, name.c_str());

#ifdef _DEBUG
  if (shader_var_id < 0) {
    throw(std::_Xruntime_error, "shader_var_id is invalid (bad name?)");
  }
#endif

  return shader_var_id;
}

void OGLShader::loadShader(const char* vert_source, const char* frag_source) {
  /* compile vertex (points) shader */
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vert_source, nullptr);
  glCompileShader(vertexShader);

  /* determine if vertex shader was successful in compiling */
  int v_success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &v_success);
#ifdef _DEBUG
  if (!v_success) {
    char v_infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, nullptr, v_infoLog);
    throw(std::_Xruntime_error, v_infoLog);
  }
#endif


  /* compile fragment shader */
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &frag_source, nullptr);
  glCompileShader(fragmentShader);

  /* determine if fragment shader was successful in compiling */
  int f_success;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &f_success);
#ifdef _DEBUG
  if (!f_success) {
    char f_infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, nullptr, f_infoLog);
    throw(std::_Xruntime_error, f_infoLog);
  }
#endif


  /* make the shader program */
  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);

  /* check that the ID was successful */
  int p_success;
  glGetProgramiv(ID, GL_LINK_STATUS, &p_success);
#ifdef _DEBUG
  if (!p_success) {
    char p_infoLog[512];
    glGetProgramInfoLog(ID, 512, nullptr, p_infoLog);
    throw(std::_Xruntime_error, p_infoLog);
  }
#endif

  /* we don't need them anymore */
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);


  // old debug, todo if needed
  //QueryInputAttribs(ID);
  //QueryUniforms(ID);
}

}  // end namespace AA
