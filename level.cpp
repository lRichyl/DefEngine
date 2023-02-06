#include "level.h"
#include "game.h"

void init_level(Level *level){
	for(int i = 0; i < LEVEL_LAYERS; i++){
		level->map_layers[i] = allocate_array_from_arena<EntitySpecifier>(&Game::main_arena, LEVEL_WIDTH * LEVEL_HEIGHT);
	}	
	init_array(&level->collision_regions, &Game::main_arena, ENTITIES_PER_TYPE);
}

void load_entities_to_level(Level *level, EntityManager *em){
	for(int i = 0; i < LEVEL_LAYERS; i++){
		for(int j = 0; j < LEVEL_WIDTH * LEVEL_HEIGHT; j++){
			int tile_x = j % LEVEL_WIDTH;
			int tile_y = j / LEVEL_WIDTH;
			float x_pixel_pos = tile_x * TILE_SIZE;
			float y_pixel_pos = tile_y * TILE_SIZE;
			V2 position = {x_pixel_pos,y_pixel_pos};
			EntitySpecifier *e_spec_layer = level->map_layers[i];
			EntitySpecifier *e_spec        = &e_spec_layer[j];
			if(e_spec->type != EntityType::ENTITY_NONE){
				Entity *e = add_entity(e_spec->type, em, position, i);
				e_spec->id = e->id;
				if(e_spec->type == EntityType::ENTITY_TILE){
					TileSpecifier *t = (TileSpecifier*)e;
					t->tile_id = e_spec->tile_id;
				}		
			}
		}
	}

	
	for(int i = 0; i < level->collision_regions.size; i++){
		Collider collider  = level->collision_regions[i];
		Collider *added = (Collider*)add_entity(EntityType::ENTITY_COLLIDER, em, collider.position, i);
		*added = collider;
	}
}

void save_collision_regions_to_level(Level *level, EntityManager *em){
	clear_array(&level->collision_regions);
	for(int i = 0; i < ENTITIES_PER_TYPE; i++){
		Collider c = em->collision_regions[i];
		if(!c.is_active) continue;
		add_array(&level->collision_regions, c);
	}
}

void empty_level(Level *level){
	level->name[0] = '\0';
	EntitySpecifier e_spec; // Default entity specifier.
	for(int i = 0; i < LEVEL_LAYERS; i++){
		for(int j = 0; j < LEVEL_WIDTH * LEVEL_HEIGHT; j++){
			EntitySpecifier * layer = level->map_layers[i];
			layer[j] = e_spec;
		}
	}
	clear_array(&level->collision_regions);
	clear_entity_manager(&Game::em);
}