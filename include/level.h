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
};

struct Level{
	EntitySpecifier *map_layers[LEVEL_LAYERS];
};

void init_level(Level *level);
void load_entities_to_level(Level *level, EntityManager *em);

