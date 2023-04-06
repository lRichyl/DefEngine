#pragma once
#include "defines.h"
#include "sprite.h"
#include "math.h"
#include "renderer.h"

namespace def{
	struct Button{
		bool is_pressed;
		RECT bounding_box;
		CHR_STR_CON text;
		Sprite sprite;
	};

	void init_button(Button *button, CHR_STR_CON text);
	void set_button_position(Button *button, VEC_2D position);
	void set_button_size(Button *button, VEC_2D size);
	void set_button_sprite(Button *button, Texture texture, RECT clip_region);
	void update_button(Button *button);
	void render_button(Button *button, Renderer *renderer);
}