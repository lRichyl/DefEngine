#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include "window.h"

namespace def {
     Window* create_window(int width, int height, const char *title){
          Window *window = new Window;
          stbi_set_flip_vertically_on_load(true);
          if(!glfwInit()){
               printf("glfw could not be initialized.");
               exit(0);
          }
          glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
          glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
          glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
          window->GLFWInstance = glfwCreateWindow(width, height, title, NULL, NULL);

          //This variables represent the internal resolution. Every calculation is based on these so that
          //if you resize the window the everything scales correctly.
          //This should be set only once at the start of the program.
     	 // Default values.
          window->internalWidth = width;
          window->internalHeight = height;

          if(!window->GLFWInstance){
               glfwTerminate();
               printf("Window could not be created.");
               exit(0);
          }
          glfwMakeContextCurrent(window->GLFWInstance);
          // glfwSwapInterval(1);
          if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
          {
              printf("Failed to initialize GLAD");
              exit(0);
          }
          glEnable(GL_BLEND);
     	 // glEnable(GL_DEPTH_TEST);
     	 // glDepthMask(false);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          return window;
     }

     void set_window_size(Window *window, int width, int height){
     	glfwSetWindowSize(window->GLFWInstance, width, height);
     }

     void swap_buffers(Window *window){
          glfwSwapBuffers(window->GLFWInstance);
     }

     void poll_events(){
          glfwPollEvents();
     }
     void destroy_window(Window *window){
          delete window;
     }
}
