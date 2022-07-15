#include "level_editor.h"
#include "game.h"
#include "camera.h"
#include "utilities.h"
#include "collision.h"
#include "memory_arena.h"

#include <cmath>

static V2 get_tile(V2 mouse_pos){
	return V2{mouse_pos.x / TILE_SIZE, ceil(mouse_pos.y / TILE_SIZE)};
}

static int V2_coords_to_array_index(V2 tile_coords){
	return tile_coords.y * LEVEL_WIDTH + tile_coords.x;
}

static bool are_coords_inside_level(V2 coords){
	 return (coords.x < LEVEL_WIDTH * TILE_SIZE && coords.x >= 0 && coords.y <= LEVEL_HEIGHT * TILE_SIZE && coords.y > 0);
}

static bool is_mouse_on_collision_region(EntityManager *em, V2 mouse_pos, int *index = NULL){
	for(int i = 0; i < ENTITIES_PER_TYPE; i++){
		if(!em->collision_regions[i].is_active) continue;
		Rect col_reg = em->collision_regions[i].bounding_box;
		if(DoRectContainsPoint(col_reg, mouse_pos)){
			if(index) *index = i;
			return true;
		} 
	}
	return false;
}

template<typename T>
static void add_entity_prototype(EntityManager *em, EntityType type){
	T *e = allocate_from_arena<T>(&Game::main_arena);
	init_entity(e);
	em->entities_prototypes[type] = e;
}

static void add_tile_prototype(EntityManager *em, Texture texture, Rect clip_region){
	Tile *tile = allocate_from_arena<Tile>(&Game::main_arena);
	set_tile_sprite(tile, texture, clip_region);
	init_entity(tile);
	tile->id = em->tiles_prototypes.size;
	add_array(&em->tiles_prototypes, tile);
}

static void init_prototypes(EntityManager *em){
	em->entities_prototypes.size = em->entities_prototypes.capacity; // HACK.
	// When we add an entity to the level we copy from this entities prototypes.
	add_entity_prototype<Collider>(em, EntityType::ENTITY_COLLIDER);
	add_entity_prototype<Player>(em, EntityType::ENTITY_PLAYER);
	add_entity_prototype<Slime>(em, EntityType::ENTITY_SLIME);

	add_tile_prototype(em, get_texture(&Game::asset_manager, "test_tiles"), {0,0,32,32});
	add_tile_prototype(em, get_texture(&Game::asset_manager, "test_tiles"), {32,0,32,32});
}

// We will use this function to update the position of the entity prototypes which are shown in the entity selector. In the future we may add the ability
// to scroll so we will need to call this function when using the scrollwheel. AT THE MOMENT WE ONLY CALL IT AT INITIALIZATION.
static void update_entity_prototypes_positions(EntitySelector *selector){
	int j = 0;
	int k = 0;
	for(int i = EntityType::ENTITY_COLLIDER; i < ENTITY_AMOUNT; i++){
		Entity *e = get_entity_prototype(&Game::em, (EntityType)i);
		if(!e) continue;
		// if(e->type == EntityType::ENTITY_NONE || e->type == EntityType::ENTITY_TILE) continue;
		e->position.x = selector->area.x + selector->entity_area_offset.x + (j * TILE_SIZE);
		e->position.y = selector->area.y - selector->entity_area_offset.y - (k * TILE_SIZE);
		update_bounding_box(e);

		j++;
		if(j == 5) k++;
	}
}

static void render_entity_prototypes(LevelEditor *editor, Renderer *renderer){
	for(int i = EntityType::ENTITY_COLLIDER; i < ENTITY_AMOUNT; i++){
		Entity *e = get_entity_prototype(&Game::em, (EntityType)i);
		if(!e) continue;
		// if(e->type == EntityType::ENTITY_NONE || e->type == EntityType::ENTITY_TILE) continue;
		render_queue_sprite(get_render_list_for_layer(LEVEL_LAYERS - 1), renderer, &e->icon, e->position, get_shader_ptr(&Game::asset_manager, "gui_shader"));
	}
}

static void update_tile_prototypes_positions(EntitySelector *selector){
	int j = 0;
	int k = 0;
	for(int i = 0; i < Game::em.tiles_prototypes.size; i++){
		Tile *e = Game::em.tiles_prototypes[i];
		assert(e);
		// if(e->type == EntityType::ENTITY_NONE || e->type == EntityType::ENTITY_TILE) continue;
		e->position.x = selector->area.x + selector->entity_area_offset.x + (j * TILE_SIZE);
		e->position.y = selector->area.y - selector->entity_area_offset.y - (k * TILE_SIZE);
		update_bounding_box(e);

		j++;
		if(j == 5) k++;
	}
}

static void render_tiles_prototypes(LevelEditor *editor, Renderer *renderer){
	for(int i = 0; i < Game::em.tiles_prototypes.size; i++){
		Tile *e = Game::em.tiles_prototypes[i];
		assert(e);
		// if(e->type == EntityType::ENTITY_NONE || e->type == EntityType::ENTITY_TILE) continue;
		render_queue_sprite(get_render_list_for_layer(LEVEL_LAYERS - 1), renderer, &e->icon, e->position, get_shader_ptr(&Game::asset_manager, "gui_shader"));
	}
}


void init_entity_selector(EntitySelector *e_selector, Window *window){
	e_selector->entity_area_offset = {15, 64};
	float width = e_selector->entities_per_row * TILE_SIZE + (e_selector->entity_area_offset.x * 2);
	e_selector->area = {window->internalWidth - width, window->internalHeight, width, window->internalHeight};
}

void update_entity_selector(EntitySelector *e_selector, LevelEditor *editor){
	// If we click on an entity in the entity selector we set the selected entity type to the one we clicked on.
	MouseInfo mouse = Game::mouse;
	switch(e_selector->tab){
		case SELECTOR_TILES:{
			if(mouse.left.state == MouseButtonState::MOUSE_PRESSED){
				for(int i = 0; i < Game::em.tiles_prototypes.size; i++){
					Tile *e = Game::em.tiles_prototypes[i];
					assert(e);
					if(DoRectContainsPoint(e->bounding_box, mouse.position)){
						editor->selected_entity.type = e->type;
						editor->selected_entity.tile_id   = e->id;
					}
				}
			}

			break;
		}
		case SELECTOR_ENTITIES:{
			if(mouse.left.state == MouseButtonState::MOUSE_PRESSED){
				for(int i = EntityType::ENTITY_COLLIDER; i < ENTITY_AMOUNT; i++){
					Entity *e = get_entity_prototype(&Game::em, (EntityType)i);
					if(!e) continue;
					if(DoRectContainsPoint(e->bounding_box, mouse.position)){
						editor->selected_entity.type = e->type;
						editor->selected_entity.id   = e->id;
					}
				}
			}
			break;
		}
	}
	
}

void render_entity_selector(EntitySelector *e_selector, LevelEditor *editor, Renderer *renderer){
	render_queue_colored_rect(get_render_list_for_layer(LEVEL_LAYERS - 1), renderer, &e_selector->area, V3{0,0,0}, 255, get_shader_ptr(&Game::asset_manager, "gui_shader"));
	switch(e_selector->tab){
		case SELECTOR_TILES:{
			render_tiles_prototypes(editor, renderer);
			break;
		}
		case SELECTOR_ENTITIES:{
			render_entity_prototypes(editor, renderer);
			break;
		}
	}
	
}

void init_level_editor(LevelEditor *editor, Window *window){
	init_level(&editor->edited_level);
	init_entity_selector(&editor->entity_selector, window); 
	editor->work_area = {0 , LEVEL_HEIGHT * TILE_SIZE, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE};
	editor->state = EditorState::EDITOR_EDIT;

	// Hardcoded selected_entity. @TODO: DELETE THIS when the entity selection is implemented.
	editor->selected_entity.type = EntityType::ENTITY_PLAYER;
	init_prototypes(&Game::em);
	update_entity_prototypes_positions(&editor->entity_selector);
	update_tile_prototypes_positions(&editor->entity_selector);

	init_button(&editor->button, "TEST");
	set_button_position(&editor->button, {100,100});
	set_button_size(&editor->button, {32,32});
	editor->button.sprite.info.texture = get_texture(&Game::asset_manager, "test_tiles");
	editor->button.sprite.clipping_box = {0,0,32,32};

}


void update_level_editor(LevelEditor *editor, Renderer *renderer){
	if(editor->state == EditorState::EDITOR_EDIT){
		
		if(was_key_pressed(GLFW_KEY_SPACE)){
			editor->state = EditorState::EDITOR_TEST;
			save_collision_regions_to_level(&editor->edited_level, &Game::em);
			return;
		}

		if(was_key_pressed(GLFW_KEY_T) && editor->is_phase_one_collider_placement){
			editor->is_entity_selector_opened = !editor->is_entity_selector_opened;
		}

		// Change the selected layer.
		if(was_key_pressed(GLFW_KEY_E)){
			int layer = editor->current_layer;
			layer++;
			if(layer >= LEVEL_LAYERS) layer = 0;
			editor->current_layer = layer;
		}
		else if(was_key_pressed(GLFW_KEY_Q)){
			int layer = editor->current_layer;
			layer--;
			if(layer < 0) layer = LEVEL_LAYERS - 1;
			editor->current_layer = layer;
		}

		MouseInfo mouse = Game::mouse;	
		if(!editor->is_entity_selector_opened){
			V2 world_pos         = get_world_position(mouse.position);
			V2 over_tile         = get_tile(world_pos);
			V2 floored_tile_pos  = {(int)over_tile.x * TILE_SIZE, (int)over_tile.y * TILE_SIZE};
			
			if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && are_coords_inside_level(floored_tile_pos)){
				int index = V2_coords_to_array_index(over_tile);
				EntitySpecifier *e_spec_layer = editor->edited_level.map_layers[editor->current_layer];
				EntitySpecifier *e_spec       = &e_spec_layer[index];
				// printf("%d\n", e_spec->type);
				assert(e_spec);

				if(e_spec->type == EntityType::ENTITY_NONE){
					if(editor->selected_entity.type == EntityType::ENTITY_PLAYER){
						// Because the player is always present in memory and there can only be one, we must set the previous location of the player
						// to ENTITY_NONE.
						if(Game::em.player.layer > -1){
							EntitySpecifier *previous_player_layer = editor->edited_level.map_layers[Game::em.player.layer];
							
							V2 player_tile_pos = get_tile(Game::em.player.position);
							int index = V2_coords_to_array_index(player_tile_pos);
							previous_player_layer[index].type = EntityType::ENTITY_NONE;						
						}
					}
					else if(editor->selected_entity.type == EntityType::ENTITY_TILE){
						TileSpecifier *e = (TileSpecifier*)add_entity(editor->selected_entity.type, &Game::em, floored_tile_pos, editor->current_layer);
						e->tile_id   = editor->selected_entity.tile_id;
						// e->tile      = Game::em.tiles_prototypes[e->tile_id];
						e_spec->type = editor->selected_entity.type;
						e_spec->id   = e->id;
						e_spec->tile_id = e->tile_id;
						return;
					}
					// If the selected entity is not a collider, a tile or the player it just gets added.
					if((editor->selected_entity.type != EntityType::ENTITY_COLLIDER) && editor->selected_entity.type != EntityType::ENTITY_TILE){
						Entity *e = add_entity(editor->selected_entity.type, &Game::em, floored_tile_pos, editor->current_layer);
						e_spec->type = editor->selected_entity.type;
						e_spec->id   = e->id;
						
					}
					
				}
				if(editor->selected_entity.type == EntityType::ENTITY_COLLIDER){
						// static bool is_phase_one = true;
						static V2 start_pos;
						 
					if(editor->is_phase_one_collider_placement){
						if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && !is_mouse_on_collision_region(&Game::em, world_pos)){
							start_pos    = floored_tile_pos;
							editor->is_phase_one_collider_placement = false;
						}
					}
					else{
						// This code determines the corner positions of the collision region depending on where the second click is pressed.
						Rect collision_region;
						if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && !is_mouse_on_collision_region(&Game::em, world_pos)){
							if(start_pos.x == floored_tile_pos.x && start_pos.y == floored_tile_pos.y){
								V2 final_pos     = {floored_tile_pos.x + TILE_SIZE, floored_tile_pos.y - TILE_SIZE};
								collision_region = {start_pos.x, start_pos.y, std::abs(final_pos.x - start_pos.x), std::abs(final_pos.y - start_pos.y)};
							}
							else if (start_pos.x <= floored_tile_pos.x && start_pos.y >= floored_tile_pos.y){
								V2 final_pos     = {floored_tile_pos.x + TILE_SIZE, floored_tile_pos.y - TILE_SIZE};
								collision_region = {start_pos.x, start_pos.y, std::abs(final_pos.x - start_pos.x), std::abs(final_pos.y - start_pos.y)};
							}
							else if (start_pos.x <= floored_tile_pos.x && start_pos.y < floored_tile_pos.y){
								V2 bottom_left   = {start_pos.x, start_pos.y - TILE_SIZE};
								V2 top_right     = {floored_tile_pos.x + TILE_SIZE, floored_tile_pos.y};
								int width        = top_right.x - bottom_left.x;
								int height       = top_right.y - bottom_left.y;
								collision_region = {bottom_left.x, bottom_left.y + height, width, height};
							}
							else if (start_pos.x >= floored_tile_pos.x && start_pos.y >= floored_tile_pos.y){
								V2 bottom_left   = {floored_tile_pos.x, floored_tile_pos.y - TILE_SIZE};
								V2 top_right     = {start_pos.x + TILE_SIZE, start_pos.y};
								int width        = top_right.x - bottom_left.x;
								int height       = top_right.y - bottom_left.y;
								collision_region = {bottom_left.x, bottom_left.y + height, width, height};
							}
							else if (start_pos.x >= floored_tile_pos.x && start_pos.y < floored_tile_pos.y){
								V2 bottom_right  = {start_pos.x + TILE_SIZE, start_pos.y - TILE_SIZE};
								V2 top_left      = {floored_tile_pos.x, floored_tile_pos.y};
								int width        = bottom_right.x - top_left.x;
								int height       = top_left.y - bottom_right.y;
								collision_region = {top_left.x, top_left.y, width, height};
							}

							editor->is_phase_one_collider_placement = true;
							V2 position = {collision_region.x, collision_region.y};
							Collider *c = (Collider*)add_entity(editor->selected_entity.type, &Game::em, position, 2);
							c->bounding_box = collision_region;
							c->sprite.info.size = {collision_region.w, collision_region.h};
						}
					}
				}

			}
			else if(mouse.right.state == MouseButtonState::MOUSE_PRESSED && are_coords_inside_level(floored_tile_pos)){
				// To eliminate colliders the selected entity type must be ENTITY_COLLIDER.
				if(editor->selected_entity.type == EntityType::ENTITY_COLLIDER){
					int collision_region_index = -1;
					if(is_mouse_on_collision_region(&Game::em, world_pos, &collision_region_index)){
						assert(collision_region_index > -1);
						eliminate_entity(&Game::em, EntityType::ENTITY_COLLIDER, collision_region_index);
					}
				}
				else{
					int index = V2_coords_to_array_index(over_tile);
					EntitySpecifier *e_spec_layer = editor->edited_level.map_layers[editor->current_layer];
					EntitySpecifier *e_spec       = &e_spec_layer[index];
					eliminate_entity(&Game::em, e_spec->type, e_spec->id);
					e_spec->type = EntityType::ENTITY_NONE;
					e_spec->id = -1;
				}
			}
		}
		else{
			update_entity_selector(&editor->entity_selector, editor);
		}
		update_camera();
	}
	else if(editor->state == EditorState::EDITOR_TEST){
		update_button(&editor->button);
		if(was_key_pressed(GLFW_KEY_SPACE)){
			editor->state = EditorState::EDITOR_EDIT;
			clear_entity_manager(&Game::em);
			load_entities_to_level(&editor->edited_level, &Game::em);
			return;
		}
		update_entities(&Game::em, renderer);
	}
}

void render_level_editor(LevelEditor *editor, Renderer *renderer){
	// Render the background color of the level editor.
	render_queue_colored_rect(&Game::layers_render_commands[0], renderer, &editor->work_area, V3 {0 , 159, 255});

	render_button(&editor->button, renderer);

	// We always render the entities, no matter the state.
	render_entities(&Game::em, renderer);
	if(editor->state == EditorState::EDITOR_EDIT){
		render_colliders(&Game::em, renderer);
		char layer_string[] = "Layer: ";
		const char *number_string    = to_string(editor->current_layer);
		const char *complete_string  = strcat(layer_string, number_string);
		render_queue_text(&Game::layers_render_commands[LEVEL_LAYERS - 1], renderer, get_font(&Game::asset_manager, "default"), complete_string, {0,0}, {255,255,255}, false,  get_shader_ptr(&Game::asset_manager, "gui_shader"));

		if(editor->is_entity_selector_opened) 
			render_entity_selector(&editor->entity_selector, editor, renderer);
	}
	else if(editor->state == EditorState::EDITOR_TEST){

	}

	
	
}