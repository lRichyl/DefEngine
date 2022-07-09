#include "level.h"
#include "game.h"

void init_level(Level *level){
	for(int i = 0; i < LEVEL_LAYERS; i++){
		level->map_layers[i] = allocate_array_from_arena<EntitySpecifier>(&Game::main_arena, LEVEL_WIDTH * LEVEL_HEIGHT);
	}	
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
			}
		}
	}
}