#pragma once
#include "sprite.h"
#include "math.h"
#include "renderer.h"

struct Button{
	bool value;
	Rect bounding_box;
	const char *text;
	Sprite sprite;
};

void init_button(Button *button, const char *text);
void set_button_position(Button *button, V2 position);
void set_button_size(Button *button, V2 size);
void update_button(Button *button);
void render_button(Button *button, Renderer *renderer);