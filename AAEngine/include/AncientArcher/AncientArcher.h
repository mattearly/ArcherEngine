#pragma once
// standard
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

// library
#include <glm/glm.hpp>

// internal
#include "Controls/Input.h"

namespace AA {

class Window;
class Camera;
class Skybox;
class imGUI;
class Prop;
class AnimProp;
class Animation;
struct DirectionalLight;
struct PointLight;
struct SpotLight;
class LongSound;
class SoundEffect;
class Speaker;

class AncientArcher {
public:

  /// <summary>
  /// Prints Engine Version if in Debug.
  /// Creates a Default Window and Renderer
  /// Initializes IMGUI
  /// Creates a Default Sound Device.
  /// Initializes Physx.
  /// </summary>
  /// <returns>true if successful, false if already initialized.</returns>
  bool Init();

  /// <summary>
  /// Runs an initialized media instance.
  /// </summary>
  /// <returns>0 if success, -4 if not init, -5 if no window</returns>
  int Run();

  /// <summary>
  /// Sets the window to close on the next frame.
  /// </summary>
  void Shutdown() noexcept;

  /// <summary>
  /// Resets all the engine variables to default and clear all functional queues.
  /// Runs user OnQuit functions first.
  /// </summary>
  void SoftReset() noexcept;

  /// <summary>
  /// Creates a camera for use. Currently stretchs to screen size 
  /// and will likely be forced to screen size no matter what 
  /// you start it as once you resize the window.
  /// Only one camera is supported currently.
  /// Will throw if you try to add more than one camera.
  /// </summary>
  /// <param name="w">viewport width</param>
  /// <param name="h">viewport height</param>
  /// <returns>id to access the camera</returns>
  unsigned int AddCamera(const int w = 0, const int h = 0);

  /// <summary>
  /// Removes a from the system camera by id.
  /// </summary>
  /// <param name="camId">id to the camera you want to remove</param>
  /// <returns>true if removal was successful, otherwise false</returns>
  bool RemoveCamera(int camId);

  /// <summary>
  /// Set the Render Distance on a camera.
  /// </summary>
  /// <param name="camId">id to the camera you want to change</param>
  /// <param name="amt">a positive value to set the render distance to. negatives will be treated as a positive via absolute value.</param>
  void SetCamMaxRenderDistance(int camId, float amt);

  /// <summary>
  /// Change camera to perspective view mode.
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  void SetCamToPerspective(int camId);

  /// <summary>
  /// Change camera to orthographic mode. 
  /// Note that this doesn't work as expect sometimes and needs work.
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  void SetCamToOrtho_testing(int camId);

  /// <summary>
  /// Changes the field of view of a camera.
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="new_fov">positive value field of view to set. Bound between 1 and 360.</param>
  void SetCamFOV(int camId, float new_fov);

  /// <summary>
  /// Sets the camera width and height.
  /// Note: Camera and projection may snap to screen size on screen resize.
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="w">desired width</param>
  /// <param name="h">desired height</param>
  void SetCamDimensions_testing(int camId, int w, int h);

  /// <summary>
  /// Moves the camera to the coordinates
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="new_loc">desired location</param>
  void SetCamPosition(int camId, glm::vec3 new_loc);

  /// <summary>
  /// Set the pitch of a camera
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="new_pitch_degrees">pitch in degrees</param>
  void SetCamPitch(int camId, float new_pitch_degrees);

  /// <summary>
  /// Set the yaw of a camera
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="new_yaw_degrees">yaw in degrees</param>
  void SetCamYaw(int camId, float new_yaw_degrees);

  /// <summary>
  /// Moves the camera to an offset of its current location
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="offset">offset to apply</param>
  void ShiftCamPosition(int camId, glm::vec3 offset);

  /// <summary>
  /// Rotates the Camera by an offset.
  /// </summary>
  /// <param name="camId">id of the camera to effect</param>
  /// <param name="pitch_offset"></param>
  /// <param name="yaw_offset"></param>
  void ShiftCamPitchAndYaw(int camId, double pitch_offset, double yaw_offset);

  /// <summary>
  /// Gets a copy of the front vec3
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>front vec3 copy</returns>
  glm::vec3 GetCamFront(int camId);

  /// <summary>
  /// Gets a copy of the right vec3
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>right vec3 copy</returns>
  glm::vec3 GetCamRight(int camId);

  /// <summary>
  /// Gets a copy of the cam position vec3
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>location vec3 copy</returns>
  glm::vec3 GetCamPosition(int camId);

  /// <summary>
  /// Get a copy of the cam pitch.
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>camera pitch</returns>
  float GetCamPitch(int camId);

  /// <summary>
  /// Get a copy of the camera yaw
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>camera yaw</returns>
  float GetCamYaw(int camId);

  /// <summary>
  /// Get a vec2 of the pitch and yaw.
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>vec2 pitch,yaw</returns>
  glm::vec2 GetPitchAndYaw(int camId);

  /// <summary>
  /// Get the projection matrix of a camera
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>mat4 copy of the projection</returns>
  glm::mat4 GetProjectionMatrix(int camId);

  /// <summary>
  /// Get the view matrix of a camera
  /// </summary>
  /// <param name="camId">id of the camera to get from</param>
  /// <returns>mat4 copy of the view</returns>
  glm::mat4 GetViewMatrix(int camId);

  /// <summary>
  /// Adds a Prop.
  /// </summary>
  /// <param name="path">literal path to the resource</param>
  /// <param name="location">optional: starting location, default = 0,0,0</param>
  /// <param name="scale">optional: starting size, default = 1,1,1</param>
  /// <returns>id of the prop</returns>
  unsigned int AddProp(const char* path, glm::vec3 location = glm::vec3(0), glm::vec3 scale = glm::vec3(1));

  /// <summary>
  /// Moves a prop to a location
  /// </summary>
  /// <param name="id">id of the prop to effect</param>
  /// <param name="loc">desired location</param>
  void MoveProp(const unsigned int id, glm::vec3 loc);

  /// <summary>
  /// Scales a prop
  /// </summary>
  /// <param name="id">id of the prop to effect</param>
  /// <param name="scale">desired scale</param>
  void ScaleProp(const unsigned int id, glm::vec3 scale);

  /// <summary>
  /// Rotates a prop
  /// </summary>
  /// <param name="id">id of the prop to effect</param>
  /// <param name="rot">rotation axis values, x, y, z should be radians -PI to PI</param>
  void RotateProp(const unsigned int id, glm::vec3 rot);

  /// <summary>
  /// Adds a Animated Prop.
  /// Assumes Bones and Animations are included in the file.
  /// </summary>
  /// <param name="path">literal path</param>
  /// <param name="location">optional: starting location, default = 0,0,0</param>
  /// <returns>id of the animated prop</returns>
  unsigned int AddAnimProp(const char* path, glm::vec3 starting_location = glm::vec3(0));

  /// <summary>
  /// Moves a animated prop to a location
  /// </summary>
  /// <param name="id">id of the animated prop to effect</param>
  /// <param name="loc">desired location</param>  
  void MoveAnimProp(const unsigned int id, glm::vec3 loc);

  /// <summary>
  /// Scales a animated prop
  /// </summary>
  /// <param name="id">id of the animated prop to effect</param>
  /// <param name="scale">desired scale</param>
  void ScaleAnimProp(const unsigned int id, glm::vec3 scale);

  /// <summary>
  /// Rotates a animated prop
  /// </summary>
  /// <param name="id">id of the animated prop to effect</param>
  /// <param name="rot">rotation axis values, x, y, z should be radians -PI to PI</param>
  void RotateAnimProp(const unsigned int id, glm::vec3 rot);

  /// <summary>
  /// Returns the bone count.
  /// </summary>
  /// <param name="id"></param>
  /// <returns></returns>
  unsigned int GetAnimPropBoneCount_testing(const unsigned int anim_prop_id);

  /// <summary>
  /// 
  /// </summary>
  /// <param name="path"></param>
  /// <param name="anim_prop_id"></param>
  /// <returns></returns>
  unsigned int AddAnimation(const char* path, const unsigned int anim_prop_id);


  bool RemoveAnimation(const unsigned int animation_id);

  /// <summary>
  /// set animation to an prop with bones (animprop)
  /// </summary>
  /// <param name="animation_id">id of the animation or -1 to turn off animation</param>
  /// <param name="animprop_id">id of the prop</param>
  void SetAnimationOnAnimProp(const unsigned int animation_id, const unsigned int animprop_id);


  enum class COLLIDERTYPE { BOX, SPHERE, CAPSULE };

  /// <summary>
  /// Adds physic to a prop. In testing. Bug: flips transforms
  /// Needs resize and and other functionality.
  /// </summary>
  /// <param name="prop_id">id of the prop to effect</param>
  /// <param name="type">what kind of colliderbox</param>
  void AddPropPhysics(const int prop_id, const COLLIDERTYPE type = COLLIDERTYPE::BOX);

  /// <summary>
  /// Adds a invisible ground plane. In testing.
  /// </summary>
  /// <param name="norm">facing direction</param>
  /// <param name="distance">distance (from norm direction?)</param>
  void AddGroundPlane(const glm::vec3 norm, float distance);

  /// <summary>
  /// Sets the stats of the physics update every frame.
  /// </summary>
  /// <param name="status">true to turn on, false to turn off</param>
  void SimulateWorldPhysics(bool status);

  /// <summary>
  /// Sets a skybox with 6 textures
  /// </summary>
  /// <param name="incomingSkymapFiles">6 textures</param>
  void SetSkybox(std::vector<std::string> incomingSkymapFiles) noexcept;


  /// <summary>
  /// Removes current skybox. You will see the clear screen color instead.
  /// Call SetSkybox with new parameters to set up the skybox again.
  /// </summary>
  void RemoveSkybox() noexcept;
  
  /// <summary>
  /// Sets the directional light on the default lit shader.
  /// Only one directional light is currently supported.
  /// See https://learnopengl.com/Lighting/Light-casters for more details on these input value.
  ///   or see the Default Lit Shader for the calculation formulas.
  /// Call this function again to change values.
  /// </summary>
  /// <param name="dir">Direction, works like a vector pointing from 0,0,0</param>
  /// <param name="amb">Ambient</param>
  /// <param name="diff">Diffuse</param>
  /// <param name="spec">Specular</param>
  void SetDirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);

  /// <summary>
  /// Removes the directional light.
  /// </summary>
  void RemoveDirectionalLight();

  /// <summary>
  /// Adds a point light to the default lit shader.
  /// Manages its own unique ids so you can add/remove at will (limits on shader).
  /// See https://learnopengl.com/Lighting/Light-casters for more details on these input value.
  ///   or see the Default Lit Shader for the calculation formulas.
  /// </summary>
  /// <param name="pos">Position or Location</param>
  /// <param name="constant">Constant calculation</param>
  /// <param name="linear">Linear Calculation</param>
  /// <param name="quad">Quadratic Calculation</param>
  /// <param name="amb">Abmient Value</param>
  /// <param name="diff">Diffuse Value</param>
  /// <param name="spec">Specular Value</param>
  /// <returns>id to access this point light</returns>
  int AddPointLight(glm::vec3 pos, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);

  /// <summary>
  /// Removes point light.
  /// </summary>
  /// <param name="which_by_id">id of the point light to remove</param>
  /// <returns>true if removal was successful</returns>
  bool RemovePointLight(int which_by_id);

  /// <summary>
  /// Moves a point light.
  /// </summary>
  /// <param name="which">id of the point light to move</param>
  /// <param name="new_pos">position to move to</param>
  void MovePointLight(int which, glm::vec3 new_pos);

  /// <summary>
  /// Changes any of the values of a point light.
  /// </summary>
  /// <param name="which">id of the point light to change values of</param>
  /// <param name="new_pos">new values</param>
  /// <param name="new_constant"></param>
  /// <param name="new_linear"></param>
  /// <param name="new_quad"></param>
  /// <param name="new_amb"></param>
  /// <param name="new_diff"></param>
  /// <param name="new_spec"></param>
  void ChangePointLight(int which, glm::vec3 new_pos, float new_constant, float new_linear, float new_quad,
    glm::vec3 new_amb, glm::vec3 new_diff, glm::vec3 new_spec);


  /// <summary>
  /// Adds a Spot light (flashlight style) to the default lit shader.
  /// Manages its own unique ids so you can add/remove at will (limited by shader).
  /// See https://learnopengl.com/Lighting/Light-casters for more details on these input value.
  ///   or see the Default Lit Shader for the calculation formulas.
  /// </summary>
  /// <param name="pos">Position or Location</param>
  /// <param name="dir">Direction, starting from the pos</param>
  /// <param name="inner">Inner value</param>
  /// <param name="outer">Outer Value</param>
  /// <param name="constant">Constant Value</param>
  /// <param name="linear">Linear Value</param>
  /// <param name="quad">Quadratic Value</param>
  /// <param name="amb">Ambient Value</param>
  /// <param name="diff">Diffuse Value</param>
  /// <param name="spec">Specular Value</param>
  /// <returns>id to access a spot light</returns>
  int AddSpotLight(glm::vec3 pos, glm::vec3 dir, float inner, float outer, float constant,
    float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);

  /// <summary>
  /// Removes a spot light.
  /// </summary>
  /// <param name="which_by_id">id of the spot light to remove</param>
  /// <returns>true if success, false otherwise</returns>
  bool RemoveSpotLight(int which_by_id);

  /// <summary>
  /// Changes a Spot Light's Position and Direction.
  /// </summary>
  /// <param name="which">id of the spot light to move</param>
  /// <param name="new_pos">new position</param>
  /// <param name="new_dir">new direction</param>
  void MoveSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir);

  /// <summary>
  /// Changes any perameter of a Spot light.
  /// </summary>
  /// <param name="which">id of the spot light to change</param>
  /// <param name="new_pos">New Values</param>
  /// <param name="new_dir"></param>
  /// <param name="new_inner"></param>
  /// <param name="new_outer"></param>
  /// <param name="new_constant"></param>
  /// <param name="new_linear"></param>
  /// <param name="new_quad"></param>
  /// <param name="new_amb"></param>
  /// <param name="new_diff"></param>
  /// <param name="new_spec"></param>
  void ChangeSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir, float new_inner,
    float new_outer, float new_constant, float new_linear, float new_quad, glm::vec3 new_amb,
    glm::vec3 new_diff, glm::vec3 new_spec);


  /// <summary>
  /// Adds a sound effect to the playable bank. 
  /// Each effect add instantiates a speaker.
  /// It then sets the sound effect just loaded to that speaker.
  /// </summary>
  /// <param name="path">path to the sound</param>
  /// <returns>id to access the sound (speaker)</returns>
  unsigned int AddSoundEffect(const char* path);

  /// <summary>
  /// Sets the volume of a sound effect.
  /// </summary>
  /// <param name="sound_id">id of the sound to effect</param>
  /// <param name="new_vol">new volume: 1.0 = max</param>
  void SetSoundEffectVolume(int sound_id, float new_vol);

  /// <summary>
  /// Removes sound effect.
  /// </summary>
  /// <param name="sound_id">id of sound effect to remove</param>
  void RemoveSoundEffect(int sound_id);

  /// <summary>
  /// Plays the currently set Sound effect.
  /// </summary>
  /// <param name="id">id of the sound effect (speaker) to play</param>
  /// <param name="interrupt">if true, will stop the previous sound and start the playback over, otherwise won't play until the previous playthrough of the sound effect is done.</param>
  void PlaySoundEffect(int id, bool interrupt = true);

  /// <summary>
  /// Adds a music track.
  /// Only one is allowed at a time.
  /// To change music, you must first remove the old track.
  /// </summary>
  /// <param name="path">literal path to a music file</param>
  void AddMusic(const char* path);

  /// <summary>
  /// Removes the current track.
  /// </summary>
  void RemoveMusic();

  /// <summary>
  /// Plays the currently loaded music.
  /// </summary>
  void PlayMusic();

  /// <summary>
  /// Pauses the currently loaded music.
  /// </summary>
  void PauseMusic();

  /// <summary>
  /// Resumes from pause.
  /// </summary>
  void ResumeMusic();

  /// <summary>
  /// Stops the track. Will start over with a new PlayMusic() call.
  /// </summary>
  void StopMusic();

  /// <summary>
  /// Changes the volume (gain).
  /// </summary>
  /// <param name="new_vol">new gain settings. negative values are treated as 0.f</param>
  void SetMusicVolume(float new_vol);

  /// <summary>
  /// Hides the mouse cursor. 
  /// Used for when you are drawing your own cursor.
  /// </summary>
  void SetCursorToHidden() noexcept;

  /// <summary>
  /// Disables the cursor. See disabled in glfw documentation. https://www.glfw.org/docs/3.3/input_guide.html
  /// </summary>
  void SetCursorToDisabled() noexcept;

  /// <summary>
  /// Sets the Cursor the the standard OS Cursor.
  /// </summary>
  void SetCursorToNormal() noexcept;

  /// <summary>
  /// Sets the default window clear color on the renderer.
  /// </summary>
  /// <param name="color">RGB color from 0 to 1</param>
  void SetWindowClearColor(glm::vec3 color = glm::vec3(.35f, .15f, .35f)) noexcept;

  /// <summary>
  /// Gets the current window Width
  /// </summary>
  /// <returns>copy of window width</returns>
  int GetWindowWidth() noexcept;

  /// <summary>
  /// Gets the current window Height.
  /// </summary>
  /// <returns>copy of the window Height</returns>
  int GetWindowHeight() noexcept;

  /// <summary>
  /// Sets the title of the window.
  /// </summary>
  /// <param name="name">desired window name</param>
  void SetWindowTitle(const char* name) noexcept;

  /// <summary>
  /// Toggles fullscreen.
  /// </summary>
  /// <param name="try_borderless">attempts borderless fullscreen if true</param>
  void ToggleFullscreen(bool try_borderless = false) noexcept;

  /// <summary>
  /// add a function that runs once at start.
  /// </summary>
  /// <param name="function">a function</param>
  /// <returns>unique access id</returns>
  unsigned int AddToOnBegin(void(*function)());

  /// <summary>
  /// Adds a function to run every update.
  /// </summary>
  /// <param name="function">a function that takes float delta time</param>
  /// <returns>unique access id</returns>
  unsigned int AddToUpdate(void(*function)(float));

  /// <summary>
  /// Adds a function that has the IMGUI updates.
  /// </summary>
  /// <param name="function">a function</param>
  /// <returns>unique access id</returns>
  unsigned int AddToImGuiUpdate(void(*function)());

  /// <summary>
  /// Adds a function to handle mouse wheel scroll.
  /// </summary>
  /// <param name="function">a function that takes the MouseScrollWheel scruct</param>
  /// <returns>unique access id</returns>
  unsigned int AddToScrollHandling(void(*function)(MouseScrollWheel&));

  /// <summary>
  /// Add a function to handle keyboard buttons.
  /// </summary>
  /// <param name="function">a function that takes theKeyboardButtons struct</param>
  /// <returns>unique access id</returns>
  unsigned int AddToKeyHandling(void(*function)(KeyboardButtons&));

  /// <summary>
  /// Adds a function to handle the mouse cursor position.
  /// </summary>
  /// <param name="function">a function that takes the MouseCursorPos struct</param>
  /// <returns>unique access id</returns>
  unsigned int AddToMouseHandling(void(*function)(MouseCursorPos&));

  /// <summary>
  /// Adds a function to handle the mouse buttons.
  /// </summary>
  /// <param name="function">a function that takes the MouseButtons struct</param>
  /// <returns>unique access id</returns>
  unsigned int AddToMouseButtonHandling(void(*function)(MouseButtons&));

  /// <summary>
  /// Adds a function that runs once on quit.
  /// </summary>
  /// <param name="function"></param>
  /// <returns>unique access id</returns>
  unsigned int AddToOnTeardown(void(*function)());

  /// <summary>
  /// Removes by id
  /// </summary>
  /// <param name="r_id">id of the function</param>
  /// <returns>true if successful, false otherwise</returns>
  bool RemoveFromOnBegin(unsigned int r_id);

  bool RemoveFromUpdate(unsigned int r_id);

  bool RemoveFromImGuiUpdate(unsigned int r_id);

  bool RemoveFromScrollHandling(unsigned int r_id);

  bool RemoveFromKeyHandling(unsigned int r_id);

  bool RemoveFromMouseHandling(unsigned int r_id);

  bool RemoveFromMouseButtonHandling(unsigned int r_id);

  bool RemoveFromTeardown(unsigned int r_id);

private:

  bool isInit;

  Window* mWindow;

  bool mSimulateWorldPhysics;

  void SetIMGUI(const bool value);
  imGUI* mIMGUI;

  std::vector<std::shared_ptr<Camera> > mCameras;

  std::vector<std::shared_ptr<Prop> > mProps;

  std::vector<std::shared_ptr<AnimProp> > mAnimProps;

  std::vector<std::shared_ptr<Animation> > mAnimation;

  std::shared_ptr<Skybox> mSkybox;

  std::shared_ptr<DirectionalLight> mDirectionalLight;

  int mNumPointLightsInUse;
  const int MAXPOINTLIGHTS = 24;  // needs to match on shader
  std::vector<std::shared_ptr<PointLight> > mPointLights;

  int mNumSpotLightsInUse;
  const int MAXSPOTLIGHTS = 12;   // needs to match on shader
  std::vector<std::shared_ptr<SpotLight> > mSpotLights;

  LongSound* mMusic;
  std::vector<std::shared_ptr<SoundEffect> > mSoundEffects;
  std::vector<std::shared_ptr<Speaker> > mSpeakers;

  std::unordered_map<unsigned int, std::function<void()> > onStart;
  std::unordered_map<unsigned int, std::function<void(float)> > onUpdate;
  std::unordered_map<unsigned int, std::function<void()> > onImGuiUpdate;
  std::unordered_map<unsigned int, std::function<void(MouseScrollWheel&)> > onScrollHandling;
  std::unordered_map<unsigned int, std::function<void(KeyboardButtons&)> > onKeyHandling;
  std::unordered_map<unsigned int, std::function<void(MouseCursorPos&)> > onMouseHandling;
  std::unordered_map<unsigned int, std::function<void(MouseButtons&)> > onMouseButtonHandling;
  std::unordered_map<unsigned int, std::function<void()> > onQuit;

  void begin();
  void update();
  void render();
  void teardown();

};

}  // end namespace AA
