#include "../../../include/AAEngine/OS/Interface/Window.h"
#include "../../../include/AAEngine/Version.h"
#include "../../../include/AAEngine/WindowOptions.h"
#include "../OpenGL/OGLGraphics.h"
namespace AA {

static unsigned int window_instance_count = 0;
static bool glfw_is_init = false;   // only init glfw once, even if multiple windows

extern void GLFWERRORCALLBACK(int e, const char* msg);
extern void FRAMEBUFFERSIZESETCALLBACK(GLFWwindow* window, int w, int h);
extern void NORMALMOUSEREPORTINGCALLBACK(GLFWwindow* window, double xpos, double ypos);
extern void KEYCALLBACK(GLFWwindow* w, int key, int scancode, int action, int mods);
extern void MOUSEBUTTONCALLBACK(GLFWwindow* w, int button, int action, int mods);
extern void MOUSESCROLLWHEELCALLBACK(GLFWwindow* w, double xoffset, double yoffset);
extern void ONWINDOWFOCUSCALLBACK(GLFWwindow* window, int focused);


/// <summary>
/// default init
/// </summary>
Window::Window() {
  if (!glfw_is_init) {
    glfwInit();
    glfw_is_init = true;
  }

  // start with default options
  WindowOptions Options;
  SetDefaults(Options);
  mWindowOptions = std::make_shared<WindowOptions>(Options);
  default_init();
  window_instance_count++;
}

/// <summary>
/// Use custom init options
/// </summary>
/// <param name="winopts">copies from current values of this</param>
Window::Window(WindowOptions winopts) {
  if (!glfw_is_init) {
    glfwInit();
    glfw_is_init = true;
  }
  mWindowOptions = std::make_shared<WindowOptions>(winopts);
  default_init();
  window_instance_count++;
}

/// <summary>
/// Use custom init options.
/// </summary>
/// <param name="winopts">shared ptr to winopts so sync with</param>
Window::Window(std::shared_ptr<WindowOptions> winopts) {
  if (!glfw_is_init) {
    glfwInit();
    glfw_is_init = true;
  }
  mWindowOptions = winopts;
  default_init();
  window_instance_count++;
}

// shares context with a previous window
//Window::Window(const Window& window) {
//  if (!glfw_is_init) {
//    glfwInit();
//    glfw_is_init = true;
//  }
//  mWindowOptions = std::make_shared<WindowOptions>();      // start with default options
//  default_init(window);
//  window_instance_count++;
//}

Window::~Window() {
  if (mGLFWwindow)
    glfwDestroyWindow(mGLFWwindow);
  window_instance_count--;
  if (window_instance_count == 0) {
    glfwTerminate();
    glfw_is_init = false;
  }
}

// note: saved prev_window_options on each call, must be carefully managed 
//       or ApplyChanges might not work as expected
std::weak_ptr<WindowOptions> Window::get_and_note_window_options() {
  mPrevWindowOptions = *mWindowOptions;  // copy to prev window options
  return mWindowOptions;
}

void Window::apply_new_window_option_changes() {
  //todo: relaunch window if required (msaa change, render tech is prelaunch only)
  if (mWindowOptions->_msaa_samples > 0 && mPrevWindowOptions._msaa_samples != mWindowOptions->_msaa_samples) {
    OGLGraphics::SetMultiSampling(true);
    // locking this at a max of 16
    if (mWindowOptions->_msaa_samples >= 1 && mWindowOptions->_msaa_samples <= 16) {
      glfwWindowHint(GLFW_SAMPLES, mWindowOptions->_msaa_samples);
    }
    else {
      glfwWindowHint(GLFW_SAMPLES, 16);
      mWindowOptions->_msaa_samples = 16;
    }
    // todo: relaunch window
  }

  if (mPrevWindowOptions._stencil_bits != mWindowOptions->_stencil_bits) {
    glfwWindowHint(GLFW_STENCIL_BITS, mWindowOptions->_stencil_bits);
    if (mWindowOptions->_stencil_bits > 0) {
      OGLGraphics::SetStencil(true);
    }
    else {
      OGLGraphics::SetStencil(false);
    }
  }

  // update things that have changed
  if (mWindowOptions->_cursor_mode != mPrevWindowOptions._cursor_mode) {
    glfwSetInputMode(mGLFWwindow, GLFW_CURSOR, static_cast<int>(mWindowOptions->_cursor_mode));
  }

  if (mWindowOptions->_title != mPrevWindowOptions._title) {
    glfwSetWindowTitle(mGLFWwindow, mWindowOptions->_title.c_str());
  }

  if (mWindowOptions->_min_width != mPrevWindowOptions._min_width || mWindowOptions->_min_height != mPrevWindowOptions._min_height) {
    // enforce engine limits
    if (mWindowOptions->_min_width < WindowOptions::_MIN_WIDTH)
      mWindowOptions->_min_width = WindowOptions::_MIN_WIDTH;
    if (mWindowOptions->_min_height < WindowOptions::_MIN_HEIGHT)
      mWindowOptions->_min_height = WindowOptions::_MIN_HEIGHT;
    // set window mins to new min
    glfwSetWindowSizeLimits(mGLFWwindow, mWindowOptions->_min_width, mWindowOptions->_min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);
  }

  if (mWindowOptions->_width != mPrevWindowOptions._width || mWindowOptions->_height != mPrevWindowOptions._height) {
    // enforce engine limits
    if (mWindowOptions->_width < mWindowOptions->_min_width)
      mWindowOptions->_width = mWindowOptions->_min_width;
    if (mWindowOptions->_height < mWindowOptions->_min_height)
      mWindowOptions->_height = mWindowOptions->_min_height;

    glfwSetWindowSize(mGLFWwindow, mWindowOptions->_width, mWindowOptions->_height);
  }

  // set window if mode changed
  if (mPrevWindowOptions._windowing_mode != mWindowOptions->_windowing_mode) {
    apply_window_sizings_from_current_options();
  }
  else if (mWindowOptions->_windowing_mode == WINDOW_MODE::WINDOWED_DEFAULT) {
    apply_window_sizings_from_current_options();
  }

  if (mPrevWindowOptions._vsync != mWindowOptions->_vsync) {
    glfwSwapInterval(mWindowOptions->_vsync);
  }

  // note (copy) applied settings
  mPrevWindowOptions = *mWindowOptions;
}

void Window::Close() {
  glfwSetWindowShouldClose(mGLFWwindow, 1);
}
void Window::SetCursorToHidden() noexcept {
  get_and_note_window_options();
  mWindowOptions->_cursor_mode = CURSOR_MODE::HIDDEN;
  apply_new_window_option_changes();
}
void Window::SetCursorToDisabled() noexcept {
  get_and_note_window_options();
  mWindowOptions->_cursor_mode = CURSOR_MODE::DISABLED;
  apply_new_window_option_changes();
}
void Window::SetCursorToNormal() noexcept {
  get_and_note_window_options();
  mWindowOptions->_cursor_mode = CURSOR_MODE::NORMAL;
  apply_new_window_option_changes();
}
void Window::SetNewWidthAndHeight(int w, int h) noexcept
{
  auto weak_opts = get_and_note_window_options();
  auto opts = weak_opts.lock();
  opts->_width = w;
  opts->_height = h;
  apply_new_window_option_changes();
}

void Window::SetNewMinWidthAndHeight(int w, int h) noexcept
{
  auto weak_opts = get_and_note_window_options();
  auto opts = weak_opts.lock();
  opts->_min_width = w;
  opts->_min_height = h;
  apply_new_window_option_changes();
}

bool Window::GetShouldClose() {
  return glfwWindowShouldClose(mGLFWwindow);
}
int Window::GetCurrentWidth() {
  int width, height;
  glfwGetWindowSize(mGLFWwindow, &width, &height);
  return width;
}

int Window::GetCurrentHeight() {
  int width, height;
  glfwGetWindowSize(mGLFWwindow, &width, &height);
  return height;
}

int Window::GetCurrentMinWidth()
{
    return mWindowOptions->_min_width;
}

int Window::GetCurrentMinHeight()
{
  return mWindowOptions->_min_height;
}


/// <summary>
/// This will attempt to apply the window sizing and lock in the updates.
/// The end user mainly will handle this on their own, except for toggling fullscreen
/// and the way the window is initialized if passed custom options
/// </summary>
void Window::apply_window_sizings_from_current_options() noexcept {
  const auto monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  switch (mWindowOptions->_windowing_mode) {
  case WINDOW_MODE::MAXIMIZED:
    glfwMaximizeWindow(mGLFWwindow);
    break;

  case WINDOW_MODE::WINDOWED:
    glfwSetWindowMonitor(mGLFWwindow, nullptr,
      /*center width*/  (int)(mode->width / 2.f - mWindowOptions->_width / 2.f),
      /*center height*/ (int)(mode->height / 2.f - mWindowOptions->_height / 2.f),
      mWindowOptions->_width, mWindowOptions->_height, GLFW_DONT_CARE);
    break;

  case WINDOW_MODE::WINDOWED_DEFAULT:
    mWindowOptions->_width = 800;
    mWindowOptions->_height = 600;
    glfwSetWindowMonitor(mGLFWwindow, nullptr,
      /*center width*/  (int)(mode->width / 2.f - mWindowOptions->_width / 2.f),
      /*center height*/ (int)(mode->height / 2.f - mWindowOptions->_height / 2.f),
      mWindowOptions->_width, mWindowOptions->_height, GLFW_DONT_CARE);
    mWindowOptions->_windowing_mode = WINDOW_MODE::WINDOWED;
    break;


  case WINDOW_MODE::FULLSCREEN_BORDERLESS:

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwSetWindowMonitor(mGLFWwindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    break;

  case WINDOW_MODE::FULLSCREEN:
    glfwRestoreWindow(mGLFWwindow); // in case of maximized
    glfwSetWindowMonitor(mGLFWwindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    mWindowOptions->_width = mode->width;
    mWindowOptions->_height = mode->height;
    break;
  }

  //// lock in with previous after this
  //mPrevWindowOptions._windowing_mode = mWindowOptions->_windowing_mode;
}

void Window::swap_buffers() {
  glfwSwapBuffers(mGLFWwindow);
}

void Window::default_init() {
  if (mWindowOptions->_width < 160)
    mWindowOptions->_width = 160;
  if (mWindowOptions->_height < 144)
    mWindowOptions->_height = 144;





  if (mWindowOptions->_windowing_mode == WINDOW_MODE::MAXIMIZED) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  }
  glfwWindowHint(GLFW_SAMPLES, mWindowOptions->_msaa_samples);
  if (mWindowOptions->_rendering_tech != RENDER_TECH::OPENGL4) {
    throw("unsupported render tech");  // todo (major): add other render techs
  } else {
    // with core profile, you have to create and manage your own VAO's, no default 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_STENCIL_BITS, mWindowOptions->_stencil_bits);
    // Try the latest version of OpenGL we are allowed, don't use older than 4.3
    struct OpenGLVersion {
      OpenGLVersion() {}
      OpenGLVersion(int maj, int min) :major(maj), minor(min) {}
      int major = -1;
      int minor = -1;
    };
    std::vector<OpenGLVersion> try_versions;
    try_versions.reserve(4);
    try_versions.push_back(OpenGLVersion(4, 3));
    try_versions.push_back(OpenGLVersion(4, 4));
    try_versions.push_back(OpenGLVersion(4, 5));
    try_versions.push_back(OpenGLVersion(4, 6));
    while (!mGLFWwindow && !try_versions.empty()) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, try_versions.back().major);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, try_versions.back().minor);
      if (mWindowOptions->_windowing_mode == WINDOW_MODE::FULLSCREEN) {
        const auto monitor = glfwGetPrimaryMonitor();
        const auto video_mode = glfwGetVideoMode(monitor);
        mGLFWwindow = glfwCreateWindow(video_mode->width, video_mode->height, mWindowOptions->_title.c_str(), monitor, nullptr);
      }
      else if (mWindowOptions->_windowing_mode == WINDOW_MODE::FULLSCREEN_BORDERLESS) {
        const auto monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        mGLFWwindow = glfwCreateWindow(mode->width, mode->height, mWindowOptions->_title.c_str(), monitor, nullptr);
      }
      else {
        mGLFWwindow = glfwCreateWindow(mWindowOptions->_width, mWindowOptions->_height, mWindowOptions->_title.c_str(), nullptr, nullptr);
      }
      if (!mGLFWwindow) {
        try_versions.pop_back();
      }
    }

    if (!mGLFWwindow) { throw("Unable to init Window for OpenGL 4.3+"); }

    glfwSetWindowSizeLimits(mGLFWwindow, mWindowOptions->_min_width, mWindowOptions->_min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetInputMode(mGLFWwindow, GLFW_CURSOR, static_cast<int>(mWindowOptions->_cursor_mode));
    glfwSwapInterval((mWindowOptions->_vsync) ? 1 : 0);

    // todo (multithreading): consider making this rendering context on its own thread : src https://discourse.glfw.org/t/question-about-glfwpollevents/1524
    glfwMakeContextCurrent(mGLFWwindow);
    OGLGraphics::Proc(glfwGetProcAddress);

  }

  if (mWindowOptions->_stencil_bits > 0) {
    OGLGraphics::SetStencil(true);
    OGLGraphics::SetStencilFuncToNotEqual();
    OGLGraphics::SetStencilOpDepthPassToReplace();
  }
  else {
    OGLGraphics::SetStencil(false);
  }

  glfwSetWindowUserPointer(mGLFWwindow, this);  // window pointer goes to this class
  set_default_callbacks();

  apply_new_window_option_changes();
}

void Window::set_default_callbacks() {
  glfwSetFramebufferSizeCallback(mGLFWwindow, FRAMEBUFFERSIZESETCALLBACK);
  glfwSetCursorPosCallback(mGLFWwindow, NORMALMOUSEREPORTINGCALLBACK);
  glfwSetKeyCallback(mGLFWwindow, KEYCALLBACK);
  glfwSetMouseButtonCallback(mGLFWwindow, MOUSEBUTTONCALLBACK);
  glfwSetScrollCallback(mGLFWwindow, MOUSESCROLLWHEELCALLBACK);
  glfwSetWindowFocusCallback(mGLFWwindow, ONWINDOWFOCUSCALLBACK);
}

}  // end namespace AA
