#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/quaternion.h>
#include <assimp/color4.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <foundation/PxTransform.h>

namespace AA {

inline glm::mat4 aiMat4_to_glmMat4(const aiMatrix4x4& inMat) {
  glm::mat4 outMat{};
  outMat[0][0] = inMat.a1;
  outMat[1][0] = inMat.b1;
  outMat[2][0] = inMat.c1;
  outMat[3][0] = inMat.d1;
  outMat[0][1] = inMat.a2;
  outMat[1][1] = inMat.b2;
  outMat[2][1] = inMat.c2;
  outMat[3][1] = inMat.d2;
  outMat[0][2] = inMat.a3;
  outMat[1][2] = inMat.b3;
  outMat[2][2] = inMat.c3;
  outMat[3][2] = inMat.d3;
  outMat[0][3] = inMat.a4;
  outMat[1][3] = inMat.b4;
  outMat[2][3] = inMat.c4;
  outMat[3][3] = inMat.d4;
  //glm::transpose(outMat);
  return outMat;
}

inline glm::vec3 aiVec3_to_glmVec3(const aiVector3D& inVec) noexcept {
  glm::vec3 outVec{};
  outVec.x = inVec.x;
  outVec.y = inVec.y;
  outVec.z = inVec.z;
  return outVec;
}

inline glm::vec4 aiColor4_to_glmVec4(const aiColor4D& inVec) noexcept {
  glm::vec4 outVec{};
  outVec.x = inVec.r;
  outVec.y = inVec.g;
  outVec.z = inVec.b;
  outVec.w = inVec.a;

  return outVec;
}

inline glm::quat aiQuat_to_glmQuat(const aiQuaternion& inQuat) noexcept {
  glm::quat outQuat{};
  outQuat.w = inQuat.w;
  outQuat.x = inQuat.x;
  outQuat.y = inQuat.y;
  outQuat.z = inQuat.z;
  return outQuat;
}

inline void physxTransform_to_glmMat4(const physx::PxTransform& in, glm::mat4& out) {
  glm::vec3 pos = glm::vec3(in.p.x, in.p.y, in.p.z);
  glm::quat ori = glm::quat(in.q.x, in.q.y, in.q.z, in.q.w);

  glm::mat4 rot = glm::toMat4(ori);
  glm::mat4 trans = glm::translate(glm::mat4(1), pos);

  out = trans * rot;

}

static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
  glm::mat4 to;
  //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
  to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
  to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
  to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
  to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
  return to;
}

static inline glm::vec3 GetGLMVec(const aiVector3D& vec) {
  return glm::vec3(vec.x, vec.y, vec.z);
}

static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation) {
  return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}


}// namespace AA