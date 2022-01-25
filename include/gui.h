#pragma once
#include "math.h"
#include "renderer.h"
#include "sprite.h"

#include <vector>

struct Button{
	Rect bounding_box;
	const char *text;
	Sprite sprite;
};


// The UI elements will be placed inside this frame.
struct Frame{
	float horizontal_padding;
	float vertical_padding;
	int work_area_layer;
	int gui_elements_layer;
	Rect bounding_box;
	Rect work_area;
	Sprite sprite;
	
	std::vector<Button> buttons;
};

void init_frame(Frame *frame, Rect bounding_box, Texture texture);
void set_padding(Frame *frame, float hpad, float vpad);
void move_frame(Frame *frame, V2 new_pos);
void render_frame(Renderer *renderer, Frame *frame);


void add_button(Frame *frame, Rect bounding_box, const char* text, Texture texture, Rect clipping_box);