#include "DefaultShaders.h"
namespace AA {
OGLShader* diffuse_3d;
OGLShader* phong_3d;
OGLShader* skel_3d;
OGLShader* DefaultShaders::get_diffuse_3d() {
  diffuse_3d->Use();
  return diffuse_3d;
}
OGLShader* DefaultShaders::get_phong_3d() {
  phong_3d->Use();
  return phong_3d;
}
OGLShader* DefaultShaders::get_skel_3d() {
  skel_3d->Use();
  return skel_3d;
}
void DefaultShaders::init_diffuse_3d() {
  const std::string vert_3D_diff = R"(
#version 430 core
layout(location=0)in vec3 inPos;
layout(location=1)in vec2 inTexUV;
//out vec3 pass_Pos;
out vec2 pass_TexUV;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
void main()
{
  vec3 pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
  pass_TexUV = inTexUV;
  vec4 final_pos = u_projection_matrix * u_view_matrix * vec4(pos, 1);
  gl_Position = final_pos;
}
)";
  const std::string frag_diff = R"(
#version 430 core
in vec2 pass_TexUV;
out vec4 out_Color;
struct Material {
  sampler2D Albedo;
};
uniform int hasAlbedo;
uniform Material material;
void main() {
  if (hasAlbedo > 0) {
    out_Color = texture(material.Albedo, pass_TexUV);
  } else {
    out_Color = vec4(0.75, 0.0, 0.0, 1.0); // red for no texture
  }
}
)";
  if (!diffuse_3d) {
    diffuse_3d = new OGLShader(vert_3D_diff.c_str(), frag_diff.c_str());
  }
}
void DefaultShaders::init_phong_3d() {

  const std::string vert_3D_lit = R"(
#version 430 core
layout(location=0)in vec3 inPos;
layout(location=1)in vec2 inTexUV;
layout(location=2)in vec3 inNorm;
//layout(location=3)in vec3 inTangent;
//layout(location=4)in vec3 inBitangent;
layout(location=0)out vec3 pass_Pos;
layout(location=1)out vec2 pass_TexUV;
layout(location=2)out vec3 pass_Norm;
//out vec3 pass_Tangent;
//out vec3 pass_Bitangent;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
void main()
{
  pass_Pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
  pass_TexUV = inTexUV;
  mat3 normal_matrix = transpose(inverse(mat3(u_model_matrix)));
  pass_Norm = normalize(normal_matrix * inNorm);
  //pass_Tangent = normalize(normal_matrix * inTangent);
  //pass_Bitangent = normalize(normal_matrix * inBitangent);
  gl_Position = u_projection_matrix * u_view_matrix * vec4(pass_Pos, 1);
}
)";
  const std::string frag_lit = R"(
#version 430 core
layout(location=0)in vec3 pass_Pos;
layout(location=1)in vec2 pass_TexUV;
layout(location=2)in vec3 pass_Norm;
//in vec3 pass_Tangent;
//in vec3 pass_Bitangent;
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
const vec3 default_color = vec3(0.9);
const int MAXPOINTLIGHTS = 24; // if changed, needs to match on core
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
  if (hasSpecular > 0) {
    vec3 specular = light.Specular * spec * texture(material.Specular, pass_TexUV).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
  } else {
    ambient *= attenuation;
    diffuse *= attenuation;
    return (ambient + diffuse);
  }
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
  if (isDirectionalLightOn > 0)
    result += CalcDirectionalLight(normal, view_dir);
  int i = 0;
  for (i; i < NUM_POINT_LIGHTS; i++)
    result += CalcPointLight(pointLight[i], normal, view_dir);
  for (i = 0; i < NUM_SPOT_LIGHTS; i++)
    result += CalcSpotLight(spotLight[i], normal, view_dir);
  if (hasEmission != 0) {
    vec3 emission = texture(material.Emission, pass_TexUV).rgb;
    result += emission;
  }
  out_Color = vec4(result, 1.0);
}
)";
  if (!phong_3d) {
    phong_3d = new OGLShader(vert_3D_lit.c_str(), frag_lit.c_str());
  }
}
void DefaultShaders::init_skel_3d() {

  const std::string skel_anim_frag = R"(
#version 430 core
layout(location=1)in vec2 pass_TexUV;
out vec4 out_Color;
struct Material {
  sampler2D Albedo;
};
uniform int hasAlbedo;
uniform Material material;
void main() {
  if (hasAlbedo > 0) {
    out_Color = texture(material.Albedo, pass_TexUV);
  } else {
    out_Color = vec4(0.75, 0.0, 0.0, 1.0); // red for no texture
  }
}
)";
  const std::string skel_anim_vert = R"(
#version 430 core
layout(location=0)in vec3 inPos;
layout(location=1)in vec2 inTexUV;
//skip normal
layout(location=3)in ivec4 inBoneIds;
layout(location=4)in vec4 inWeights;
layout(location=1)out vec2 pass_TexUV;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform int isAnimating;
void main()
{
    pass_TexUV = inTexUV;
    vec4 totalPosition = vec4(0.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(inBoneIds[i] == -1) 
            continue;
        if(inBoneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(inPos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[inBoneIds[i]] * vec4(inPos,1.0);
        totalPosition += localPosition * inWeights[i];
        //vec3 localNormal = mat3(finalBonesMatrices[inBoneIds[i]]) * norm;
    }
		
    mat4 viewModel = u_view_matrix * u_model_matrix;

    if (isAnimating > 0) {
      gl_Position = u_projection_matrix * viewModel * totalPosition;
    } else {
      gl_Position = u_projection_matrix * viewModel * vec4(inPos,1.0);
    }
}
)";
  if (!skel_3d) {
    skel_3d = new OGLShader(skel_anim_vert.c_str(), skel_anim_frag.c_str());
  }
}


// old helper functions we probably don't need anymore
//// helper function to update a shader projection from a specific camera
//void updateProjectionFromCam(OGLShader* shader_to_update, const Camera& from_cam) {
//  if (!shader_to_update)
//    return;
//  shader_to_update->Use();
//  shader_to_update->SetMat4("u_projection_matrix", from_cam.mProjectionMatrix);
//}
//
//// helper function to update a shader view from a specific camera
//void updateViewFromCam(OGLShader* shader_to_update, const Camera& from_cam) {
//  if (!shader_to_update)
//    return;
//  shader_to_update->Use();
//  shader_to_update->SetMat4("u_view_matrix", from_cam.mViewMatrix);
//}

}  // end namespace AA
