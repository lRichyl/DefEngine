#pragma once
#include "GLFW/glfw3.h"
namespace def {
     struct Window{
          GLFWwindow *GLFWInstance;
          int internalWidth = 0;
          int internalHeight = 0;
     };

     Window* create_window(int width, int height, const char *title);
     void set_window_size(Window *window, int width, int height);
     void swap_buffers(Window *window);
     void poll_events();
     void destroy_window(Window *window);
}
