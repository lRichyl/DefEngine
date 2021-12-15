#include "sprite.h"

void render_sprite(Renderer *renderer, Sprite *spr, V2 position){
	Rect bounding_box = {position.x, position.y, spr->size.x, spr->size.y};
	if(spr->clipping_box.x == 0 && spr->clipping_box.y == 0 && spr->clipping_box.w == 0 && spr->clipping_box.h == 0){
		render_quad(renderer, &bounding_box, &spr->texture, spr->layer, NULL,  spr->is_x_mirrored, spr->alpha, spr->tint, spr->is_y_mirrored);
		
	}else{
		render_quad(renderer, &bounding_box, &spr->texture, spr->layer, &spr->clipping_box, spr->is_x_mirrored, spr->alpha, spr->tint, spr->is_y_mirrored);
		
	}
}

void render_sprite_as_background(Renderer *renderer, Sprite *spr){
	render_quad(renderer, NULL, &spr->texture, spr->layer, NULL, spr->is_x_mirrored, spr->alpha, spr->tint, spr->is_y_mirrored);
}