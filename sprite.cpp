#include "sprite.h"
#include "renderer.h"

void render_sprite(Renderer *renderer, Sprite *spr, V2 position, ShaderProgram *shader){
	Rect bounding_box = {position.x, position.y, spr->info.size.x, spr->info.size.y};
	if(spr->clipping_box.x == 0 && spr->clipping_box.y == 0 && spr->clipping_box.w == 0 && spr->clipping_box.h == 0){
		if(!shader)
			render_quad(renderer, &bounding_box, &spr->info.texture, NULL,  spr->info.is_x_mirrored, spr->info.alpha, spr->info.tint, spr->info.is_y_mirrored);
		else
			render_quad_with_shader(renderer, &bounding_box, &spr->info.texture, *shader, NULL,  spr->info.is_x_mirrored, spr->info.alpha, spr->info.tint, spr->info.is_y_mirrored);
	}else{
		if(!shader)
			render_quad(renderer, &bounding_box, &spr->info.texture, &spr->clipping_box, spr->info.is_x_mirrored, spr->info.alpha, spr->info.tint, spr->info.is_y_mirrored);
		else
			render_quad_with_shader(renderer, &bounding_box, &spr->info.texture, *shader, &spr->clipping_box, spr->info.is_x_mirrored, spr->info.alpha, spr->info.tint, spr->info.is_y_mirrored);
	}
}

void render_sprite_as_background(Renderer *renderer, Sprite *spr){
	render_quad(renderer, NULL, &spr->info.texture, NULL, spr->info.is_x_mirrored, spr->info.alpha, spr->info.tint, spr->info.is_y_mirrored);
}

void add_animation_frame(AnimatedSprite *animation, Rect clipping_box){
	animation->clipping_boxes.push_back(clipping_box);
}

void render_animation(Renderer *renderer, AnimatedSprite *anim_spr, V2 position){
	anim_spr->timer.Tick();
	Rect bounding_box = {position.x, position.y, anim_spr->info.size.x, anim_spr->info.size.y};
	
	if(anim_spr->timer.isTimeReached){
		anim_spr->current_frame++;
		if(anim_spr->current_frame == anim_spr->clipping_boxes.size()){
			anim_spr->current_frame = 0;
		}
	}
	
	render_quad(renderer, &bounding_box, &anim_spr->info.texture, &anim_spr->clipping_boxes[anim_spr->current_frame],  anim_spr->info.is_x_mirrored, anim_spr->info.alpha, anim_spr->info.tint, anim_spr->info.is_y_mirrored);
	
	
}