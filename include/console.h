#pragma once
#include "renderer.h"
#include "text.h"
// Console color 120,130,50.

struct Console{
	Rect bounding_box;
	Rect text_box_divider;
	V3 color = {120, 130, 50};
	V3 division_color;
	Font *font = NULL;
	
	// We render the console with its own shader to set individually its MVP so it will not scale and move
	// when we move the camera.
	ShaderProgram shader;
	float scroll_offset = 0;
	float text_box_divider_height = 10;
};

void init_console(Console *console, Renderer *renderer);
void update_console(Console *console);
void render_console(Console *console, Renderer *renderer);