#include "entity.h"
#include "game.h"

void init_entity_manager(EntityManager *em){
	init_array(&em->entities_draw_list, &Game::main_arena, 1000);
	init_array(&em->collision_regions, &Game::main_arena, 100);
	init_array(&em->slimes, &Game::main_arena, 50);
}

void update_entities(EntityManager *em, Renderer *renderer){
	if(em->player.is_on_level)
		update_player(&em->player, renderer);
	for(int i = 0; i < em->slimes.size; i++) update_slime(&em->slimes[i], renderer);
}

void render_entities(EntityManager *em, Renderer *renderer){
	// This is just for testing. Entities should be drawn in order using the draw list.
	if(em->player.is_on_level)
		render_player(&em->player, renderer);
	for(int i = 0; i < em->slimes.size; i++) render_slime(&em->slimes[i], renderer);
}

Entity* get_selection_entity(EntitySelection *selection){
	if(selection->entity_index == -1) return NULL;
	Entity *entity = selection->prototype_list->entities[selection->entity_index];
	return entity;
}

void clear_entity_manager(EntityManager *em){
	// Do not clear the player and other entities that persist between levels.
	em->player.is_on_level = false;
	clear_array(&em->entities_draw_list);
	clear_array(&em->slimes);
}

void init_entity(Collider *collider, V2 icon_size){
	collider->icon.sprite = Sprite();
	collider->icon.sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
	collider->icon.sprite.info.alpha     = 100;
	collider->icon.sprite.info.size      = icon_size;
	collider->icon.sprite.clipping_box   = {128, 0, 32, 32}; 
	
	collider->type = EntityType::ENTITY_COLLIDER;
	collider->area_type = AreaType::AREA_SINGLE;
}

// void update_collider(Collider *collider, Renderer *renderer){
	
// }

// void render_collider(Collider *collider, Renderer *renderer){
	
// }

void init_entity(Player *player, V2 icon_size){
	// Initialize the player's icon sprite.
	player->icon.sprite = Sprite();
	player->icon.sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
	// player->icon.sprite.info.size      = {32,32};
	player->icon.sprite.clipping_box   = {96, 0, 32, 32};
	player->icon.sprite.info.size      = icon_size;
	
	player->type = EntityType::ENTITY_PLAYER;
	player->area_type = AreaType::AREA_SINGLE;
	player->area = {1,1};
	player->sprite.info.texture = get_texture(&Game::asset_manager, "test_tiles");
	player->sprite.info.size      = {32,32};
	player->sprite.clipping_box   = {96, 0, 32, 32};

	player->speed = 100;
	player->bounding_box.w = icon_size.x;
	player->bounding_box.h = icon_size.y;
	
	// Example of how to set the special placement function.
	// player->special_placement = [](){
		// printf("Player special placement\n");
	// };
}

void update_player(Player *player, Renderer *renderer){
	// printf("Player position %f, %f\n", player->position.x, player->position.y);
	if(IsKeyPressed(renderer->window, GLFW_KEY_D)){
		player->direction.x = 1;
		// player->position.x += 50 * Game::dt;
	}
	else if(IsKeyPressed(renderer->window, GLFW_KEY_A)){
		player->direction.x = -1;
		// player->position.x -= 50 * Game::dt;
	}
	else player->direction.x = 0;
	
	if(IsKeyPressed(renderer->window, GLFW_KEY_D) && IsKeyPressed(renderer->window, GLFW_KEY_A)) player->direction.x = 0;
	if(IsKeyPressed(renderer->window, GLFW_KEY_W) && IsKeyPressed(renderer->window, GLFW_KEY_S)) player->direction.y = 0;
	
	if(IsKeyPressed(renderer->window, GLFW_KEY_W)){
		player->direction.y = 1;
		// player->position.y += 50 * Game::dt;
	}
	else if(IsKeyPressed(renderer->window, GLFW_KEY_S)){
		player->direction.y = -1;
		// player->position.y -= 50 * Game::dt;
	}
	else player->direction.y = 0;
	
	player->position.x += player->direction.x * player->speed * Game::dt;
	player->position.y += player->direction.y * player->speed * Game::dt;
	
	// player->bounding_box = Rect{player->position.x, player->position.y, player->sprite.info.size.x, player->sprite.info.size.y};
	update_position(player);
}

void render_player(Player *player, Renderer *renderer){
	render_sprite(renderer, &player->sprite, player->position);
	// printf("%f,%f\n", player->bounding_box.w, player->bounding_box.h);
}

void init_entity(Slime *slime, V2 icon_size){
	slime->sprite = Sprite();
	slime->sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
	slime->sprite.info.size      = icon_size;
	slime->sprite.clipping_box   = {64,0,32,32};
	slime->icon.sprite           = slime->sprite;
	
	slime->type                  = EntityType::ENTITY_SLIME;
	slime->area_type             = AreaType::AREA_SINGLE;
	slime->area                  = {1,1};
	slime->sprite = slime->icon.sprite;
}

void update_slime(Slime *slime, Renderer *renderer){
	slime->bounding_box = Rect{slime->position.x, slime->position.y, slime->sprite.info.size.x, slime->sprite.info.size.y};
	// printf("%f, %f\n", slime->position.x, slime->position.y);
}

void render_slime(Slime *slime, Renderer *renderer){
	render_sprite(renderer, &slime->sprite, slime->position);
}

void init_entity(Multi *multi, V2 icon_size){
	multi->sprite = Sprite();
	multi->sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
	multi->sprite.info.size      = {64,64};
	multi->sprite.clipping_box   = {0,32,64,64};
	
	multi->icon.sprite           = multi->sprite;
	multi->icon.sprite.info.size = icon_size;

	multi->type                  = EntityType::ENTITY_NONE;
	multi->area_type             = AreaType::AREA_MULTI;
	multi->area                  = {2,2};
	multi->sprite = multi->icon.sprite;
}
void update_multi(Multi *multi, Renderer *renderer);
void render_multi(Multi *multi, Renderer *renderer);

