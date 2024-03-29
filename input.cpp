#include "input.h"
#include <queue>
#include <memory.h>

namespace def {
	DefArray<unsigned int> Input::unicode_array;
	// static std::queue<Event> events; 
	unsigned int           Input::keys_state[KEYS_AMOUNT];

	static MouseInfo mouseInfo;

	static void get_mouse_button_state(MouseButton *mb_info){
		
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

	// bool GetNextEvent(Event *event){
	     // Event e = {};
	     // if(events.size() == 0) return false;

	     // Event e = events.front();
	     // event->key = e.key;
	     // event->action = e.action;
	     // events.pop();
	     // return true;

	// }

	bool is_key_being_pressed(Window *window, int key){
		int state = glfwGetKey(window->GLFWInstance, key);
		return state == GLFW_PRESS;
	}

	bool was_key_pressed(int key){
		if(key >= KEYS_AMOUNT){
			printf("Surpassed the maximum key index : %d\n", key);
		}
		int state = Input::keys_state[key];
		return state == GLFW_PRESS;
	}

	bool was_key_released(int key){
		int state = Input::keys_state[key];
		return state == GLFW_RELEASE;
	}

	void clear_keys_state(){
		memset(Input::keys_state, 3, sizeof(unsigned int) * KEYS_AMOUNT);
	}

	void PollKeyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mods){
		Input::keys_state[key] = action;
	}

	void CursorCallback(GLFWwindow* window, double xpos, double ypos){
		mouseInfo.position.x = xpos;
		mouseInfo.position.y = ypos;
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
		if(yoffset > 0){
			mouseInfo.wheel = ScrollWheelState::WHEEL_FORWARDS;
			// printf("Wheel up\n");
		} 
		else if (yoffset < 0){
			mouseInfo.wheel = ScrollWheelState::WHEEL_BACKWARDS;
			// printf("Wheel down\n");
		} 
	}

	void character_callback(GLFWwindow* window, unsigned int codepoint){
		add_array(&Input::unicode_array, codepoint);
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

	void set_scroll_wheel_callback(Window *window){
		glfwSetScrollCallback(window->GLFWInstance, scroll_callback);
	}

	void set_character_callback(Window *window){
		glfwSetCharCallback(window->GLFWInstance, character_callback);
	}


	MouseInfo GetMouseInfo(Window *window){
		MouseInfo result = mouseInfo;
		int windowWidth;
		int windowHeight;
		glfwGetWindowSize(window->GLFWInstance, &windowWidth, &windowHeight);
		
		double xBufferToWindowFactor = (double)window->internalWidth / (float)windowWidth; 
		double yBufferToWindowFactor = (double)window->internalHeight / (float)windowHeight;
		
		result.position.x = mouseInfo.position.x * xBufferToWindowFactor;
		result.position.y = mouseInfo.position.y * yBufferToWindowFactor;
		
		result.position.y = window->internalHeight - result.position.y;
		// printf("%f , %f , %f\n", xBufferToWindowFactor, result.x, result.y);
		
		get_mouse_button_state(&mouseInfo.left);
		get_mouse_button_state(&mouseInfo.right);

		result.left = mouseInfo.left;
		result.right = mouseInfo.right;
		
		// result.wheel = mouseInfo.wheel;

		
		return result;
	}

	void clear_mouse_info(){
		mouseInfo.wheel = ScrollWheelState::WHEEL_NONE;
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

	// void PrintEvents(){
	     // for(int i = 0; i < events.size(); i++){
	          // Event e = events.front();
	          // printf("%d\n", e.key);
	          // printf("%d\n", e.action);
	          // printf("\n\n");
	          // events.pop();
	     // }
	// }
}