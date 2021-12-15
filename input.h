#ifndef EVENTS_H
#define EVENTS_H

#include "GLFW/glfw3.h"
#include "window.h"

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

struct MouseButton{
	int button;
	int gl_key_state;
	MouseButtonState state = MOUSE_NONE;
};

struct MouseInfo{
	double x;
	double y;
	MouseButton left {GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE};
	MouseButton right {GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE};
};


bool IsKeyPressed(Window *window, int key);
void PollKeyboardEvents();
void SetKeyboardCallback(Window *window);
void SetCursorCallback(Window *window);
void SetMouseButtonCallback(Window *window);
void PrintMouseInfo(MouseInfo *mouse);
void PrintEvents();
bool GetNextEvent(Event *event);

MouseInfo GetMouseInfo(Window *window);

#endif
