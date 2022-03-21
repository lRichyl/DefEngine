#include "game.h"
#include "level_editor.h"
#include "asset_manager.h"
#include "entity.h"
#include "input.h"
#include "utilities.h"
#include <cmath>

static void add_tile_prototype(LevelEditor *editor, Texture texture, Rect clipping_box){
	Tile *tile                = allocate_from_arena<Tile>(&Game::main_arena);
	tile->sprite = Sprite();
	tile->sprite.clipping_box = clipping_box;
	tile->sprite.info.size    = editor->icon_size;
	tile->sprite.info.texture = texture;
	tile->icon.sprite         = tile->sprite;
	
	add_prototype(&editor->tiles, tile);
}


static void add_entity_prototype(LevelEditor *editor, Entity *entity){
	add_prototype(&editor->entities, entity);
}

static void init_level(Level *level){
	// level->current_layer = 0;
	for(int i = 0; i < LEVEL_LAYERS; i++){
		level->layers[i] = allocate_array_from_arena<MapObject>(&Game::main_arena, LEVEL_SIZE * LEVEL_SIZE);
	}
	
	// level->collision_region_sprite.info.texture   = get_texture(&Game::asset_manager, "test_tiles");
	// level->collision_region_sprite.info.alpha     = 100;
	// level->collision_region_sprite.info.size      = icon_size;
	// level->collision_region_sprite.clipping_box   = {128, 0, 32, 32}; 
	
	// for(int i = 0; i < LEVEL_LAYERS; i++){
		// MapObject *level = level->layers[i];
		// for(int j = 0; j < LEVEL_SIZE*LEVEL_SIZE; j++){
			// level[j].entity_index = -1;
		// }
	// }
}

void init_level_editor(LevelEditor *editor, Rect bounding_box, Texture frame_texture, Texture tab_texture){
	init_tabbed_menu(&editor->menu, bounding_box, tab_texture, frame_texture, get_font(&Game::asset_manager, "tabs_font"), {0,0,0}, {0,0,0});
	init_prototype_list(&editor->tiles, TILES_AMOUNT, "Tiles");
	init_prototype_list(&editor->entities, ENTITIES_AMOUNT, "Entities");
	init_prototype_list(&editor->multi_tiles, ENTITIES_AMOUNT, "Multi-tile");
	init_prototype_list(&editor->multi_entities, ENTITIES_AMOUNT, "Multi-entities");
	
	// TODO: This should be done when creating a new level.
	init_array(&editor->current_level.collision_regions, &Game::main_arena, MAX_COLLISION_REGIONS);
	init_level(&editor->current_level);
	
	editor->icon_size = V2 {32,32}; // At the moment this can only be set during initialization.
	editor->show_entity_selector = false;
	editor->current_layer = 0;
	editor->test_level = false;
	
	// Add tile prototypes.
	add_tile_prototype(editor, get_texture(&Game::asset_manager, "test_tiles"), {0,0,32,32});
	add_tile_prototype(editor, get_texture(&Game::asset_manager, "test_tiles"), {32,0,32,32});
	
	// Add entities prototypes.
	Slime    *slime    = allocate_from_arena<Slime>   (&Game::main_arena); init_slime   (slime,    editor->icon_size); add_entity_prototype(editor, slime);
	Player   *player   = allocate_from_arena<Player>  (&Game::main_arena); init_player  (player,   editor->icon_size); add_entity_prototype(editor, player);
	Collider *collider = allocate_from_arena<Collider>(&Game::main_arena); init_collider(collider, editor->icon_size); add_entity_prototype(editor, collider);
	
}

static Frame* get_tab_frame(DefTable<Frame*> *frame_table, const char *tab_name){
	Frame *frame = NULL;
	frame = get_from_def_table(frame_table, tab_name);
	assert(frame);
	return frame;
}

static void add_entity_selector_to_editor_tab(LevelEditor *editor, const char *tab_name, PrototypeList *list, V2 padding){
	Frame* frame = get_tab_frame(editor->menu.tabs, tab_name);
	add_entity_selector(frame, list, padding, editor->icon_size);
}

void load_prototype_lists(LevelEditor *editor){
	// Set the prototype list's pointer for each tab's frame's entity selector.
	V2 padding = {16, 32}; 
	add_entity_selector_to_editor_tab(editor, "Tiles", &editor->tiles, padding);      // Add an entity selector gui component to the "Tiles"    tab and assigned it a prototype list.
	add_entity_selector_to_editor_tab(editor, "Entities", &editor->entities, padding);// Add an entity selector gui component to the "Entities" tab and assigned it a prototype list.
	add_entity_selector_to_editor_tab(editor, "Multi-tile", &editor->multi_tiles, padding);
	add_entity_selector_to_editor_tab(editor, "Multi-entities", &editor->multi_entities, padding);
}

// When the level editor is initialized it is assigned a default font. With this function we can set the tab font at any point.
void set_level_editor_tab_font(LevelEditor *editor, Font *font){
	editor->menu.tab_font = font;
}

static V2 get_tile(V2 mouse_pos){
	return V2{mouse_pos.x / TILE_SIZE, ceil(mouse_pos.y / TILE_SIZE)};
}

static bool is_mouse_on_collision_region(Level *level, V2 mouse_pos, int *index = NULL){
	for(int i = 0; i < level->collision_regions.size; i++){
		Rect col_reg = level->collision_regions[i];
		if(DoRectContainsPoint(col_reg, mouse_pos)){
			if(index) *index = i;
			return true;
		} 
	}
	return false;
}

void update_level_editor(Renderer *renderer, LevelEditor *editor){
	// printf("Layer %d\n", editor->current_layer);
	switch(editor->state){
		case EDITOR_EDIT:{
			if(editor->show_entity_selector){
				update_tabbed_menu(renderer, &editor->menu);
			}else{
				MapObject *tile_map = editor->current_level.layers[editor->current_layer];
				assert(tile_map);
				// When the entity selector is not showing, we can place tiles on the map.
				MouseInfo mouse                  = Game::mouse;
				EntitySelection *selection       = &editor->selected_entity;
				
				if(selection->entity_index != -1 && selection->prototype_list){
					V2 tile_position                    = get_tile(mouse.position);
					int index                           = (int)tile_position.x * LEVEL_SIZE + (int)tile_position.y;
					EntitySelection *entity_on_location = &tile_map[index].selected_entity;
					Entity *entity                      = selection->prototype_list->entities[selection->entity_index];
					
					// TODO: We should be able to erase objects when right clicked, collision regions aswell.
					
					if(entity->type == EntityType::ENTITY_COLLIDER){
						static bool is_phase_one = true;
						static V2 start_pos;
						V2 tile_screen_pos = {(int)tile_position.x * TILE_SIZE, (int)tile_position.y * TILE_SIZE}; 
						if(is_phase_one){
							if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && !is_mouse_on_collision_region(&editor->current_level, mouse.position)){
								start_pos    = tile_screen_pos;
								is_phase_one = false;
							}
							// If we right click on an entity collider if gets removed. At the moment we can only delete collision regions if
							// the collider entity is selected. Maybe we should change this ????
							else if(mouse.right.state == MouseButtonState::MOUSE_PRESSED){
								int index = -1;
								if(is_mouse_on_collision_region(&editor->current_level, mouse.position, &index)){
									assert(index > -1);
									erase_from_array(&editor->current_level.collision_regions, index);
									return;
								}
							}
							
						}else{
							// This code determines the corner positions of the collision region depending on where the second click is pressed.
							Rect collision_region;
							if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && !is_mouse_on_collision_region(&editor->current_level, mouse.position)){
								if(start_pos.x == tile_screen_pos.x && start_pos.y == tile_screen_pos.y){
									V2 final_pos     = {tile_screen_pos.x + editor->icon_size.x, tile_screen_pos.y - editor->icon_size.y};
									collision_region = {start_pos.x, start_pos.y, abs(final_pos.x - start_pos.x), abs(final_pos.y - start_pos.y)};
								}
								else if (start_pos.x <= tile_screen_pos.x && start_pos.y >= tile_screen_pos.y){
									V2 final_pos     = {tile_screen_pos.x + editor->icon_size.x, tile_screen_pos.y - editor->icon_size.y};
									collision_region = {start_pos.x, start_pos.y, abs(final_pos.x - start_pos.x), abs(final_pos.y - start_pos.y)};
								}
								else if (start_pos.x <= tile_screen_pos.x && start_pos.y < tile_screen_pos.y){
									V2 bottom_left   = {start_pos.x, start_pos.y - editor->icon_size.y};
									V2 top_right     = {tile_screen_pos.x + editor->icon_size.x, tile_screen_pos.y};
									int width        = top_right.x - bottom_left.x;
									int height       = top_right.y - bottom_left.y;
									collision_region = {bottom_left.x, bottom_left.y + height, width, height};
								}
								else if (start_pos.x >= tile_screen_pos.x && start_pos.y >= tile_screen_pos.y){
									V2 bottom_left   = {tile_screen_pos.x, tile_screen_pos.y - editor->icon_size.y};
									V2 top_right     = {start_pos.x + editor->icon_size.x, start_pos.y};
									int width        = top_right.x - bottom_left.x;
									int height       = top_right.y - bottom_left.y;
									collision_region = {bottom_left.x, bottom_left.y + height, width, height};
								}
								else if (start_pos.x >= tile_screen_pos.x && start_pos.y < tile_screen_pos.y){
									V2 bottom_right  = {start_pos.x + editor->icon_size.x, start_pos.y - editor->icon_size.y};
									V2 top_left      = {tile_screen_pos.x, tile_screen_pos.y};
									int width        = bottom_right.x - top_left.x;
									int height       = top_left.y - bottom_right.y;
									collision_region = {top_left.x, top_left.y, width, height};
								}
								
								// TODO?: If the resulting rect is contained or contains another rect do not add the collision region.
								add_array(&editor->current_level.collision_regions, collision_region);
								is_phase_one = true;
							}
						}
						return;
					}
					
					if(tile_position.x >= 0 && tile_position.x < LEVEL_SIZE && tile_position.y >= 0 && tile_position.y < LEVEL_SIZE){
						// If we are within bounds and the current tile is not occupied insert the new tile in the current layer.
						if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && entity_on_location->entity_index == -1){
							if(!entity->special_placement){
								tile_map[index].selected_entity = *selection;
								tile_map[index].origin          =  selection;
							}
							else{
								entity->special_placement();
							}
						}
						else if(mouse.right.state == MouseButtonState::MOUSE_PRESSED && entity_on_location->entity_index != -1){
							EntitySelection empty_selection;
							tile_map[index].selected_entity = empty_selection;
						}
						
						
						
					}
					
					
					
				}
			}
			
			if(editor->test_level){
				editor->state = EditorState::EDITOR_TEST;
				init_level_entity_manager(&editor->current_level, &Game::em);
			} 
			break;
		}
		
		case EDITOR_COLLISION:{
			
			
			
			break;
		}
		
		case EDITOR_TEST:{
			update_level(renderer, &editor->current_level, &Game::em);
			check_collisions(&Game::em, &editor->current_level);
			if(!editor->test_level) editor->state = EditorState::EDITOR_EDIT;
			break;
		}
	}
	
}

static void render_editor_level(Renderer *renderer, LevelEditor *editor){
	// Render all the level's layers.
	for(int k = 0; k < LEVEL_LAYERS; k++){
		MapObject *layer = editor->current_level.layers[k];
		for(int j = 0; j < LEVEL_SIZE; j++){
			for(int i = 0; i < LEVEL_SIZE; i++){
				V2 position = {i * TILE_SIZE, j * TILE_SIZE};
				int index = i * LEVEL_SIZE + j;
				EntitySelection *selection = &layer[index].selected_entity;
				if(selection->entity_index != -1 && selection->prototype_list){
					Sprite *icon = &selection->prototype_list->entities[selection->entity_index]->icon.sprite; 
					render_sprite(renderer, icon, position);
				}		
				
			}
		}
	}
}

void render_level_editor(Renderer *renderer, LevelEditor *editor){
	static Rect tileable_area = {0, renderer->window->internalHeight, renderer->window->internalWidth, renderer->window->internalHeight};
	render_colored_rect(renderer, &tileable_area, V3{120,120,120});
	
	editor->selected_entity = get_current_tab(&editor->menu)->e_selector->selected_entity;
	switch(editor->state){
		case EDITOR_EDIT:{
			render_editor_level(renderer, editor);
			render_collision_regions(renderer, &editor->current_level);
			if(editor->show_entity_selector){
				render_tabbed_menu(renderer, &editor->menu);
			}
			else{
				
			}
			
			// Render the number of the current layer.
			char layer_string[] = "Layer: ";
			const char *number_string    = to_string(editor->current_layer);
			const char *complete_string  = strcat(layer_string, number_string);
			render_text(renderer, get_font(&Game::asset_manager, "default"), complete_string, {0,0});
			break;
		}
		
		case EDITOR_TEST:{
			render_level(renderer, &editor->current_level, &Game::em);
			// Render the collision boxes on top of the level. They will be transparent.
			break;
		}
	}
	
	
}

void init_prototype_list(PrototypeList *list, int size, const char *name){
	list->name = name;
	init_array(&list->entities, &Game::main_arena, size);
}

void add_prototype(PrototypeList *list, Entity *e){
	e->prototype_id = list->entities.size;
	add_array(&list->entities, e);
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
				case ENTITY_SLIME:{
					Slime new_slime = cast_and_position_entity<Slime>(entity, position);
					// new_slime.position = position;
					add_array(&em->slimes, new_slime);
					break;
				}
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
		for(int j = 0; j < LEVEL_SIZE; j++){
			for(int i = 0; i < LEVEL_SIZE; i++){
				V2 position = {i * TILE_SIZE, j * TILE_SIZE};
				int index = i * LEVEL_SIZE + j;
				EntitySelection *selection = &layer[index].selected_entity;
				if(selection->entity_index != -1 && selection->prototype_list){
					Sprite *icon = &selection->prototype_list->entities[selection->entity_index]->icon.sprite; 
					render_sprite(renderer, icon, position);
				}		
				
			}
		}
	}
	
	render_entities(em, renderer); 
	
	// Render the last two layers(foreground).
	for(int k = 3; k < 5; k++){
		MapObject *layer = level->layers[k];
		for(int j = 0; j < LEVEL_SIZE; j++){
			for(int i = 0; i < LEVEL_SIZE; i++){
				V2 position = {i * TILE_SIZE, j * TILE_SIZE};
				int index = i * LEVEL_SIZE + j;
				EntitySelection *selection = &layer[index].selected_entity;
				if(selection->entity_index != -1 && selection->prototype_list){
					Sprite *icon = &selection->prototype_list->entities[selection->entity_index]->icon.sprite; 
					render_sprite(renderer, icon, position);
				}		
				
			}
		}
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