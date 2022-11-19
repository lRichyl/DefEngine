#include "gui.h"
#include "collision.h"
#include "input.h"
#include "game.h"

namespace def {
	void init_button(Button *button, const char *text){
		button->is_pressed = false;
		button->text  = text;
	}

	void set_button_position(Button *button, V2 position){
		button->bounding_box.x = position.x;
		button->bounding_box.y = position.y;
	}

	void set_button_size(Button *button, V2 size){
		button->bounding_box.w = size.x;
		button->bounding_box.h = size.y;
		button->sprite.info.size = {size.x, size.y};
	}

	void update_button(Button *button){
		MouseInfo mouse = Game::mouse;
		if(DoRectContainsPoint(button->bounding_box, mouse.position)){
			if(mouse.left.state == MouseButtonState::MOUSE_PRESSED){
				button->sprite.info.tint = {100, 100, 100};
				button->is_pressed = true;
				return;
			}
		}

		
		// The button value only remains true for one frame because the mouse state can't return to the PRESSED state until you release it.
		button->sprite.info.tint = {255, 255, 255};
		button->is_pressed = false;
	}

	void render_button(Button *button, Renderer *renderer){
		V2 position = {button->bounding_box.x, button->bounding_box.y};
		render_queue_sprite(get_render_list_for_layer(LEVEL_LAYERS - 1), renderer, &button->sprite, position, get_shader_ptr(&Game::asset_manager, "gui_shader"));
	}

	void set_button_sprite(Button *button, Texture texture, Rect clip_region){
		button->sprite.info.texture = texture;
		button->sprite.clipping_box = clip_region;
	}
}