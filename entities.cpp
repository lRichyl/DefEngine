#include "entity.h"
#include "game.h"
#include <cstdlib>
#include "def_math.h"
#include <algorithm>

#define RENDER_ENTITIES(type , list)\
	for(int i = 0; i < ENTITIES_PER_TYPE; i++){ type *e = &em->list[i]; if(e->is_active) render_entity(e, renderer);}\

#define UPDATE_ENTITIES(type , list)\
	for(int i = 0; i < ENTITIES_PER_TYPE; i++){ type *e = &em->list[i]; if(e->is_active) update_entity(e, renderer); update_bounding_box(e);}\

#define CLEAR_ENTITIES(list)\
	for(int  i = 0; i < ENTITIES_PER_TYPE; i++) {em->list[i].is_active = false; em->list[i].is_disabled = false;}\

namespace def {
	void init_entity_manager(EntityManager *em){
		// The entity and tile prototypes will be stored in this arrays.
		init_array(&em->tiles_prototypes, &Game::main_arena, MAX_TILE_PROTOTYPE);
		init_array(&em->entities_prototypes, &Game::main_arena, ENTITY_AMOUNT);
	}
	Entity* get_entity_prototype(EntityManager *em, EntityType type){
		Entity *entity = em->entities_prototypes[type];
		if(entity) return entity;
		else{
			printf("There's no prototype for that entity type\n");
			return NULL;
		}
	}

	void eliminate_entity(EntityManager *em, EntityType type ,int id){
		switch(type){
			case ENTITY_PLAYER:{
				em->player.is_on_level = false;
				em->player.is_active = false;
				break;
			}
			// We run the init_entity function outside the ADD_ENTITY macro for each entity because it may be different.
			case ENTITY_SLIME:   { ELIMINATE_ENTITY(slimes); break;}
			case ENTITY_TILE:    { ELIMINATE_ENTITY(tiles); break;}
			case ENTITY_COLLIDER:{ ELIMINATE_ENTITY(collision_regions); break;}
			case ENTITY_NONE:
				printf("Invalid entity type\n");
			default:
				printf("There's no entity of the type ID: %d\n", type);
				break;
		}
	}

	DefArray<RenderCommand>* get_render_list_for_layer(int layer){
		return &Game::layers_render_commands[layer];
	}

	void update_entities(EntityManager *em, Renderer *renderer){
		if(em->player.is_on_level){
			update_player(&em->player, renderer);
			update_bounding_box(&em->player);
		}
		UPDATE_ENTITIES(Slime , slimes);
		// UPDATE_ENTITIES(Collider, collision_regions)
	}

	void render_entities(EntityManager *em, Renderer *renderer){
		// This is just for testing. Entities should be drawn in order using the draw list.
		if(em->player.is_on_level)
			render_player(&em->player, renderer);
		RENDER_ENTITIES(Slime, slimes);
		for(int i = 0; i < MAX_TILES_PER_LEVEL; i++){ TileSpecifier *t = &em->tiles[i]; if(t->is_active) render_entity(t, renderer);}
	}

	void clear_entity_manager(EntityManager *em){
		// We use this function when we want to completely clear the entity manager, like when we want to
		// load a different level.
		em->player.is_on_level = false;
		CLEAR_ENTITIES(slimes);
		CLEAR_ENTITIES(tiles);
		for(int  i = 0; i < MAX_TILES_PER_LEVEL; i++) {em->tiles[i].is_active = false; em->tiles[i].is_disabled = false;}
		CLEAR_ENTITIES(collision_regions);
	}

	void render_colliders(EntityManager *em, Renderer *renderer){
		for(int i = 0; i < ENTITIES_PER_TYPE; i++){ 
			Collider *c = &em->collision_regions[i]; if(c->is_active) render_collider(c, renderer);
		}
	}


	void init_entity(Collider *collider){
		collider->sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
		collider->sprite.info.alpha     = 100;
		// collider->sprite.info.size      = {TILE_SIZE, TILE_SIZE};
		collider->sprite.clipping_box   = {128, 0, 32, 32}; 

		collider->icon = collider->sprite;
		
		collider->type = EntityType::ENTITY_COLLIDER;
		// collider->bounding_box = collider
		// collider->area_type = AreaType::AREA_SINGLE;
	}

	// void update_collider(Collider *collider, Renderer *renderer){
		
	// }

	void render_collider(Collider *collider, Renderer *renderer){
		render_queue_sprite(get_render_list_for_layer(collider->layer), renderer, &collider->sprite, collider->position);
	}

	void init_entity(Player *player){
		player->type = EntityType::ENTITY_PLAYER;
		// player->area_type = AreaType::AREA_SINGLE;
		// player->area = {1,1};
		player->sprite.info.texture = get_texture(&Game::asset_manager, "test_tiles");
		player->sprite.info.size      = {TILE_SIZE, TILE_SIZE};
		player->sprite.clipping_box   = {96, 0, 32, 32};

		player->icon = player->sprite;

		player->speed = 100;
		player->bounding_box.w = TILE_SIZE;
		player->bounding_box.h = TILE_SIZE;
		
		// Example of how to set the special placement function.
		// player->special_placement = [](){
			// printf("Player special placement\n");
		// };
	}

	void update_player(Player *player, Renderer *renderer){
		// printf("Player position %f, %f\n", player->position.x, player->position.y);
		if(is_key_being_pressed(renderer->window, GLFW_KEY_D)){
			player->direction.x = 1;
			// player->position.x += 50 * Game::dt;
		}
		else if(is_key_being_pressed(renderer->window, GLFW_KEY_A)){
			player->direction.x = -1;
			// player->position.x -= 50 * Game::dt;
		}
		else player->direction.x = 0;
		
		if(is_key_being_pressed(renderer->window, GLFW_KEY_D) && is_key_being_pressed(renderer->window, GLFW_KEY_A)) player->direction.x = 0;
		if(is_key_being_pressed(renderer->window, GLFW_KEY_W) && is_key_being_pressed(renderer->window, GLFW_KEY_S)) player->direction.y = 0;
		
		if(is_key_being_pressed(renderer->window, GLFW_KEY_W)){
			player->direction.y = 1;
			// player->position.y += 50 * Game::dt;
		}
		else if(is_key_being_pressed(renderer->window, GLFW_KEY_S)){
			player->direction.y = -1;
			// player->position.y -= 50 * Game::dt;
		}
		else player->direction.y = 0;
		
		player->position.x += player->direction.x * player->speed * Game::dt;
		player->position.y += player->direction.y * player->speed * Game::dt;
	}

	void render_player(Player *player, Renderer *renderer){
		render_queue_sprite(get_render_list_for_layer(player->layer), renderer, &player->sprite, player->position);
		// printf("%f,%f\n", player->bounding_box.w, player->bounding_box.h);
	}

	void init_entity(Slime *slime){
		slime->sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
		// slime->sprite.info.size      = {TILE_SIZE, TILE_SIZE};
		slime->sprite.clipping_box   = {64,0,32,32};
		slime->icon                  = slime->sprite;
		
		slime->type                  = EntityType::ENTITY_SLIME;

		slime->area                  = {2,2};
		slime->sprite.info.size      = {slime->area.x * TILE_SIZE, slime->area.y * TILE_SIZE};
		// slime->sprite = slime->icon.sprite;
	}

	void update_entity(Slime *slime, Renderer *renderer){
		// slime->bounding_box = Rect{slime->position.x, slime->position.y, slime->sprite.info.size.x, slime->sprite.info.size.y}; // Why was this done ? 
		// printf("%f, %f\n", slime->position.x, slime->position.y);
	}

	void render_entity(Slime *slime, Renderer *renderer){
		render_queue_sprite(get_render_list_for_layer(slime->layer), renderer, &slime->sprite, slime->position);
	}

	void init_entity(Multi *multi){
		multi->sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
		multi->sprite.info.size      = {64,64};
		multi->sprite.clipping_box   = {0,32,64,64};
		
		multi->icon           = multi->sprite;
		// multi->icon.info.size = icon_size;

		multi->type                  = EntityType::ENTITY_NONE;
		// multi->area_type             = AreaType::AREA_MULTI;
		// multi->area                  = {2,2};
	}
	void update_multi(Multi *multi, Renderer *renderer);
	void render_multi(Multi *multi, Renderer *renderer);

	void update_bounding_box(Entity *entity){
		// We need to update the bounding box position because it is use for collision detection and position is used for rendering the sprite at 
		// a certain location.
		entity->bounding_box.x = entity->position.x;
		entity->bounding_box.y = entity->position.y;
		entity->bounding_box.w = TILE_SIZE;
		entity->bounding_box.h = TILE_SIZE;
	}

	void init_entity(Tile *tile){
		tile->type = EntityType::ENTITY_TILE;
		tile->icon = tile->sprite;
	}

	void set_tile_sprite(Tile *tile, Texture texture, RECT clip_region){
		tile->sprite.info.texture = texture;
		tile->sprite.clipping_box = clip_region;
	}

	void init_entity(TileSpecifier *tile_e){
		tile_e->type = EntityType::ENTITY_TILE;
		// assert(tile_e->tile);
		// init_entity(tile_e->tile);
	}

	void render_entity(TileSpecifier *tile_e, Renderer *renderer){
		Tile *tile = Game::em.tiles_prototypes[tile_e->tile_id];
		render_queue_sprite(get_render_list_for_layer(tile_e->layer), renderer, &tile->sprite, tile_e->position);
	}

	Entity* add_entity(EntityType e_type, EntityManager *em, VEC_2D position, int layer){
		switch(e_type){
			case ENTITY_PLAYER:{
				em->player.is_on_level = true;
				em->player.position    = position;
				em->player.layer       = layer;
				return &em->player;
				break;
			}
			// We run the init_entity function outside the ADD_ENTITY macro for each entity because it may be different.
			case ENTITY_SLIME:   { ADD_ENTITY(slimes, Slime); break;}
			case ENTITY_COLLIDER:{ ADD_ENTITY(collision_regions, Collider); break;}
			case ENTITY_TILE:    { 
				TileSpecifier *e = NULL;
				int i = 0;
				for(i; i < MAX_TILES_PER_LEVEL; i++){ 
					if(!em->tiles[i].is_active){
						e = &em->tiles[i];
						e->id = i;
						break;
					} 
				}
				if (e == NULL) {
					char buf[] = "****";
					std::string err = "TileSpecifier 'e' was NULL, at 'i' : ";
					err += _itoa_s(i, buf, 10);
					throw(err);
				}
				init_entity(e);
				e->is_active = true;
				e->position = position;
				e->layer = layer;
			 	TileSpecifier *t = (TileSpecifier*)e;
			  	return t;}
			case ENTITY_NONE:
				printf("Invalid entity type\n");
			default:
				printf("There's no entity of the type ID: %d\n", e_type);
				break;
		}
	}
}
