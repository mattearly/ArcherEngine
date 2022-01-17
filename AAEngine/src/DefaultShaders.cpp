#include "DefaultShaders.h"
namespace AA {

OGLShader* UBERSHADER;
OGLShader* STENCILSHADER;

OGLShader* DefaultShaders::get_ubershader() {
  if (!UBERSHADER)
    init_ubershader();
  UBERSHADER->Use();
  return UBERSHADER;
}

void DefaultShaders::init_stencilshader()
{
  if (STENCILSHADER)
    return;

  const std::string VERT_CODE = R"(#version 430 core
layout(location=0)in vec3 inPos;
layout(location=3)in ivec4 inBoneIds;
layout(location=4)in vec4 inWeights;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform int isAnimating;
void main(){
  vec4 totalPosition = vec4(0.0);
  if (isAnimating > 0) {
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
      if(inBoneIds[i] == -1) continue;
      if(inBoneIds[i] >= MAX_BONES) {
          totalPosition = vec4(inPos, 1.0f);
          break;
      }
      vec4 localPosition = finalBonesMatrices[inBoneIds[i]] * vec4(inPos,1.0);
      totalPosition += localPosition * inWeights[i];
    }
    pass_Pos = (u_model_matrix * totalPosition).xyz;
  } else {  // Not Animating
    pass_Pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
    totalPosition = vec4(inPos, 1.0);
  }
  mat4 viewMatrix = u_view_matrix * u_model_matrix;
  gl_Position = u_projection_matrix * viewMatrix * totalPosition;
})";

  const std::string FRAG_CODE = R"(#version 430
out vec4 FragColor;
uniform vec3 u_stencil_color;
void main() {
  FragColor = vec4(u_stencil_color, 1.0);
})";

  STENCILSHADER = new OGLShader(VERT_CODE.c_str(), FRAG_CODE.c_str());
}

OGLShader* DefaultShaders::get_stencilshader() {
  if (!STENCILSHADER)
    init_stencilshader();
  STENCILSHADER->Use();
  return STENCILSHADER;
}

void DefaultShaders::init_ubershader() {
  if (UBERSHADER)
    return;

  const std::string UBERSHADER_VERT_CODE = R"(
#version 430 core
layout(location=0)in vec3 inPos;
layout(location=1)in vec2 inTexUV;
layout(location=2)in vec3 inNorm;
layout(location=3)in ivec4 inBoneIds;
layout(location=4)in vec4 inWeights;

layout(location=0)out vec3 pass_Pos;
layout(location=1)out vec2 pass_TexUV;
layout(location=2)out vec3 pass_Norm;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform int isAnimating;

void main(){
  pass_TexUV = inTexUV;
  vec4 totalPosition = vec4(0.0);

  if (isAnimating > 0) {
    vec3 totalNormal = vec3(0.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
      if(inBoneIds[i] == -1) continue;
      if(inBoneIds[i] >= MAX_BONES) {
          totalPosition = vec4(inPos, 1.0f);
          break;
      }
      vec4 localPosition = finalBonesMatrices[inBoneIds[i]] * vec4(inPos,1.0);
      totalPosition += localPosition * inWeights[i];
      //totalNormal += mat3(finalBonesMatrices[inBoneIds[i]]) * inNorm;
    }
    pass_Pos = (u_model_matrix * totalPosition).xyz;
  } else {  // Not Animating
    mat3 normal_matrix = transpose(inverse(mat3(u_model_matrix)));
    pass_Norm = normalize(normal_matrix * inNorm);
    pass_Pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
    totalPosition = vec4(inPos, 1.0);
  }
  mat4 viewMatrix = u_view_matrix * u_model_matrix;
  gl_Position = u_projection_matrix * viewMatrix * totalPosition;
})";

  const std::string UBERSHADER_FRAG_CODE = R"(
#version 430 core
layout(location=0)in vec3 pass_Pos;
layout(location=1)in vec2 pass_TexUV;
layout(location=2)in vec3 pass_Norm;

layout(location=0)out vec4 out_Color;

struct Material {
  sampler2D Albedo;
  sampler2D Specular;
  sampler2D Normal;
  sampler2D Emission;
  float Shininess;
};
struct DirectionalLight {
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
};
struct PointLight {
  vec3 Position;
  float Constant, Linear, Quadratic;
  vec3 Ambient, Diffuse, Specular;
};
struct SpotLight {
  vec3 Position, Direction;
  float CutOff, OuterCutOff;
  float Constant, Linear, Quadratic;
  vec3 Ambient, Diffuse, Specular;
};

const vec3 default_color = vec3(0.1);
const int MAXPOINTLIGHTS = 24; // if changed, needs to match on light controllers
const int MAXSPOTLIGHTS = 12;

uniform vec3 u_view_pos;
uniform int hasAlbedo;
uniform int hasSpecular;
uniform int hasNormal;
uniform int hasEmission;
uniform Material material;
uniform int isDirectionalLightOn;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight[MAXPOINTLIGHTS];
uniform SpotLight spotLight[MAXSPOTLIGHTS];
uniform int NUM_POINT_LIGHTS;
uniform int NUM_SPOT_LIGHTS;

vec3 CalcDirectionalLight(vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-directionalLight.Direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.);
  // specular shading
  float spec;
  if (hasSpecular > 0) {
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.), material.Shininess);
  }
  // combine results
  vec3 ambient;
  vec3 diffuse;
  if (hasAlbedo > 0) { 
    ambient = directionalLight.Ambient * texture(material.Albedo, pass_TexUV).rgb;
    diffuse = directionalLight.Diffuse * diff * texture(material.Albedo, pass_TexUV).rgb;
  } else {
    ambient = directionalLight.Ambient * default_color;
    diffuse = directionalLight.Diffuse * diff * default_color;
  }
  if (hasSpecular > 0) {
    vec3 specular = directionalLight.Specular * spec * texture(material.Specular, pass_TexUV).rgb;
    return(ambient + diffuse + specular);
  } else {
    return(ambient + diffuse);
  }
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir){
  vec3 lightDir = normalize(light.Position - pass_Pos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  float spec;
  if (hasSpecular > 0) {
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
  }
  // attenuation
  float dist = length(light.Position - pass_Pos);
  float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));
  // combine results
  vec3 ambient;
  vec3 diffuse;
  if (hasAlbedo > 0) {
    ambient = light.Ambient * texture(material.Albedo, pass_TexUV).rgb;
    diffuse = light.Diffuse * diff * texture(material.Albedo, pass_TexUV).rgb;
  } else {
    ambient = light.Ambient * default_color;
    diffuse = light.Diffuse * diff * default_color;
  }
  ambient *= attenuation;
  diffuse *= attenuation;
  vec3 specular;
  if (hasSpecular > 0) {
    specular = light.Specular * spec * texture(material.Specular, pass_TexUV).rgb;
  } else {
    specular = vec3(1,1,1);
  }
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir){
  vec3 lightDir = normalize(light.Position - pass_Pos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shaing
  float spec;
  if (hasSpecular > 0) {
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
  }
  // attenuation
  float dist = length(light.Position - pass_Pos);
  float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));
  // cone of light
  float theta = dot(lightDir, normalize(-light.Direction));
  float epsilon = light.CutOff - light.OuterCutOff;
  float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);
  // combine results
   vec3 ambient;
   vec3 diffuse;
  if (hasAlbedo > 0) {
    ambient = light.Ambient * texture(material.Albedo, pass_TexUV).rgb;
    diffuse = light.Diffuse * diff * texture(material.Albedo, pass_TexUV).rgb;
  } else {
    ambient = light.Ambient * default_color;
    diffuse = light.Diffuse * diff * default_color;
  }
  if (hasSpecular > 0) {
    vec3 specular = light.Specular * spec * texture(material.Specular, pass_TexUV).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
  } else {
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    return (ambient + diffuse);
  }
}

void main()
{
  vec3 normal;
  if (hasNormal > 0) {
    normal = texture(material.Normal, pass_TexUV).rgb;
    normal = normalize(normal * 2.0 - 1.0);
  } else {
    normal = normalize(pass_Norm * 2.0 - 1.0);
  }
  vec3 view_dir = normalize(u_view_pos - pass_Pos);
  vec3 result;
  if (isDirectionalLightOn > 0) { result += CalcDirectionalLight(normal, view_dir); }
  int i = 0;
  for (i; i < NUM_POINT_LIGHTS; i++) { result += CalcPointLight(pointLight[i], normal, view_dir); }
  for (i = 0; i < NUM_SPOT_LIGHTS; i++) { result += CalcSpotLight(spotLight[i], normal, view_dir); }
  if (hasEmission > 0) {
    vec3 emission = texture(material.Emission, pass_TexUV).rgb;
    result += emission;
  }
  out_Color = vec4(result, 1.0);
})";

  UBERSHADER = new OGLShader(UBERSHADER_VERT_CODE.c_str(), UBERSHADER_FRAG_CODE.c_str());
}

}  // end namespace AA
