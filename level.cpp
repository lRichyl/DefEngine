#include "level.h"
#include "game.h"
#include "level_editor.h"

void init_level(Level *level){
	// level->current_layer = 0;
	for(int i = 0; i < LEVEL_LAYERS; i++){
		level->layers[i] = allocate_array_from_arena<MapObject>(&Game::main_arena, LEVEL_SIZE * LEVEL_SIZE);
	}
	
}

void init_level_entity_manager(Level *level, EntityManager *em){
	clear_entity_manager(em);
	MapObject *entities_layer = level->layers[2];
	for(int j = 0; j < LEVEL_SIZE; j++){
		for(int i = 0; i < LEVEL_SIZE; i++){
			V2 position = {i * TILE_SIZE, j * TILE_SIZE};
			int index = i * LEVEL_SIZE + j;
			EntitySelection *selection = &entities_layer[index].selected_entity;
			if(selection->entity_index == -1) continue;
			Entity *entity = selection->prototype_list->entities[selection->entity_index];
			
			switch(entity->type){
				case ENTITY_PLAYER:{
					Player *player = (Player*)entity;
					em->player = *player;
					em->player.position = position;
					em->player.is_on_level = true;
					break;
				}
				case ENTITY_SLIME: Slime new_slime = cast_and_position_entity<Slime>(entity, position); add_array(&em->slimes, new_slime); break;
				
			}
		}
	}
	// Collision regions can be used directly, as they are not dynamic entities like the background and foreground tiles and do not require initialization.
}

void update_level(Renderer *renderer, Level *level, EntityManager *em){
	update_entities(em, renderer);
}

void render_level(Renderer *renderer, Level *level, EntityManager *em){
	// We first render the first to layers(background).
	for(int k = 0; k < 2; k++){
		MapObject *layer = level->layers[k];
		render_tile_map(renderer, layer);
	}
	
	render_entities(em, renderer); 
	
	// Render the last two layers(foreground).
	for(int k = 3; k < 5; k++){
		MapObject *layer = level->layers[k];
		render_tile_map(renderer, layer);
	}
}

void render_collision_regions(Renderer *renderer, Level *level){
	static Texture col_reg_tex = get_texture(&Game::asset_manager, "test_tiles");
	static Rect    clip_reg    = {128,0,32,32};
	for(int i = 0; i < level->collision_regions.size; i++){
		Rect *col_reg = &level->collision_regions[i];
		// printf("Col regs: %f, %f, %f, %f\n", col_reg->x, col_reg->y, col_reg->w, col_reg->h);
		render_quad(renderer, col_reg, &col_reg_tex, &clip_reg, false, 100);
	}
}

