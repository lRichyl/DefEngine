#pragma once
#include "math.h"
#include "texture.h"
#include "renderer.h"
#include "timer.h"

#include <vector>

// struct Renderer;
// struct ShaderProgram;
// struct RenderCommand;
// Clipping boxes values are based on the top left corner of the texture. 
// Position is the position of the top left corner of the sprite. The origin of world coordinates is on the
// bottom left corner of the screen.
namespace def{
	struct SpriteInfo{
		Texture texture;
		bool is_x_mirrored = false;
		bool is_y_mirrored = false;
		float alpha = 255;
		// int layer = 0;  // This is not used at the moment.
		V3 tint = {255,255,255};
		V2 size = {32, 32};
		// V2 position = {};

	};

	struct Sprite{
		Rect clipping_box {};
		SpriteInfo info;

	};

	void render_sprite(Renderer *renderer, Sprite *spr, V2 position, ShaderProgram *shader = NULL);
	void render_queue_sprite(DefArray<RenderCommand> *command_list, Renderer *renderer, Sprite *spr, V2 position, ShaderProgram *shader = NULL);
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
	void render_queue_animation(DefArray<RenderCommand> *command_list, Renderer *renderer, AnimatedSprite *anim_spr, V2 position);
}