#pragma once
#include "gui.h"
#include "def_array.h"
#include "console.h"

const int TILES_AMOUNT          = 50;
const int ENTITIES_AMOUNT       = 50;
const int LEVEL_SIZE            = 30;
const int LEVEL_LAYERS          = 5;
const int TILE_SIZE             = 32;  
const int MAX_COLLISION_REGIONS = 100;

enum EditorState{
	EDITOR_EDIT,
	EDITOR_CONSOLE,
	EDITOR_TEST
};



struct MapObject{
	EntitySelection selected_entity;
	EntitySelection *origin = NULL;// This is used when we wanna place and delete multi tile entities.
	// int index = -1;
};

struct Level{
	const char *name;
	MapObject      *layers[LEVEL_LAYERS];
	DefArray<Rect> collision_regions;
	// Sprite collision_region_sprite;
};

void init_level_entity_manager(Level *level, EntityManager *em);
void update_level(Renderer *renderer, Level *level, EntityManager *em);
void render_level(Renderer *renderer, Level *level, EntityManager *em);
void render_collision_regions(Renderer *renderer, Level *level); // Collision regions are only drawn when in edit mode.

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