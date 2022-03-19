#include "gui.h"
#include "input.h"
#include "collision.h"
#include "asset_manager.h"
#include "game.h"


void init_frame(Frame *frame, Rect bounding_box, Texture texture, Rect frame_clipping_box){
	frame->bounding_box    = bounding_box;
	frame->work_area       = bounding_box;
	frame->sprite.info.texture  = texture;
	frame->sprite.info.size     = {frame->bounding_box.w, frame->bounding_box.h};
	frame->sprite.clipping_box = frame_clipping_box;
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
	
	for(int i = 0; i < frame->buttons.size(); i++){
		Button *button = &frame->buttons[i];
		button->relative_bounding_box.x = frame->work_area.x + button->origin_bounding_box.x;
		button->relative_bounding_box.y = frame->work_area.y - button->origin_bounding_box.y;
		// printf("%f, %f\n", buttonorigin_bounding_box.w, button.origin_bounding_box.h);

	}
	
}

static void render_button(Renderer* renderer, Frame *frame, Button *button){
	render_sprite(renderer, &button->sprite, {button->relative_bounding_box.x, button->relative_bounding_box.y});
}

static void render_entity_selector(Renderer *renderer, Frame *frame){
	DefArray<Entity*> *entities = &frame->e_selector->prototypes->entities;
	Rect *work_area             = &frame->work_area;
	V2 work_area_center         = {work_area->x + work_area->w / 2, work_area->y - frame->e_selector->padding.y};
	render_text(renderer, get_font(&Game::asset_manager, "prototype_list_font"), frame->e_selector->prototypes->name,work_area_center, V3{255,255,255}, true);
	for(int i = 0; i < entities->size; i++){
		Entity *e = array_at(entities, i);
		V2 position = {e->icon.bounding_box.x, e->icon.bounding_box.y};
		render_sprite(renderer, &e->icon.sprite, position);
	}
}

void render_frame(Renderer *renderer, Frame *frame){
	// If a sprite without a texture is passed to frame, a black rect is rendered.
	render_sprite(renderer, &frame->sprite, {frame->bounding_box.x, frame->bounding_box.y});
	
	// This is temporary, just to visualize the work_area.
	// render_colored_rect(renderer, &frame->work_area, V3{255,0,255});
	
	for(int i = 0; i < frame->buttons.size(); i++){
		render_button(renderer, frame, &frame->buttons[i]);
	}
	
	if(frame->e_selector){
		render_entity_selector(renderer, frame);
	}
}


// @SPEED: Right now we calculate the position of the icons every frame even if they don't move. This is done because we intend to add scrolling, if
// 		   it becomes necessary this will be moved to be only run when we scroll, but right now it's not a problem. 
static void update_entity_selector(Frame *frame, MouseInfo mouse){
	Rect *work_area            = &frame->work_area;
	EntitySelector *e_selector =  frame->e_selector;
	
	Rect entity_selector_area  = {work_area->x + e_selector->padding.x, work_area->y - e_selector->padding.y, work_area->w - (e_selector->padding.x * 2), work_area->h};
	
	DefArray<Entity*> *entities = &e_selector->prototypes->entities;
	// Al the entity prototypes are assigned the same icon size, so we retreive the size of the first element only.
	V2 icon_size = e_selector->icon_size;
	int icons_per_line = entity_selector_area.w / icon_size.x;
	
	int i = 0;
	for(int y = 0; y <= (entities->size / icons_per_line); y++){
		int y_pos = entity_selector_area.y - (y * icon_size.y) - e_selector->padding.y;
		for(int x = 0; x < icons_per_line; x++){
			if(i == entities->size) break;
			Entity *e = array_at(entities, i);
			V2 position = {entity_selector_area.x + (x * icon_size.x), y_pos};
			
			e->icon.bounding_box = {position.x, position.y, icon_size.x, icon_size.y};
			i++;
		}
	}
	
	// Check if the mouse is over one of the prototypes icons and set the selected entity if clicked.
	for(int i = 0; i < entities->size; i++){
		Entity *e = array_at(entities, i);
		
		if(DoRectContainsPoint(e->icon.bounding_box, mouse.position)){
			// We change the tint to indicate the selected icon.
			if(mouse.left.state == MouseButtonState::MOUSE_NONE){
				e->icon.sprite.info.tint = {200,200,200};
			}
			
			if(mouse.left.state == MouseButtonState::MOUSE_PRESSED){
				e_selector->selected_entity.entity_index = i;
				e_selector->selected_entity.prototype_list = e_selector->prototypes;
				// printf("Selected entity: %d\n", e_selector->selected_entity.entity_index);
				break;
			}
			
		}else{
			e->icon.sprite.info.tint = {255,255,255};
		}
	}
	
	
}

void update_frame(Frame *frame, Renderer *renderer){
	MouseInfo mouse = Game::mouse;
	// TODO: Set the value of every button to false.
	
	
	
	for(int i = 0; i < frame->buttons.size(); i++){
		Button *button = &frame->buttons[i];
		// button->sprite.info.tint = {255,255,255};
		if(DoRectContainsPoint(button->relative_bounding_box, mouse.position)){
			if(mouse.left.state == MouseButtonState::MOUSE_NONE){
				button->sprite.info.tint = button->selected_color;
			}else if(mouse.left.state == MouseButtonState::MOUSE_PRESSED){
				// TODO: Set the button boolean pointer to true.
				button->sprite.info.tint = button->clicked_color;
			}
		}else{
			button->sprite.info.tint = {255,255,255};
		}
		return;
	}
	
	// Update the entity selector component.
	if(frame->e_selector){
		update_entity_selector(frame, mouse);
	}
}

// The bounding_box x and y positions are offsets to the work area of the frame.
void add_button(Frame *frame, Rect bounding_box, const char* text, Texture texture, Rect clipping_box, V3 selected_color, V3 clicked_color){
	Button button;
	button.text = text;
	button.origin_bounding_box = bounding_box;
	button.relative_bounding_box = {frame->work_area.x + bounding_box.x, frame->work_area.y - bounding_box.y, bounding_box.w, bounding_box.h};
	button.selected_color = selected_color;
	button.clicked_color  = clicked_color;
	// button.bounding_box.y = frame->work_area.y - bounding_box.y;
	button.sprite.info.texture = texture;
	button.sprite.info.size    = {bounding_box.w, bounding_box.h};
	button.sprite.clipping_box = clipping_box;
	frame->buttons.push_back(button);
}

void init_tabbed_menu(TabbedMenu *tabbed_menu, Rect bounding_box, Texture tab_texture, Texture frame_texture, Font *font ,Rect tab_clipping_box, Rect frame_clipping_box){
	// These two are default values.
	tabbed_menu->tab_height     = 32;
	tabbed_menu->current_tab    = 0;
	tabbed_menu->last_tab_x_pos = bounding_box.x;
	tabbed_menu->number_of_tabs = 0;
	tabbed_menu->tabs           = create_def_table<Frame*>(&Game::main_arena, 20);

	tabbed_menu->tab_font       = font;
	
	tabbed_menu->bounding_box   = bounding_box;
	tabbed_menu->tab_texture    = tab_texture;
	tabbed_menu->frame_texture  = frame_texture;
	tabbed_menu->tab_clip_box   = tab_clipping_box;
	tabbed_menu->frame_clip_box = frame_clipping_box;
	
	init_array(&tabbed_menu->tabbed_frames, &Game::main_arena, 50);
	
	for(int i = 0; i < tabbed_menu->tabbed_frames.size; i++){
		Frame *frame = &tabbed_menu->tabbed_frames[i].frame;
		init_frame(frame, bounding_box, frame_texture, frame_clipping_box);
	}
}

void add_tab(TabbedMenu *tabbed_menu, const char *tab_text){
	TabbedFrame tabbed_frame;
	tabbed_frame.tab.bounding_box.w = get_text_width(tabbed_menu->tab_font, tab_text) + 8;
	tabbed_frame.tab.bounding_box.h = tabbed_menu->tab_height;
	tabbed_frame.tab.bounding_box.x = tabbed_menu->last_tab_x_pos;
	tabbed_frame.tab.bounding_box.y = tabbed_menu->bounding_box.y + tabbed_menu->tab_height;
	tabbed_frame.tab.sprite.info.texture = tabbed_menu->tab_texture;
	tabbed_frame.tab.sprite.info.size    = V2{tabbed_frame.tab.bounding_box.w, tabbed_frame.tab.bounding_box.h};
	tabbed_frame.tab_text = tab_text;

	init_frame(&tabbed_frame.frame, tabbed_menu->bounding_box, tabbed_menu->frame_texture, tabbed_menu->frame_clip_box);
	add_array(&tabbed_menu->tabbed_frames, tabbed_frame);
	// We add the tabs addresses to a def table to easily get a reference to a tab by name.
	insert_to_def_table(tabbed_menu->tabs, tab_text, &tabbed_menu->tabbed_frames[tabbed_menu->number_of_tabs].frame);
	tabbed_menu->number_of_tabs++;
	tabbed_menu->last_tab_x_pos += tabbed_frame.tab.bounding_box.w;
	printf("Added tab\n");
	
}

void update_tabbed_menu(Renderer *renderer, TabbedMenu *tabbed_menu){
	assert(tabbed_menu->tabbed_frames.size > 0);
	
	//Check if a tab is clicked on.
	MouseInfo mouse = Game::mouse;
	for(int i = 0; i < tabbed_menu->tabbed_frames.size; i++){
		Tab *tab= &tabbed_menu->tabbed_frames[i].tab;
		
		if(DoRectContainsPoint(tab->bounding_box, mouse.position)){
			if(mouse.left.state == MouseButtonState::MOUSE_PRESSED){
				tabbed_menu->current_tab = i;
			}
			break;
		}
	}
	
	// Update only the selected tab.
	Frame *frame = &tabbed_menu->tabbed_frames[tabbed_menu->current_tab].frame;
	update_frame(frame, renderer);
}

Frame* get_current_tab(TabbedMenu *menu){
	return &menu->tabbed_frames[menu->current_tab].frame;
}

void render_tabbed_menu(Renderer *renderer, TabbedMenu *tabbed_menu){
	// Render the tabs textures and their text.
	for(int i = 0; i < tabbed_menu->tabbed_frames.size; i++){
		Tab *tab = &tabbed_menu->tabbed_frames[i].tab;
		// render_quad(renderer, &tab->bounding_box, &tabbed_menu->tab_texture, &tabbed_menu->tab_clip_box, false, 255);
	
	
		// @SPEED: We should probably not do this every frame but every time we select a new tab.
		if(i == tabbed_menu->current_tab){
			// render_colored_rect(renderer, &tab->bounding_box, V3{0,0,255});
			tab->sprite.info.tint = {255,255,255};
		}else{
			tab->sprite.info.tint = {170,170,170};
			// render_colored_rect(renderer, &tab->bounding_box, V3{150,150,255});
		}
		render_sprite(renderer, &tab->sprite, {tab->bounding_box.x, tab->bounding_box.y});
		
		V2 text_position = {tab->bounding_box.x + tab->bounding_box.w / 2, tab->bounding_box.y - tabbed_menu->tab_height / 2};
		render_text(renderer, tabbed_menu->tab_font, tabbed_menu->tabbed_frames[i].tab_text, text_position, V3{255,255,255}, true);
	}
	
	// Render the current selected tab frame.
	assert(tabbed_menu->tabbed_frames.size > 0);
	Frame *frame = &tabbed_menu->tabbed_frames[tabbed_menu->current_tab].frame;
	render_frame(renderer, frame);
}




void add_entity_selector(Frame *frame, PrototypeList *prototype_list, V2 padding, V2 icon_size){
	assert(frame);
    frame->e_selector             = allocate_from_arena<EntitySelector>(&Game::main_arena);
	frame->e_selector->prototypes = prototype_list;
	frame->e_selector->padding    = padding;
	frame->e_selector->icon_size  = icon_size;
}