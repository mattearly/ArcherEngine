#include "OGLShader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#ifdef _DEBUG
#include <iostream>
//#include "QueryShader.h"
#endif
namespace AA {



const char* OGLShader::get_glsl_version() {
    return (char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS);
}
OGLShader::OGLShader(const char* vert_src, const char* frag_src) {
#ifdef _DEBUG
  std::cout << "SHADER ADDED (vert/frag)\n";
#endif
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

  if (shader_var_id < 0) {
    throw("shader_var_id is invalid (bad name?)");
  }

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
  if (!v_success) {
#ifdef _DEBUG
    char v_infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, nullptr, v_infoLog);
    throw(v_infoLog);
    std::cout << "error in vertex shader, compilation failed: " << v_infoLog << std::endl;
    char a;
    std::cin >> a;
#endif
    //exit(-1);
  }

  /* compile fragment shader */
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &frag_source, nullptr);
  glCompileShader(fragmentShader);

  /* determine if fragment shader was successful in compiling */
  int f_success;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &f_success);
  if (!f_success) {
#ifdef _DEBUG
    char f_infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, nullptr, f_infoLog);
    throw(f_infoLog);
    std::cout << "error in fragment shader, compilation failed: " << f_infoLog << std::endl;
    char a;
    std::cin >> a;
#endif
    //exit(-1);
  }

  /* make the shader program */
  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);

  /* check that the ID was successful */
  int p_success;
  glGetProgramiv(ID, GL_LINK_STATUS, &p_success);
  if (!p_success) {
#ifdef _DEBUG
    char p_infoLog[512];
    glGetProgramInfoLog(ID, 512, nullptr, p_infoLog);
    throw(p_infoLog);
    //std::cout << "error in ID: " << p_infoLog << std::endl;
    //char a;
    //std::cin >> a;
#endif
    //exit(-1);
  }

  /* we don't need them anymore */
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

#ifdef _DEBUG
  //QueryInputAttribs(ID);
  //QueryUniforms(ID);
#endif
}

}  // end namespace AA