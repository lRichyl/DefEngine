#ifndef SPRITE_H
#define SPRITE_H
#include "renderer.h"
#include "math.h"

struct Sprite{
	Texture texture;
	Rect clipping_box {};
	bool is_x_mirrored = false;
	bool is_y_mirrored = false;
	float alpha = 1;
	int layer = 0;
	V3 tint = {1,1,1};
	V2 size = {32, 32};
	V2 position = {};

};

void render_sprite(Renderer *renderer, Sprite *spr, V2 position);
// void render_sprite_with_transparency(Renderer *renderer, Sprite *spr, V2 position, )
void render_sprite_as_background(Renderer *renderer, Sprite *spr);


#endif