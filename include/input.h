#pragma once

#include "GLFW/glfw3.h"
#include "window.h"
#include "def_math.h"
#include "def_array.h"

#define KEYS_AMOUNT 100

struct Input{
	static DefArray<unsigned int> unicode_array;
	static unsigned int keys_state[KEYS_AMOUNT];
};

struct Event{
     int key;
     int action;
};

enum MouseButtonState{
	MOUSE_NONE,
	MOUSE_RELEASED,
	MOUSE_PRESSED,
	MOUSE_HELD
};

enum ScrollWheelState{
	WHEEL_NONE,
	WHEEL_FORWARDS,
	WHEEL_BACKWARDS
};

struct MouseButton{
	int button;
	int gl_key_state;
	MouseButtonState state = MOUSE_NONE;
};

struct MouseInfo{
	V2 position;
	MouseButton left {GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE};
	MouseButton right {GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE};
	ScrollWheelState wheel = ScrollWheelState::WHEEL_NONE;
};


bool is_key_being_pressed(Window *window, int key);
bool was_key_pressed(int key);
bool was_key_released(int key);
void clear_keys_state();
void PollKeyboardEvents();
void SetKeyboardCallback(Window *window);
void SetCursorCallback(Window *window);
void SetMouseButtonCallback(Window *window);
void set_scroll_wheel_callback(Window *window);
void set_character_callback(Window *window);
void PrintMouseInfo(MouseInfo *mouse);
// void PrintEvents();
// bool GetNextEvent(Event *event);

MouseInfo GetMouseInfo(Window *window);
void clear_mouse_info();


