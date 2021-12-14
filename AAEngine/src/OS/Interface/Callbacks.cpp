#include "Window.h"
#include "../OpenGL/OGLGraphics.h"
#include <memory>
//#include <iostream>

namespace AA {

bool g_new_key_reads = false;
bool g_os_window_resized = true;  // start true so projection updates right away

void g_poll_input_events() {
  glfwPollEvents();
}

// todo: reset the reciever structs back to default on window switch (maybe?)
KeyboardButtons  g_keyboard_input_status;
MouseScrollWheel g_scroll_input_status;
MouseCursorPos   g_mouse_input_status;
MouseButtons     g_mouse_button_status; 

void GLFWERRORCALLBACK(int e, const char* msg) {
  if (e != GLFW_NO_ERROR) {
    throw(msg);
  }
}

void FRAMEBUFFERSIZESETCALLBACK(GLFWwindow* window, int w, int h) {
  // Update Cached Width & Height
  Window* self = (Window*)glfwGetWindowUserPointer(window);
  self->mWindowOptions->_width = w;
  self->mWindowOptions->_height = h;

  // Update Graphics Viewport
  switch (self->mWindowOptions->_rendering_tech) {
  case RENDER_TECH::OPENGL4:
    OGLGraphics::SetViewportSize(0, 0, w, h);
    break;
  case RENDER_TECH::D3D11:
  case RENDER_TECH::VULKAN1:
    throw("Render Tech Not Supported");
    break;
  }
  // Update Projection Matricies
  g_os_window_resized = true;
}

void NORMALMOUSEREPORTINGCALLBACK(GLFWwindow* window, double xpos, double ypos) {
  // Update Cached Position
  g_mouse_input_status.xOffset = xpos;
  g_mouse_input_status.yOffset = ypos;
  //std::cout << "Mouse Pos: " << xpos << " " << ypos << '\n';
}

void ONWINDOWFOCUSCALLBACK(GLFWwindow* window, int focused) {
  //if (focused) {
    //std::cout << "Focus changed to New Window\n";
    //Window* self = (Window*)glfwGetWindowUserPointer(window);
    //glfwMakeContextCurrent(window);
  //} else {
    //std::cout << "Focus left window\n";
  //}
}

void KEYCALLBACK(GLFWwindow* w, int key, int scancode, int action, int mods) {
  // esc
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    g_keyboard_input_status.esc = true;
  } else if (key == GLFW_KEY_ESCAPE == GLFW_RELEASE) {
    g_keyboard_input_status.esc = false;
  }
  // function keys
  if (key == GLFW_KEY_F1 == GLFW_PRESS) {
    g_keyboard_input_status.f1 = true;
  } else if (key == GLFW_KEY_F1 == GLFW_RELEASE) {
    g_keyboard_input_status.f1 = false;
  }
  if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
    g_keyboard_input_status.f2 = true;
  } else if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f2 = false;
  }
  if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
    g_keyboard_input_status.f3 = true;
  } else if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f3 = false;
  }
  if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
    g_keyboard_input_status.f4 = true;
  } else if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f4 = false;
  }
  if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
    g_keyboard_input_status.f5 = true;
  } else if (key == GLFW_KEY_F5 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f5 = false;
  }
  if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
    g_keyboard_input_status.f6 = true;
  } else if (key == GLFW_KEY_F6 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f6 = false;
  }
  if (key == GLFW_KEY_F7 && action == GLFW_PRESS) {
    g_keyboard_input_status.f7 = true;
  } else if (key == GLFW_KEY_F7 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f7 = false;
  }
  if (key == GLFW_KEY_F8 && action == GLFW_PRESS) {
    g_keyboard_input_status.f8 = true;
  } else if (key == GLFW_KEY_F8 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f8 = false;
  }
  if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
    g_keyboard_input_status.f9 = true;
  } else if (key == GLFW_KEY_F9 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f9 = false;
  }
  if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
    g_keyboard_input_status.f10 = true;
  } else if (key == GLFW_KEY_F10 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f10 = false;
  }
  if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    g_keyboard_input_status.f11 = true;
  } else if (key == GLFW_KEY_F11 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f11 = false;
  }
  if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
    g_keyboard_input_status.f12 = true;
  } else if (key == GLFW_KEY_F12 && action == GLFW_RELEASE) {
    g_keyboard_input_status.f12 = false;
  }
  // number key row
  if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
    g_keyboard_input_status.graveAccent = true;
  } else if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_RELEASE) {
    g_keyboard_input_status.graveAccent = false;
  }
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    g_keyboard_input_status.n1 = true;
  } else if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n1 = false;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    g_keyboard_input_status.n2 = true;
  } else if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n2 = false;
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    g_keyboard_input_status.n3 = true;
  } else if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n3 = false;
  }
  if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
    g_keyboard_input_status.n4 = true;
  } else if (key == GLFW_KEY_4 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n4 = false;
  }
  if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
    g_keyboard_input_status.n5 = true;
  } else if (key == GLFW_KEY_5 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n5 = false;
  }
  if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
    g_keyboard_input_status.n6 = true;
  } else if (key == GLFW_KEY_6 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n6 = false;
  }
  if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
    g_keyboard_input_status.n7 = true;
  } else if (key == GLFW_KEY_7 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n7 = false;
  }
  if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
    g_keyboard_input_status.n8 = true;
  } else if (key == GLFW_KEY_8 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n8 = false;
  }
  if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
    g_keyboard_input_status.n9 = true;
  } else if (key == GLFW_KEY_9 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n9 = false;
  }
  if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
    g_keyboard_input_status.n0 = true;
  } else if (key == GLFW_KEY_0 && action == GLFW_RELEASE) {
    g_keyboard_input_status.n0 = false;
  }
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
    g_keyboard_input_status.minus = true;
  } else if (key == GLFW_KEY_MINUS && action == GLFW_RELEASE) {
    g_keyboard_input_status.minus = false;
  }
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
    g_keyboard_input_status.equal = true;
  } else if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE) {
    g_keyboard_input_status.equal = false;
  }
  if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
    g_keyboard_input_status.backspace = true;
  } else if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
    g_keyboard_input_status.backspace = false;
  }
  // alphabet keys
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    g_keyboard_input_status.a = true;
  } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
    g_keyboard_input_status.a = false;
  }
  if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    g_keyboard_input_status.b = true;
  } else if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
    g_keyboard_input_status.b = false;
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    g_keyboard_input_status.c = true;
  } else if (key == GLFW_KEY_C && action == GLFW_RELEASE) {
    g_keyboard_input_status.c = false;
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    g_keyboard_input_status.d = true;
  } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
    g_keyboard_input_status.d = false;
  }
  if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    g_keyboard_input_status.e = true;
  } else if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
    g_keyboard_input_status.e = false;
  }
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    g_keyboard_input_status.f = true;
  } else if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
    g_keyboard_input_status.f = false;
  }
  if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    g_keyboard_input_status.g = true;
  } else if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
    g_keyboard_input_status.g = false;
  }
  if (key == GLFW_KEY_H && action == GLFW_PRESS) {
    g_keyboard_input_status.h = true;
  } else if (key == GLFW_KEY_H && action == GLFW_RELEASE) {
    g_keyboard_input_status.h = false;
  }
  if (key == GLFW_KEY_I && action == GLFW_PRESS) {
    g_keyboard_input_status.i = true;
  } else if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
    g_keyboard_input_status.i = false;
  }
  if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    g_keyboard_input_status.j = true;
  } else if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
    g_keyboard_input_status.j = false;
  }
  if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    g_keyboard_input_status.k = true;
  } else if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
    g_keyboard_input_status.k = false;
  }
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    g_keyboard_input_status.l = true;
  } else if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
    g_keyboard_input_status.l = false;
  }
  if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    g_keyboard_input_status.m = true;
  } else if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
    g_keyboard_input_status.m = false;
  }
  if (key == GLFW_KEY_N && action == GLFW_PRESS) {
    g_keyboard_input_status.n = true;
  } else if (key == GLFW_KEY_N && action == GLFW_RELEASE) {
    g_keyboard_input_status.n = false;
  }
  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    g_keyboard_input_status.o = true;
  } else if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
    g_keyboard_input_status.o = false;
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    g_keyboard_input_status.p = true;
  } else if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
    g_keyboard_input_status.p = false;
  }
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    g_keyboard_input_status.q = true;
  } else if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
    g_keyboard_input_status.q = false;
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    g_keyboard_input_status.r = true;
  } else if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
    g_keyboard_input_status.r = false;
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    g_keyboard_input_status.s = true;
  } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
    g_keyboard_input_status.s = false;
  }
  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    g_keyboard_input_status.t = true;
  } else if (key == GLFW_KEY_T && action == GLFW_RELEASE) {
    g_keyboard_input_status.t = false;
  }
  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    g_keyboard_input_status.u = true;
  } else if (key == GLFW_KEY_U && action == GLFW_RELEASE) {
    g_keyboard_input_status.u = false;
  }
  if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    g_keyboard_input_status.v = true;
  } else if (key == GLFW_KEY_V && action == GLFW_RELEASE) {
    g_keyboard_input_status.v = false;
  }
  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    g_keyboard_input_status.w = true;
  } else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
    g_keyboard_input_status.w = false;
  }
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    g_keyboard_input_status.x = true;
  } else if (key == GLFW_KEY_X && action == GLFW_RELEASE) {
    g_keyboard_input_status.x = false;
  }
  if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    g_keyboard_input_status.y = true;
  } else if (key == GLFW_KEY_Y && action == GLFW_RELEASE) {
    g_keyboard_input_status.y = false;
  }
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    g_keyboard_input_status.z = true;
  } else if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
    g_keyboard_input_status.z = false;
  }
  // tab-shift-control-alt
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    g_keyboard_input_status.tab = true;
  } else if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
    g_keyboard_input_status.tab = false;
  }
  if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
    g_keyboard_input_status.leftShift = true;
  } else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
    g_keyboard_input_status.leftShift = false;
  }
  if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS) {
    g_keyboard_input_status.rightShift = true;
  } else if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_RELEASE) {
    g_keyboard_input_status.rightShift = false;
  }
  if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
    g_keyboard_input_status.leftControl = true;
  } else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
    g_keyboard_input_status.leftControl = false;
  }
  if (key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_PRESS) {
    g_keyboard_input_status.rightControl = true;
  } else if (key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_RELEASE) {
    g_keyboard_input_status.rightControl = false;
  }
  if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
    g_keyboard_input_status.leftAlt = true;
  } else if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
    g_keyboard_input_status.leftAlt = false;
  }
  if (key == GLFW_KEY_RIGHT_ALT && action == GLFW_PRESS) {
    g_keyboard_input_status.rightAlt = true;
  } else if (key == GLFW_KEY_RIGHT_ALT && action == GLFW_RELEASE) {
    g_keyboard_input_status.rightAlt = false;
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    g_keyboard_input_status.spacebar = true;
  } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
    g_keyboard_input_status.spacebar = false;
  }
  // brackets
  if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
    g_keyboard_input_status.leftSquareBracket = true;
  } else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
    g_keyboard_input_status.leftSquareBracket = false;
  }
  if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
    g_keyboard_input_status.rightSquareBracket = true;
  } else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
    g_keyboard_input_status.rightSquareBracket = false;
  }
  // slash-quote-semicolon-enter
  if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
    g_keyboard_input_status.backslash = true;
  } else if (key == GLFW_KEY_BACKSLASH && action == GLFW_RELEASE) {
    g_keyboard_input_status.backslash = false;
  }
  if (key == GLFW_KEY_SEMICOLON && action == GLFW_PRESS) {
    g_keyboard_input_status.semiColon = true;
  } else if (key == GLFW_KEY_SEMICOLON && action == GLFW_RELEASE) {
    g_keyboard_input_status.semiColon = false;
  }
  if (key == GLFW_KEY_APOSTROPHE && action == GLFW_PRESS) {
    g_keyboard_input_status.apostrophe = true;
  } else if (key == GLFW_KEY_APOSTROPHE && action == GLFW_RELEASE) {
    g_keyboard_input_status.apostrophe = false;
  }
  if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
    g_keyboard_input_status.enter = true;
  } else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
    g_keyboard_input_status.enter = false;
  }
  // comma-period-forwardslash
  if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
    g_keyboard_input_status.comma = true;
  } else if (key == GLFW_KEY_COMMA && action == GLFW_RELEASE) {
    g_keyboard_input_status.comma = false;
  }
  if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS) {
    g_keyboard_input_status.period = true;
  } else if (key == GLFW_KEY_PERIOD && action == GLFW_RELEASE) {
    g_keyboard_input_status.period = false;
  }
  if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
    g_keyboard_input_status.forwardSlash = true;
  } else if (key == GLFW_KEY_SLASH && action == GLFW_RELEASE) {
    g_keyboard_input_status.forwardSlash = false;
  }
  // printscreen-etc
  if (key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS) {
    g_keyboard_input_status.printScreen = true;
  } else if (key == GLFW_KEY_PRINT_SCREEN && action == GLFW_RELEASE) {
    g_keyboard_input_status.printScreen = false;
  }
  if (key == GLFW_KEY_SCROLL_LOCK && action == GLFW_PRESS) {
    g_keyboard_input_status.scrollLock = true;
  } else if (key == GLFW_KEY_SCROLL_LOCK && action == GLFW_RELEASE) {
    g_keyboard_input_status.scrollLock = false;
  }
  if (key == GLFW_KEY_PAUSE && action == GLFW_PRESS) {
    g_keyboard_input_status.pauseBreak = true;
  } else if (key == GLFW_KEY_PAUSE && action == GLFW_RELEASE) {
    g_keyboard_input_status.pauseBreak = false;
  }
  if (key == GLFW_KEY_INSERT && action == GLFW_PRESS) {
    g_keyboard_input_status.insert = true;
  } else if (key == GLFW_KEY_INSERT && action == GLFW_RELEASE) {
    g_keyboard_input_status.insert = false;
  }
  if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
    g_keyboard_input_status.del = true;
  } else if (key == GLFW_KEY_DELETE && action == GLFW_RELEASE) {
    g_keyboard_input_status.del = false;
  }
  if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
    g_keyboard_input_status.home = true;
  } else if (key == GLFW_KEY_HOME && action == GLFW_RELEASE) {
    g_keyboard_input_status.home = false;
  }
  if (key == GLFW_KEY_END && action == GLFW_PRESS) {
    g_keyboard_input_status.end = true;
  } else if (key == GLFW_KEY_END && action == GLFW_RELEASE) {
    g_keyboard_input_status.end = false;
  }
  if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) {
    g_keyboard_input_status.pageUp = true;
  } else if (key == GLFW_KEY_PAGE_UP && action == GLFW_RELEASE) {
    g_keyboard_input_status.pageUp = false;
  }
  if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS) {
    g_keyboard_input_status.pageDown = true;
  } else if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_RELEASE) {
    g_keyboard_input_status.pageDown = false;
  }
  // arrows
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    g_keyboard_input_status.upArrow = true;
  } else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
    g_keyboard_input_status.upArrow = false;
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    g_keyboard_input_status.downArrow = true;
  } else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
    g_keyboard_input_status.downArrow = false;
  }
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    g_keyboard_input_status.leftArrow = true;
  } else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
    g_keyboard_input_status.leftArrow = false;
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    g_keyboard_input_status.rightArrow = true;
  } else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
    g_keyboard_input_status.rightArrow = false;
  }
  g_new_key_reads = true;
}

void MOUSEBUTTONCALLBACK(GLFWwindow* w, int button, int action, int mods) {
  // mouse clicks
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    g_mouse_button_status.mouseButton1 = true;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    g_mouse_button_status.mouseButton1 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    g_mouse_button_status.mouseButton2 = true;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    g_mouse_button_status.mouseButton2 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    g_mouse_button_status.mouseButton3 = true;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    g_mouse_button_status.mouseButton3 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS) {
    g_mouse_button_status.mouseButton4 = true;
  } else if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_RELEASE) {
    g_mouse_button_status.mouseButton4 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS) {
    g_mouse_button_status.mousebutton5 = true;
  } else if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_RELEASE) {
    g_mouse_button_status.mousebutton5 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_6 && action == GLFW_PRESS) {
    g_mouse_button_status.mouseButton6 = true;
  } else if (button == GLFW_MOUSE_BUTTON_6 && action == GLFW_RELEASE) {
    g_mouse_button_status.mouseButton6 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_7 && action == GLFW_PRESS) {
    g_mouse_button_status.mousebutton7 = true;
  } else if (button == GLFW_MOUSE_BUTTON_7 && action == GLFW_RELEASE) {
    g_mouse_button_status.mousebutton7 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_8 && action == GLFW_PRESS) {
    g_mouse_button_status.mouseButton8 = true;
  } else if (button == GLFW_MOUSE_BUTTON_8 && action == GLFW_RELEASE) {
    g_mouse_button_status.mouseButton8 = false;
  }
  g_new_key_reads = true;
}

/*    
*   LEGACY FUNCTIONS FROM V005
* 
* 
// Switches To First Person Perspective Mouse Reporting Style 
// disabled/hidden cursor - calls SetMouseToDisabled()
// repeated offset from center with mFPPMouseSensitivity applied to onMouseHandling funcs
// notes: overrides any previous glfwSetCursorPosCallback callback settings
void SetMouseReadToPerspectiveLookAround() noexcept {
  if (mMouseReporting == MouseReporting::PERSPECTIVE)
    return;  // already in perspective handling
  mMouseReporting = MouseReporting::PERSPECTIVE;
  SetMouseToDisabled();
  static double lastX{ 0 }, lastY{ 0 };
  static double xDelta{ 0 }, yDelta{ 0 };
  glfwSetCursorPos(mGLFWwindow, 0, 0);
  mMousePosition.xOffset = 0;
  mMousePosition.yOffset = 0;
  lastX = 0;
  lastY = 0;

  // will be called anytime the mouse moves
  ::glfwSetCursorPosCallback(mGLFWwindow, [](GLFWwindow* window, double x, double y) {
    // reset 
    xDelta = 0, yDelta = 0;
    // get change
    xDelta = x - lastX;
    yDelta = lastY - y;
    // hold result
    lastX = x;
    lastY = y;
    // get a percent of the actual offset
    xDelta *= mFPPMouseSensitivity;
    yDelta *= mFPPMouseSensitivity;
    // apply it to our mouse position
    mMousePosition.xOffset = xDelta;
    mMousePosition.yOffset = yDelta;
    // have your functions handle this mouse calculation
    for (auto& oMH : onMouseHandling) { oMH.second(mMousePosition); }
    // recenter your mouse position after it has been handled
    mMousePosition.xOffset = 0;
    mMousePosition.yOffset = 0;
  });
}


// Switches To Normal Cursor Mouse Handling Style
// sets cursor to normal mode - calls SetMouseToNormal()
// updates mouse on mGUI_testing shader
// mouse pos reported on onMouseHandling funcs
void SetMouseReadToNormal() noexcept {
  if (mMouseReporting == MouseReporting::STANDARD)  // already standard
    return;
  mMouseReporting = MouseReporting::STANDARD;

  SetMouseToNormal();
  ::glfwSetCursorPosCallback(mGLFWwindow, [](GLFWwindow* window, double xpos, double ypos) {
    mMousePosition.xOffset = xpos;
    mMousePosition.yOffset = ypos;
    if (mGUI_testing) {
      mGUI_testing->UpdateMouseLoc(vec2(mMousePosition.xOffset, mMousePosition.yOffset));
    }
    for (auto& oMH : onMouseHandling) { oMH.second(mMousePosition); }
  });
}


// drag and drop will only work if you have it enabled and there is at least 1 camera

::glfwSetDropCallback(mGLFWwindow, [](GLFWwindow* w, int count, const char** paths) {
// todo: fix
//if (!Settings::Get()->GetOptions().drag_and_drop_files_support)
//  return;
//if (mCameras.empty())
//  return;
//int i;
//for (i = 0; i < count; i++) {
//  std::string proc_ = paths[i];
//  std::size_t the_last_slash = proc_.find_last_of("/\\") + 1;
//  std::size_t the_last_dot = proc_.find_last_of(".");
//  //std::string path_to = proc_.substr(0, the_last_slash);  // path to filename's dir
//  std::string file_extension = proc_.substr(
//    static_cast<std::basic_string<char,
//    std::char_traits<char>,
//    std::allocator<char>>::size_type>(the_last_dot) + 1);  // get the file extension (type of file)
//  if (file_extension == "fbx" || file_extension == "FBX" ||
//    file_extension == "obj" || file_extension == "OBJ" ||
//    file_extension == "dae" || file_extension == "DAE" ||
//    file_extension == "glb" || file_extension == "gltf"
//    ) {
//    int added_prop = AddProp(paths[i]);
//  }
//}
});


::glfwSetFramebufferSizeCallback(mGLFWwindow, [](GLFWwindow* window, int w, int h) {
OGLGraphics::SetViewportSize(0, 0, w, h);
if (mCameras.empty())
return;

for (auto& cam : mCameras) {
cam.updateProjectionMatrix(w, h);
}
if (mDiffShader) {
mDiffShader->Use();
mDiffShader->SetMat4("u_projection_matrix", mCameras.front().mProjectionMatrix);
}
if (mLitShader) {
mLitShader->Use();
mLitShader->SetMat4("u_projection_matrix", mCameras.front().mProjectionMatrix);
}
if (mSkybox) {
mSkybox->SetProjectionMatrix(mCameras.front().mProjectionMatrix);
}

});

::glfwSetScrollCallback(mGLFWwindow, [](GLFWwindow* w, double x, double y) {
mMouseWheelScroll.xOffset = x;
mMouseWheelScroll.yOffset = y;
// process scroll wheel and reset back to 0
for (const auto& oSH : onScrollHandling) { oSH.second(mMouseWheelScroll); }
mMouseWheelScroll.yOffset = 0;
mMouseWheelScroll.xOffset = 0;
});



 END LEGACY FUNCTIONS */

}  // end namespace AA