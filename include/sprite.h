#ifndef SPRITE_H
#define SPRITE_H
#include "renderer.h"
#include "math.h"
#include "timer.h"

#include <vector>

// Clipping boxes values are based on the top left corner of the texture. 
// Position is the position of the top left corner of the sprite. The origin of world coordinates is on the
// bottom left corner of the screen.

struct SpriteInfo{
	Texture texture;
	bool is_x_mirrored = false;
	bool is_y_mirrored = false;
	float alpha = 1;
	int layer = 0;
	V3 tint = {1,1,1};
	V2 size = {32, 32};
	// V2 position = {};

};

struct Sprite{
	Rect clipping_box {};
	SpriteInfo info;

};

void render_sprite(Renderer *renderer, Sprite *spr, V2 position);
// void render_sprite_with_transparency(Renderer *renderer, Sprite *spr, V2 position, )
void render_sprite_as_background(Renderer *renderer, Sprite *spr);


struct AnimatedSprite{
	std::vector<Rect> clipping_boxes;
	int current_frame = 0;
	Timer timer;
	SpriteInfo info;
};

void add_animation_frame(AnimatedSprite *animation, Rect clipping_box);
void render_animation(Renderer *renderer, AnimatedSprite *anim_spr, V2 position);


#endif