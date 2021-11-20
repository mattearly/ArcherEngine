#pragma once
#include <string>
#include <glm\glm.hpp>
namespace AA {
class OGLShader {
public:
  static const char* get_glsl_version();

  OGLShader(const char* vert_src, const char* frag_src);
  ~OGLShader();
  const int GetID() const;

  void Use() const noexcept;

  //uniform setters
  void SetBool(const std::string& name, bool value) const;
  void SetInt(const std::string& name, int value) const;
  void SetUint(const std::string& name, unsigned int value) const;
  void SetFloat(const std::string& name, float value) const;

  void SetVec2(const std::string& name, const glm::vec2& value) const;
  void SetVec2(const std::string& name, float x, float y) const;

  void SetVec3(const std::string& name, const glm::vec3& value) const;
  void SetVec3(const std::string& name, float x, float y, float z) const;

  void SetVec4(const std::string& name, const glm::vec4& value) const;
  void SetVec4(const std::string& name, float x, float y, float z, float w) const;

  void SetMat2(const std::string& name, const glm::mat2& mat) const;
  void SetMat3(const std::string& name, const glm::mat3& mat) const;
  void SetMat4(const std::string& name, const glm::mat4& mat) const;

private:

  void stop() const noexcept;  // probably not used

  int getAndCheckShaderUniform(const std::string& name) const;

  OGLShader() = delete;

  unsigned int ID;

  void loadShader(const char* vert, const char* frag);


};
}  // end namespace AA