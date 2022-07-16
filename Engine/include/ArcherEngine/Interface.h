/*
* This is the main way to interface with AAEngine, and maybe the only include file you need.
*/

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
#include "Scene/Camera.h"
#include "Scene/SunLight.h"
#include "OS/Interface/Window.h"
#include "WindowOptions.h"
#include "Scene/Scene.h"

namespace AA {

class imGUI;
class Animation;
struct PointLight;
struct SpotLight;
class LongSound;
class SoundEffect;
class Speaker;

class Interface final {
public:

  /// <summary>
  /// Init Window and Renderer with Default Options.
  /// Initializes IMGUI.
  /// Creates a Default Sound Device.
  /// Initializes Physx.
  /// </summary>
  /// <returns>True if successful, False if already initialized.</returns>
  bool Init();

  /// <summary>
  /// Init Window and Renderer with user specificed WindowOptions to copy over.
  /// Initializes IMGUI.
  /// Creates a Default Sound Device.
  /// Initializes Physx.
  /// </summary>
  /// <param name="winopts"></param>
  /// <returns>True if successful, False if already initialized.</returns>
  bool Init(WindowOptions winopts);

  /// <summary>
  /// Init Window and Renderer with user specificed WindowOptions to share.
  /// Initializes IMGUI.
  /// Creates a Default Sound Device.
  /// Initializes Physx.
  /// </summary>
  /// <param name="winopts"></param>
  /// <returns>True if successful, False if already initialized.</returns>
  bool Init(std::shared_ptr<WindowOptions> winopts);

  /// <summary>
  /// Runs an initialized media instance.
  /// </summary>
  /// <returns>0 if success, -4 if not init, -5 if no window</returns>
  int Run();

  /// <summary>
  /// Sets the window to close on the next frame. Same as GetWindow()->Close().
  /// </summary>
  void Shutdown() noexcept;

  /// <summary>
  /// Resets all the engine variables to default and clear all functional queues.
  /// Runs user OnQuit functions first.
  /// </summary>
  void SoftReset() noexcept;

  /// <summary>
  /// Creates a camera for use in a scene.
  /// </summary>
  /// <param name="w">viewport width</param>
  /// <param name="h">viewport height</param>
  /// <returns>unique id to access the camera</returns>
  unsigned int AddCamera(const int w = 0, const int h = 0);

  /// <summary>
  /// Creates a camera for use in a scene.
  /// </summary>
  /// <param name="w"></param>
  /// <param name="h"></param>
  /// <param name="stay_window_size"></param>
  /// <returns>unique id to access the camera</returns>
  unsigned int AddCamera(const int& w, const int& h, const bool& stay_window_size);

  /// <summary>
  /// Removes a from the system camera by id.
  /// </summary>
  /// <param name="camId">id to the camera you want to remove</param>
  /// <returns>true if removal was successful, otherwise false</returns>
  bool RemoveCamera(int camId);

  /// <summary>
  /// Access Camera by Id
  /// </summary>
  /// <param name="camId">id that was returned by AddCamera()</param>
  /// <returns></returns>
  std::weak_ptr<Camera> GetCamera(uidtype camId);

  /// <summary>
  /// Adds a Scene.
  /// </summary>
  /// <param name="path">literal path to the resource</param>
  /// <param name="location">optional: starting location, default = 0,0,0</param>
  /// <param name="scale">optional: starting size, default = 1,1,1</param>
  /// <returns>id of the prop</returns>
  unsigned int AddProp(const char* path, const bool& try_animated, const glm::vec3 location = glm::vec3(0), const glm::vec3 scale = glm::vec3(1));

  /// <summary>
  /// Removes a prop for our list of managed ones.
  /// Calls remove cache on model data.
  /// </summary>
  /// <param name="id">id of the prop to remove</param>
  /// <returns>true if successful, false otherwise</returns>
  bool RemoveProp(const unsigned int id);

  /// <summary>
  /// Access to props.
  /// </summary>
  /// <param name="id">id of the prop to access</param>
  /// <returns>a weak ptr to a prop</returns>
  [[nodiscard]] std::weak_ptr<Scene> GetProp(const unsigned int id) const;

  /// <summary>
  /// Adds Skeletal Animation Data to the memory bank of a Animated Scene.
  /// </summary>
  /// <param name="path">path to a file (fbx tested, but should handle any file that assimp can load</param>
  /// <param name="anim_prop_id">The Unique ID of the animated prop to apply this animation to.</param>
  /// <returns>The Unique ID to access this animation.</returns>
  unsigned int AddAnimation(const char* path, const unsigned int prop_id_to_match_bones_with);

  /// <summary>
  /// Remvoes animation from memory bank.
  /// </summary>
  /// <param name="animation_id">id of animation to remove</param>
  /// <returns>True if succeeded on remove, False otherwise.</returns>
  bool RemoveAnimation(const unsigned int animation_id);

  /// <summary>
  /// set animation to an prop with bones (animprop)
  /// </summary>
  /// <param name="animation_id">id of the animation or pass in -1 to turn off animation</param>
  /// <param name="animprop_id">id of the prop</param>
  void SetAnimationOnProp(const unsigned int animation_id, const unsigned int animprop_id);

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
  void SetSunLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);

  void SetSunLight(const SunLight& other);

  std::weak_ptr<SunLight> GetSunLight() noexcept;

  /// <summary>
  /// Removes the directional light.
  /// </summary>
  void RemoveSunLight();

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

  void DebugLightIndicatorsOnOrOff(const bool& tf);

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
  /// Note: Does not work with .mp3 files, as there are some Legal limitations with openAL (https://www.gamedev.net/forums/topic/211901-does-openal-play-mp3/)
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
  /// Sets the default window clear color on the renderer.
  /// </summary>
  /// <param name="color">RGB color from 0 to 1</param>
  void SetWindowClearColor(glm::vec3 color = glm::vec3(.35f, .15f, .35f)) noexcept;

  /// <summary>
  /// Get window to access public functions
  /// </summary>
  /// <returns>A Shared Pointer to the Window Class</returns>
  std::weak_ptr<Window> GetWindow();

  /// <summary>
  /// Sets the title of the window.
  /// </summary>
  /// <param name="name">desired window name</param>
  void SetWindowTitle(const char* name) noexcept;

  /// <summary>
  /// Turns the log stream on for console or file dir (outputs to your runtime dir)
  /// </summary>
  /// <param name="on_or_off">true = turn on, false = turn off</param>
  /// <param name="file_or_stdout">true = file out, false = stdout; ignored if on_or_off is false.</param>
  void SetLogStream(const bool& on_or_off, const bool& file_or_stdout);

  /// <summary>
  /// Toggles fullscreen.
  /// </summary>
  /// <param name="try_borderless">attempts borderless fullscreen if true</param>
  void ToggleWindowFullscreen(bool try_borderless = false) noexcept;

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
  unsigned int AddToOnQuit(void(*function)());


  /// <summary>
  /// Removes by id
  /// </summary>
  /// <param name="r_id">id of the function</param>
  /// <returns>true if successful, false otherwise</returns>
  bool RemoveFromOnBegin(unsigned int r_id);

  bool RemoveFromOnUpdate(unsigned int r_id);

  bool RemoveFromImGuiUpdate(unsigned int r_id);

  bool RemoveFromScrollHandling(unsigned int r_id);

  bool RemoveFromKeyHandling(unsigned int r_id);

  bool RemoveFromMouseHandling(unsigned int r_id);

  bool RemoveFromMouseButtonHandling(unsigned int r_id);

  bool RemoveFromOnQuit(unsigned int r_id);

  /// <summary>
  /// resets all the runtime functions set by:
  /// AddToOnBegin, AddToUpdate, AddToImGuiUpdate, AddToScrollHandling
  /// AddToKeyHandling, AddToMouseHandling, AddToMouseButtonHandling, AddToOnTeardown
  /// </summary>
  void ClearAllRuntimeLamdaFunctions();

private:

  bool isInit;

  std::shared_ptr<Window> mWindow;

  bool mSimulateWorldPhysics;

  void SetIMGUI(const bool value);
  imGUI* mIMGUI;

  std::vector<std::shared_ptr<Camera> > mCameras;

  std::vector<std::shared_ptr<Scene> > mProps;

  std::vector<std::shared_ptr<Animation> > mAnimation;

  std::shared_ptr<SunLight> mSunLight;

  int mNumPointLightsInUse;
  const int MAXPOINTLIGHTS = 24;  // needs to match on shader
  std::vector<std::shared_ptr<PointLight> > mPointLights;

  int mNumSpotLightsInUse;
  const int MAXSPOTLIGHTS = 12;   // needs to match on shader
  std::vector<std::shared_ptr<SpotLight> > mSpotLights;

  bool mDebugLightIndicators = false;

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
  void settle_window_resize_flag();  // pre_render helper
  void pre_render();
  void render();
  void post_render();
  void teardown();

};

}  // end namespace AA
