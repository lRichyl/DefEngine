#pragma once
#include "gui.h"
#include "def_array.h"
#include "console.h"
#include "level.h"

const int TILES_AMOUNT          = 50;
const int ENTITIES_AMOUNT       = 50;
const int TILE_SIZE             = 32;  
const int MAX_COLLISION_REGIONS = 100;

enum EditorState{
	EDITOR_EDIT,
	EDITOR_CONSOLE,
	EDITOR_TEST
};


struct PrototypeList{
	const char *name;
	DefArray<Entity*> entities;
};

void init_prototype_list(PrototypeList *list, int size, const char *name);
void add_prototype(PrototypeList *list, Entity *e);

struct LevelEditor{
	EditorState state = EditorState::EDITOR_EDIT;
	EntitySelection selected_entity;
	TabbedMenu menu;
	V2 icon_size;
	PrototypeList tiles;
	PrototypeList entities;
	PrototypeList multi_tiles;
	PrototypeList multi_entities;
	Level current_level;
	// Console console;
	bool show_entity_selector;
	bool test_level;
	// bool show_console;
	int current_layer;
};

void init_level_editor(LevelEditor *editor, Rect bounding_box, Texture frame_texture, Texture tab_texture, Renderer *renderer);
void load_prototype_lists(LevelEditor *editor); // Call this after adding all the tabs.
void set_level_editor_tab_font(LevelEditor *editor, Font *font);
void update_level_editor(Renderer *renderer, LevelEditor *editor);
void render_level_editor(Renderer *renderer, LevelEditor *editor);
void render_tile_map(Renderer *renderer, MapObject *tile_map);

bool save_level(LevelEditor *editor, const char *name);
bool save_new_level(LevelEditor *editor, const char *name);