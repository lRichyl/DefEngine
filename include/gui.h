#pragma once
#include "sprite.h"
#include "math.h"
#include "renderer.h"
#include "console.h"

struct Button{
	bool is_pressed;
	Rect bounding_box;
	const char *text;
	Sprite sprite;
};

void init_button(Button *button, const char *text);
void set_button_position(Button *button, V2 position);
void set_button_size(Button *button, V2 size);
void set_button_sprite(Button *button, Texture texture, Rect clip_region);
void update_button(Button *button);
void render_button(Button *button, Renderer *renderer);

struct TempTextInput{
	bool show = false;
	float height;
	EditableString text;
	Rect bounding_box;
	V3 color;
};

void init_temp_text_input(TempTextInput *tmp_text_in, float text_box_height);
void render_temp_text_input(TempTextInput *tmp_text_in, Renderer *renderer);
void update_temp_text_input(TempTextInput *tmp_text_in, Renderer *renderer);

