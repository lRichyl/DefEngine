#include "gui.h"

void init_frame(Frame *frame, Rect bounding_box, Texture texture){
	frame->bounding_box    = bounding_box;
	frame->work_area       = bounding_box;
	// frame->work_area_layer = 1;
	frame->sprite.info.texture  = texture;
	frame->sprite.info.size     = {frame->bounding_box.w, frame->bounding_box.h};
	// frame->sprite.info.layer    = 2;
	// frame->gui_elements_layer   = 0;
}

void set_padding(Frame *frame, float hpad, float vpad){
	Rect *bbox = &frame->bounding_box;
	frame->horizontal_padding = hpad;
	frame->vertical_padding = vpad;
	frame->work_area.x += hpad;
	frame->work_area.y -= vpad;
	frame->work_area.w -= hpad * 2;
	frame->work_area.h -= vpad * 2;
}

void move_frame(Frame *frame, V2 new_pos){
	frame->bounding_box.x = new_pos.x;
	frame->bounding_box.y = new_pos.y;
	frame->work_area.x    = frame->bounding_box.x + frame->horizontal_padding;
	frame->work_area.y    = frame->bounding_box.y - frame->vertical_padding;
}

static void render_button(Renderer* renderer, Frame *frame, Button *button){
	render_sprite(renderer, &button->sprite, {frame->work_area.x + button->bounding_box.x, frame->work_area.y - button->bounding_box.y});
}

void render_frame(Renderer *renderer, Frame *frame){
	// If a sprite without a texture is passed to frame, a black rect is rendered.
	render_sprite(renderer, &frame->sprite, {frame->bounding_box.x, frame->bounding_box.y});
	
	// This is temporary, just to visualize the work_area.
	render_colored_rect(renderer, &frame->work_area, V3{255,0,255});
	
	for(int i = 0; i < frame->buttons.size(); i++){
		render_button(renderer, frame, &frame->buttons[i]);
	}
}

// The bounding_box x and y positions are based around the work area of the frame.
void add_button(Frame *frame, Rect bounding_box, const char* text, Texture texture, Rect clipping_box){
	Button button;
	button.text = text;
	button.bounding_box = bounding_box;
	// button.bounding_box.y = frame->work_area.y - bounding_box.y;
	button.sprite.info.texture = texture;
	button.sprite.info.size    = {bounding_box.w, bounding_box.h};
	button.sprite.clipping_box = clipping_box;
	frame->buttons.push_back(button);
}
