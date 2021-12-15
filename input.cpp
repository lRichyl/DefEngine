#include "input.h"
#include <queue>
#include <stdio.h>

static std::queue<Event> events;

static MouseInfo mouseInfo;

static void set_mouse_button_state(MouseButton *mb_info){
	
	if(mb_info->state == MouseButtonState::MOUSE_NONE && mb_info->gl_key_state == GLFW_PRESS){
		mb_info->state = MouseButtonState::MOUSE_PRESSED;
	}
	else if(mb_info->state == MouseButtonState::MOUSE_PRESSED && mb_info->gl_key_state == GLFW_PRESS){
		mb_info->state = MouseButtonState::MOUSE_HELD;
	}
	else if(mb_info->state == MouseButtonState::MOUSE_PRESSED){
		mb_info->state = MouseButtonState::MOUSE_NONE;
	}
	
	if(mb_info->state == MouseButtonState::MOUSE_HELD && mb_info->gl_key_state == GLFW_RELEASE){
		mb_info->state = MouseButtonState::MOUSE_RELEASED;
	}
	else if(mb_info->state == MouseButtonState::MOUSE_RELEASED && mb_info->gl_key_state == GLFW_RELEASE){
		mb_info->state = MouseButtonState::MOUSE_NONE;
	}
	

}

bool GetNextEvent(Event *event){
     // Event e = {};
     if(events.size() == 0) return false;

     Event e = events.front();
     event->key = e.key;
     event->action = e.action;
     events.pop();
     return true;

}

bool IsKeyPressed(Window *window, int key){
     int state = glfwGetKey(window->GLFWInstance, key);
     return state == GLFW_PRESS;
    
}

void PollKeyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
     Event e = {key , action};
     events.push(e);
}

void CursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouseInfo.x = xpos;
	mouseInfo.y = ypos;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	if(button == GLFW_MOUSE_BUTTON_LEFT){
		mouseInfo.left.gl_key_state = action;
		// printf("Left press\n");
	}else if(button == GLFW_MOUSE_BUTTON_RIGHT){
		mouseInfo.right.gl_key_state = action;
		// printf("Right press\n");
	}
	
	
}

//TODO: Set here the different callbacks to glfw and change the name of the function.
void SetKeyboardCallback(Window *window){
	glfwSetKeyCallback(window->GLFWInstance, PollKeyboardEvents);
}

void SetCursorCallback(Window *window){
	
	glfwSetCursorPosCallback(window->GLFWInstance, CursorCallback);
}

void SetMouseButtonCallback(Window *window){
	glfwSetMouseButtonCallback(window->GLFWInstance, MouseButtonCallback);
}



MouseInfo GetMouseInfo(Window *window){
	MouseInfo result;
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window->GLFWInstance, &windowWidth, &windowHeight);
	
	double xBufferToWindowFactor = (double)window->internalWidth / (float)windowWidth; 
	double yBufferToWindowFactor = (double)window->internalHeight / (float)windowHeight;
	
	result.x = mouseInfo.x * xBufferToWindowFactor;
	result.y = mouseInfo.y * yBufferToWindowFactor;
	
	result.y = window->internalHeight - result.y;
	// printf("%f , %f , %f\n", xBufferToWindowFactor, result.x, result.y);
	
	set_mouse_button_state(&mouseInfo.left);
	set_mouse_button_state(&mouseInfo.right);

	result.left = mouseInfo.left;
	result.right = mouseInfo.right;
	
	return result;
}

void PrintMouseInfo(MouseInfo *mouse){
	if(mouse->left.state == MouseButtonState::MOUSE_RELEASED){
		printf("LEFT RELEASED\n");
	}else if(mouse->left.state == MouseButtonState::MOUSE_PRESSED){
		printf("LEFT PRESSED\n");
	}else if(mouse->left.state == MouseButtonState::MOUSE_HELD){
		printf("LEFT HELD\n");
	}
	
	if(mouse->right.state == MouseButtonState::MOUSE_RELEASED){
		printf("RIGHT RELEASED\n");
	}else if(mouse->right.state == MouseButtonState::MOUSE_PRESSED){
		printf("RIGHT PRESSED\n");
	}else if(mouse->right.state == MouseButtonState::MOUSE_HELD){
		printf("RIGHT HELD\n");
	}
}

void PrintEvents(){
     for(int i = 0; i < events.size(); i++){
          Event e = events.front();
          printf("%d\n", e.key);
          printf("%d\n", e.action);
          printf("\n\n");
          events.pop();
     }
}
