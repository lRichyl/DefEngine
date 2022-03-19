#pragma once
#include "math.h"
#include "renderer.h"
#include "sprite.h"
#include "text.h"
#include "def_table.h"
#include "entity.h"

#include <vector>

// struct PrototypeList;
// This button works as a latch so it return to its off value after pressing it. 
// Set the value of the button to false at the beginning of every frame. It will only remain true for one frame. So you should check its state
// after calling the update_frame() function.
struct Button{ // Latch
	const char *text; // Implement this.
	V3 selected_color;
	V3 clicked_color;
	// static V3 clicked_color;
	Rect origin_bounding_box;
	Rect relative_bounding_box;
	Sprite sprite;
};



struct EntitySelector{
	// int vertical_gap_for_text;
	// static constexpr int PROTOTYPE_LISTS = 1;
	EntitySelection selected_entity;
	V2 icon_size;
	// y component is for the space above the selector for the prototype list name. 
	// x component is for the space to the left and right of the entity selector.
	V2 padding; 
	PrototypeList *prototypes;
};


// The UI elements will be placed inside this frame.
struct Frame{
	float horizontal_padding;
	float vertical_padding;
	int work_area_layer;
	int gui_elements_layer;
	Rect bounding_box;
	Rect work_area;
	Sprite sprite;
	
	std::vector<Button> buttons;
	EntitySelector *e_selector = NULL;
};

struct Tab{
	Rect bounding_box;
	Sprite sprite;
};

struct TabbedFrame{
	Tab tab;
	Frame frame;
	const char * tab_text;
};

struct TabbedMenu{
	int current_tab;
	int last_tab_x_pos;
	int number_of_tabs;
	int tab_height;
	DefTable<Frame*> *tabs;
	DefArray<TabbedFrame> tabbed_frames;
	Font  *tab_font = NULL;
	Texture tab_texture;
	Texture frame_texture;
	Rect bounding_box;
	Rect tab_clip_box;
	Rect frame_clip_box;
};

void init_frame(Frame *frame, Rect bounding_box, Texture texture, Rect frame_clipping_box);
void update_frame(Frame *frame, Renderer *renderer);
void set_padding(Frame *frame, float hpad, float vpad);
void move_frame(Frame *frame, V2 new_pos);
void render_frame(Renderer *renderer, Frame *frame);


void add_button(Frame *frame, Rect bounding_box, const char* text, Texture texture, Rect clipping_box, V3 selected_color = {150,150,150}, V3 clicked_color = {100,100,100});
void add_entity_selector(Frame *frame, PrototypeList *prototype_list, V2 padding, V2 icon_size);

void init_tabbed_menu(TabbedMenu *tabbed_menu, Rect bounding_box, Texture tab_texture, Texture frame_texture, Font *font, Rect tab_clipping_box, Rect frame_clipping_box);
Frame* get_current_tab(TabbedMenu *menu);
void add_tab(TabbedMenu *tabbed_menu, const char *tab_text);
void render_tabbed_menu(Renderer *renderer, TabbedMenu *tabbed_menu);
void update_tabbed_menu(Renderer *renderer, TabbedMenu *tabbed_menu);
