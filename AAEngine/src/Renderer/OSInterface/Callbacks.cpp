#include "Window.h"
#include "../OpenGL/OGLGraphics.h"
#include <memory>
//#include <iostream>

namespace AA {

bool extern_NewKeyReads = false;
bool extern_ProjectionResized = true;  // start true so projection updates right away

void extern_Pollevents() {
  glfwPollEvents();
}

// todo: reset the reciever structs back to default on window switch (maybe?)
KeyboardButtons  extern_KeyboardInputReciever;
MouseScrollWheel extern_ScrollInputReciever;
MouseCursorPos   extern_MouseInputReciever;
MouseButtons     extern_MouseButtonReciever; 

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
  extern_ProjectionResized = true;
}

void NORMALMOUSEREPORTINGCALLBACK(GLFWwindow* window, double xpos, double ypos) {
  // Update Cached Position
  extern_MouseInputReciever.xOffset = xpos;
  extern_MouseInputReciever.yOffset = ypos;
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
    extern_KeyboardInputReciever.esc = true;
  } else if (key == GLFW_KEY_ESCAPE == GLFW_RELEASE) {
    extern_KeyboardInputReciever.esc = false;
  }
  // function keys
  if (key == GLFW_KEY_F1 == GLFW_PRESS) {
    extern_KeyboardInputReciever.f1 = true;
  } else if (key == GLFW_KEY_F1 == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f1 = false;
  }
  if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f2 = true;
  } else if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f2 = false;
  }
  if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f3 = true;
  } else if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f3 = false;
  }
  if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f4 = true;
  } else if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f4 = false;
  }
  if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f5 = true;
  } else if (key == GLFW_KEY_F5 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f5 = false;
  }
  if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f6 = true;
  } else if (key == GLFW_KEY_F6 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f6 = false;
  }
  if (key == GLFW_KEY_F7 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f7 = true;
  } else if (key == GLFW_KEY_F7 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f7 = false;
  }
  if (key == GLFW_KEY_F8 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f8 = true;
  } else if (key == GLFW_KEY_F8 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f8 = false;
  }
  if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f9 = true;
  } else if (key == GLFW_KEY_F9 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f9 = false;
  }
  if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f10 = true;
  } else if (key == GLFW_KEY_F10 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f10 = false;
  }
  if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f11 = true;
  } else if (key == GLFW_KEY_F11 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f11 = false;
  }
  if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f12 = true;
  } else if (key == GLFW_KEY_F12 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f12 = false;
  }
  // number key row
  if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.graveAccent = true;
  } else if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.graveAccent = false;
  }
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n1 = true;
  } else if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n1 = false;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n2 = true;
  } else if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n2 = false;
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n3 = true;
  } else if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n3 = false;
  }
  if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n4 = true;
  } else if (key == GLFW_KEY_4 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n4 = false;
  }
  if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n5 = true;
  } else if (key == GLFW_KEY_5 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n5 = false;
  }
  if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n6 = true;
  } else if (key == GLFW_KEY_6 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n6 = false;
  }
  if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n7 = true;
  } else if (key == GLFW_KEY_7 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n7 = false;
  }
  if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n8 = true;
  } else if (key == GLFW_KEY_8 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n8 = false;
  }
  if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n9 = true;
  } else if (key == GLFW_KEY_9 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n9 = false;
  }
  if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n0 = true;
  } else if (key == GLFW_KEY_0 && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n0 = false;
  }
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.minus = true;
  } else if (key == GLFW_KEY_MINUS && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.minus = false;
  }
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.equal = true;
  } else if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.equal = false;
  }
  if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.backspace = true;
  } else if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.backspace = false;
  }
  // alphabet keys
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.a = true;
  } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.a = false;
  }
  if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.b = true;
  } else if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.b = false;
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.c = true;
  } else if (key == GLFW_KEY_C && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.c = false;
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.d = true;
  } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.d = false;
  }
  if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.e = true;
  } else if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.e = false;
  }
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.f = true;
  } else if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.f = false;
  }
  if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.g = true;
  } else if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.g = false;
  }
  if (key == GLFW_KEY_H && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.h = true;
  } else if (key == GLFW_KEY_H && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.h = false;
  }
  if (key == GLFW_KEY_I && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.i = true;
  } else if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.i = false;
  }
  if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.j = true;
  } else if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.j = false;
  }
  if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.k = true;
  } else if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.k = false;
  }
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.l = true;
  } else if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.l = false;
  }
  if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.m = true;
  } else if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.m = false;
  }
  if (key == GLFW_KEY_N && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.n = true;
  } else if (key == GLFW_KEY_N && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.n = false;
  }
  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.o = true;
  } else if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.o = false;
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.p = true;
  } else if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.p = false;
  }
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.q = true;
  } else if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.q = false;
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.r = true;
  } else if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.r = false;
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.s = true;
  } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.s = false;
  }
  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.t = true;
  } else if (key == GLFW_KEY_T && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.t = false;
  }
  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.u = true;
  } else if (key == GLFW_KEY_U && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.u = false;
  }
  if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.v = true;
  } else if (key == GLFW_KEY_V && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.v = false;
  }
  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.w = true;
  } else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.w = false;
  }
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.x = true;
  } else if (key == GLFW_KEY_X && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.x = false;
  }
  if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.y = true;
  } else if (key == GLFW_KEY_Y && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.y = false;
  }
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.z = true;
  } else if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.z = false;
  }
  // tab-shift-control-alt
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.tab = true;
  } else if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.tab = false;
  }
  if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.leftShift = true;
  } else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.leftShift = false;
  }
  if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.rightShift = true;
  } else if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.rightShift = false;
  }
  if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.leftControl = true;
  } else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.leftControl = false;
  }
  if (key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.rightControl = true;
  } else if (key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.rightControl = false;
  }
  if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.leftAlt = true;
  } else if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.leftAlt = false;
  }
  if (key == GLFW_KEY_RIGHT_ALT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.rightAlt = true;
  } else if (key == GLFW_KEY_RIGHT_ALT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.rightAlt = false;
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.spacebar = true;
  } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.spacebar = false;
  }
  // brackets
  if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.leftSquareBracket = true;
  } else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.leftSquareBracket = false;
  }
  if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.rightSquareBracket = true;
  } else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.rightSquareBracket = false;
  }
  // slash-quote-semicolon-enter
  if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.backslash = true;
  } else if (key == GLFW_KEY_BACKSLASH && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.backslash = false;
  }
  if (key == GLFW_KEY_SEMICOLON && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.semiColon = true;
  } else if (key == GLFW_KEY_SEMICOLON && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.semiColon = false;
  }
  if (key == GLFW_KEY_APOSTROPHE && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.apostrophe = true;
  } else if (key == GLFW_KEY_APOSTROPHE && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.apostrophe = false;
  }
  if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.enter = true;
  } else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.enter = false;
  }
  // comma-period-forwardslash
  if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.comma = true;
  } else if (key == GLFW_KEY_COMMA && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.comma = false;
  }
  if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.period = true;
  } else if (key == GLFW_KEY_PERIOD && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.period = false;
  }
  if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.forwardSlash = true;
  } else if (key == GLFW_KEY_SLASH && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.forwardSlash = false;
  }
  // printscreen-etc
  if (key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.printScreen = true;
  } else if (key == GLFW_KEY_PRINT_SCREEN && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.printScreen = false;
  }
  if (key == GLFW_KEY_SCROLL_LOCK && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.scrollLock = true;
  } else if (key == GLFW_KEY_SCROLL_LOCK && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.scrollLock = false;
  }
  if (key == GLFW_KEY_PAUSE && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.pauseBreak = true;
  } else if (key == GLFW_KEY_PAUSE && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.pauseBreak = false;
  }
  if (key == GLFW_KEY_INSERT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.insert = true;
  } else if (key == GLFW_KEY_INSERT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.insert = false;
  }
  if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.del = true;
  } else if (key == GLFW_KEY_DELETE && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.del = false;
  }
  if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.home = true;
  } else if (key == GLFW_KEY_HOME && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.home = false;
  }
  if (key == GLFW_KEY_END && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.end = true;
  } else if (key == GLFW_KEY_END && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.end = false;
  }
  if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.pageUp = true;
  } else if (key == GLFW_KEY_PAGE_UP && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.pageUp = false;
  }
  if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.pageDown = true;
  } else if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.pageDown = false;
  }
  // arrows
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.upArrow = true;
  } else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.upArrow = false;
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.downArrow = true;
  } else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.downArrow = false;
  }
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.leftArrow = true;
  } else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.leftArrow = false;
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    extern_KeyboardInputReciever.rightArrow = true;
  } else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
    extern_KeyboardInputReciever.rightArrow = false;
  }
  extern_NewKeyReads = true;
}

void MOUSEBUTTONCALLBACK(GLFWwindow* w, int button, int action, int mods) {
  // mouse clicks
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mouseButton1 = true;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mouseButton1 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mouseButton2 = true;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mouseButton2 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mouseButton3 = true;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mouseButton3 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mouseButton4 = true;
  } else if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mouseButton4 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mousebutton5 = true;
  } else if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mousebutton5 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_6 && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mouseButton6 = true;
  } else if (button == GLFW_MOUSE_BUTTON_6 && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mouseButton6 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_7 && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mousebutton7 = true;
  } else if (button == GLFW_MOUSE_BUTTON_7 && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mousebutton7 = false;
  }
  if (button == GLFW_MOUSE_BUTTON_8 && action == GLFW_PRESS) {
    extern_MouseButtonReciever.mouseButton8 = true;
  } else if (button == GLFW_MOUSE_BUTTON_8 && action == GLFW_RELEASE) {
    extern_MouseButtonReciever.mouseButton8 = false;
  }
  extern_NewKeyReads = true;
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
