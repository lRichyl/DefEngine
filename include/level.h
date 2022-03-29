#pragma once
#include "def_array.h"
#include "entity.h"
#include "renderer.h"

const int LEVEL_SIZE      = 30;
const int LEVEL_LAYERS    = 5;
const int LEVEL_NAME_SIZE = 50;


struct MapObject{
	EntitySelection selected_entity;
	EntitySelection *origin = NULL;// This is used when we wanna place and delete multi tile entities.
	int origin_index = -1;
};

struct Level{
	char name[LEVEL_NAME_SIZE]{};
	MapObject      *layers[LEVEL_LAYERS];
	DefArray<Rect> collision_regions;
	// Sprite collision_region_sprite;
};

void init_level(Level *level);
void init_level_entity_manager(Level *level, EntityManager *em);
void update_level(Renderer *renderer, Level *level, EntityManager *em);
void render_level(Renderer *renderer, Level *level, EntityManager *em);
void render_collision_regions(Renderer *renderer, Level *level); // Collision regions are only drawn when in edit mode.
void clear_level_memory(Level *level); // This function deallocate all the level's memory.
void empty_level(Level *level); // This function resets the level memory to its default state.


