//#pragma once
//#include "../Renderer/OpenGL/OGLShader.h"
//#include <vector>
//namespace AA {
//
//using glm::vec2;
//using glm::mat4;
//using glm::vec3;
//
//struct Button {
//  Button(vec2 pos, float width_scale, float height_scale, vec3 color, float alpha);
//  Button(vec2 pos, float width_scale, float height_scale, float alpha, const char* texture_path);
//  vec2 pos;
//  float width;
//  float height;
//  mat4 transformation;
//  vec3 color;
//  float alpha;
//  int texture_id = 0;
//  bool overlapped = false;
//};
//
////struct Slider {
////  vec2 bot_left_loc;
////  float low_val;
////  float high_val;
////  float bar_thickness;
////  float bar_length;
////  bool horizontal;
////  mat4 local_transform;
////};
//
//class PlainGUI {
//public:
//  PlainGUI();
//  ~PlainGUI();
//
//  void AddButton(vec2 pos, float width_scale, float height_scale, vec3 color, float alpha);
//  void AddButton(vec2 pos, float width_scale, float height_scale, float alpha, const char* texture_path);
//
//  //void AddSlider(Slider slider, void(*resulthandler)(float result));
//
//  void ShowInterface();
//
//  void HideInterface();
//
//  void InitShader();
//
//  void UpdateMouseLoc(vec2 mouse_loc);
//
//
//  void Draw();
//
//private:
//
//  OGLShader* Shader;
//
//  void UpdateMouseOverlap(vec2 mouse_loc);
//
//  void UpdateUniforms(vec2 resolution, vec2 mouse_loc, float elapsed_time);
//
//  //std::vector<Slider> p_Sliders;
//
//  std::vector<Button> p_Buttons;
//
//  unsigned int vao_id;
//
//  int ind_size;
//
//  float total_time = 0;
//
//  bool p_HideAll = false;
//};
//
//}