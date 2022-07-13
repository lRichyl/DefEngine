#pragma once
#include "level.h"
#include "window.h"
#include "entity.h"
#include "gui.h"

#define MAX_TILE_PROTOTYPE 100

enum SelectorTab{
	SELECTOR_TILES,
	SELECTOR_ENTITIES
};


struct EntitySelector{
	SelectorTab tab = SelectorTab::SELECTOR_TILES;
	int entities_per_row = 5;
	Rect area;
	V2 entity_area_offset;
};

void init_entity_selector(EntitySelector *e_selector, Window *window);
void render_entity_selector(EntitySelector *e_selector, LevelEditor *editor, Renderer *renderer);
void update_entity_selector(EntitySelector *e_selector, LevelEditor *editor);

enum EditorState{
	EDITOR_EDIT,
	EDITOR_TEST
};

struct LevelEditor{
	int current_layer = 0;
	bool is_entity_selector_opened = false;
	bool is_phase_one_collider_placement = true;
	EntitySpecifier selected_entity;
	Level edited_level;
	Rect work_area;
	EditorState state;

	EntitySelector entity_selector;

	Button button;
};

void init_level_editor(LevelEditor *editor, Window *window);
void update_level_editor(LevelEditor *editor, Renderer *renderer);
void render_level_editor(LevelEditor *editor, Renderer *renderer);