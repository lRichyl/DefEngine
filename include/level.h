#pragma once
#include "entity.h"
#include "renderer.h"

#define LEVEL_WIDTH 100
#define LEVEL_HEIGHT 100
#define LEVEL_LAYERS 5
#define TILE_SIZE 32

struct EntitySpecifier{
	EntityType type = EntityType::ENTITY_NONE;
	// The id is only used if the type is ENTITY_TILE. It is used to get the correct tile from a list.
	int id = -1; 
	int tile_id = -1;
	V2 area;
};

struct Level{
	char *name;
	EntitySpecifier *map_layers[LEVEL_LAYERS];
	DefArray<Collider> collision_regions;
};

void init_level(Level *level);
void load_entities_to_level(Level *level, EntityManager *em);
void save_collision_regions_to_level(Level *level, EntityManager *em);

