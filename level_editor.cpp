#include "game.h"
#include "level_editor.h"
#include "asset_manager.h"
#include "entity.h"
#include "input.h"
#include "utilities.h"
#include <cmath>

char *HEADER = "LVLDAT";

static void add_tile_prototype(LevelEditor *editor, Texture texture, Rect clipping_box, V2 area = {1,1}){
	Tile *tile                = allocate_from_arena<Tile>(&Game::main_arena);
	tile->sprite = Sprite();
	if(area.x != 1 && area.y != 1){
		tile->area_type = AreaType::AREA_MULTI;
	}
	tile->area   = area;
	tile->sprite.clipping_box   = clipping_box;
	tile->sprite.info.size      = {area.x * editor->icon_size.x, area.y * editor->icon_size.y};
	tile->sprite.info.texture   = texture;
	tile->icon.sprite           = tile->sprite;
	tile->icon.sprite.info.size = editor->icon_size;
	
	if(tile->area_type == AreaType::AREA_MULTI){
		add_prototype(&editor->multi_tiles, tile);
	}
	else
		add_prototype(&editor->tiles, tile);
	
}


static void add_entity_prototype(LevelEditor *editor, Entity *entity){
	if(entity->area_type == AreaType::AREA_MULTI){
		add_prototype(&editor->multi_entities, entity);
	}
	else
		add_prototype(&editor->entities, entity);
}



template<typename T>
void add_entity_prototype(LevelEditor *editor){
	T *e = allocate_from_arena<T>(&Game::main_arena);
	init_entity(e, editor->icon_size);
	
	add_entity_prototype(editor, e);
}

void init_level_editor(LevelEditor *editor, Rect bounding_box, Texture frame_texture, Texture tab_texture, Renderer *renderer){
	init_tabbed_menu(&editor->menu, bounding_box, tab_texture, frame_texture, get_font(&Game::asset_manager, "tabs_font"), {0,0,0}, {0,0,0});
	init_prototype_list(&editor->tiles, TILES_AMOUNT, "Tiles");
	init_prototype_list(&editor->entities, ENTITIES_AMOUNT, "Entities");
	init_prototype_list(&editor->multi_tiles, ENTITIES_AMOUNT, "Multi-tile");
	init_prototype_list(&editor->multi_entities, ENTITIES_AMOUNT, "Multi-entities");
	
	// TODO: This should be done when creating a new level.
	init_level(&editor->current_level);
	
	editor->icon_size            = V2 {32,32}; // At the moment this can only be set during initialization.
	editor->show_entity_selector = false;
	editor->current_layer        = 0;
	editor->test_level           = false;
	// editor->show_console         = false;
	
	// Add tile prototypes.
	// We currently don't have a way of identifying a particular tile other than visually. We may add a tag later to identity in code the tile type.
	add_tile_prototype(editor, get_texture(&Game::asset_manager, "test_tiles"), {0,0,32,32});
	add_tile_prototype(editor, get_texture(&Game::asset_manager, "test_tiles"), {32,0,32,32});
	add_tile_prototype(editor, get_texture(&Game::asset_manager, "test_tiles"), {0,32,64,64}, {2,2});
	
	// Add entities prototypes.
	// Collider *collider = allocate_from_arena<Collider>(&Game::main_arena); init_collider(collider, editor->icon_size); add_entity_prototype(editor, collider);
	// Slime    *slime    = allocate_from_arena<Slime>   (&Game::main_arena); init_slime   (slime,    editor->icon_size); add_entity_prototype(editor, slime);
	// Player   *player   = allocate_from_arena<Player>  (&Game::main_arena); init_player  (player,   editor->icon_size); add_entity_prototype(editor, player);
	add_entity_prototype<Collider>(editor);
	add_entity_prototype<Slime>(editor);
	add_entity_prototype<Player>(editor);
	add_entity_prototype<Multi>(editor);
	// IMPLEMENT MULTI TILE ENTITIES.
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
			if(Game::console.show_console){
				update_console(&Game::console, editor, renderer);
				// The console should interrupt any editing so we always return when the console is active.
				return;
			}
			
			if(editor->show_entity_selector){
				update_tabbed_menu(renderer, &editor->menu);
			}else{
				MapObject *tile_map = editor->current_level.layers[editor->current_layer];
				assert(tile_map);
				// When the entity selector is not showing, we can place tiles on the map.
				MouseInfo mouse            = Game::mouse;
				V2 world_pos               = get_world_position(&Game::camera, mouse.position);
				EntitySelection *selection = &editor->selected_entity;
				
				if(selection->entity_index != -1 && selection->prototype_list){
					V2 tile_position                    = get_tile(world_pos);
					V2 tile_pixel_pos = {(int)tile_position.x * TILE_SIZE, (int)tile_position.y * TILE_SIZE};
					int index                           = (int)tile_position.x * LEVEL_SIZE + (int)tile_position.y;
					EntitySelection *entity_on_location = &tile_map[index].selected_entity;
					Entity *entity                      = get_selection_entity(selection);
					
					// TODO: We should be able to erase objects when right clicked, collision regions aswell.
					
					if(entity->type == EntityType::ENTITY_COLLIDER){
						static bool is_phase_one = true;
						static V2 start_pos;
						 
						if(is_phase_one){
							if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && !is_mouse_on_collision_region(&editor->current_level, world_pos)){
								start_pos    = tile_pixel_pos;
								is_phase_one = false;
							}
							// If we right click on an entity collider if gets removed. At the moment we can only delete collision regions if
							// the collider entity is selected. Maybe we should change this ????
							else if(mouse.right.state == MouseButtonState::MOUSE_PRESSED){
								int collision_region_index = -1;
								if(is_mouse_on_collision_region(&editor->current_level, world_pos, &collision_region_index)){
									assert(collision_region_index > -1);
									erase_from_array(&editor->current_level.collision_regions, collision_region_index);
									return;
								}
							}
							
						}else{
							// This code determines the corner positions of the collision region depending on where the second click is pressed.
							Rect collision_region;
							if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && !is_mouse_on_collision_region(&editor->current_level, world_pos)){
								if(start_pos.x == tile_pixel_pos.x && start_pos.y == tile_pixel_pos.y){
									V2 final_pos     = {tile_pixel_pos.x + editor->icon_size.x, tile_pixel_pos.y - editor->icon_size.y};
									collision_region = {start_pos.x, start_pos.y, abs(final_pos.x - start_pos.x), abs(final_pos.y - start_pos.y)};
								}
								else if (start_pos.x <= tile_pixel_pos.x && start_pos.y >= tile_pixel_pos.y){
									V2 final_pos     = {tile_pixel_pos.x + editor->icon_size.x, tile_pixel_pos.y - editor->icon_size.y};
									collision_region = {start_pos.x, start_pos.y, abs(final_pos.x - start_pos.x), abs(final_pos.y - start_pos.y)};
								}
								else if (start_pos.x <= tile_pixel_pos.x && start_pos.y < tile_pixel_pos.y){
									V2 bottom_left   = {start_pos.x, start_pos.y - editor->icon_size.y};
									V2 top_right     = {tile_pixel_pos.x + editor->icon_size.x, tile_pixel_pos.y};
									int width        = top_right.x - bottom_left.x;
									int height       = top_right.y - bottom_left.y;
									collision_region = {bottom_left.x, bottom_left.y + height, width, height};
								}
								else if (start_pos.x >= tile_pixel_pos.x && start_pos.y >= tile_pixel_pos.y){
									V2 bottom_left   = {tile_pixel_pos.x, tile_pixel_pos.y - editor->icon_size.y};
									V2 top_right     = {start_pos.x + editor->icon_size.x, start_pos.y};
									int width        = top_right.x - bottom_left.x;
									int height       = top_right.y - bottom_left.y;
									collision_region = {bottom_left.x, bottom_left.y + height, width, height};
								}
								else if (start_pos.x >= tile_pixel_pos.x && start_pos.y < tile_pixel_pos.y){
									V2 bottom_right  = {start_pos.x + editor->icon_size.x, start_pos.y - editor->icon_size.y};
									V2 top_left      = {tile_pixel_pos.x, tile_pixel_pos.y};
									int width        = bottom_right.x - top_left.x;
									int height       = top_left.y - bottom_right.y;
									collision_region = {top_left.x, top_left.y, width, height};
								}
								
								// TODO?: If the resulting rect is contained or contains another collision region do not add the collision region.
								add_array(&editor->current_level.collision_regions, collision_region);
								is_phase_one = true;
							}
						}
						return;
					}
					
					
					// printf("%f, %f", tile_position.x, tile_position.y);
					// printf("         %d\n", LEVEL_SIZE * TILE_SIZE);

					if(tile_position.x >= 0 && tile_position.x < LEVEL_SIZE && tile_position.y > 0 && tile_position.y < LEVEL_SIZE){
						// If we are within bounds and the current tile is not occupied insert the new tile in the current layer.
						if(mouse.left.state == MouseButtonState::MOUSE_PRESSED && entity_on_location->entity_index == -1) {
							// Insert the tile only if the entity on the selected tile is not part of a multi tile object.
							if(entity->area.x == 1 && entity->area.y == 1){
								tile_map[index].selected_entity                = *selection;
								tile_map[index].selected_entity.tile_map_index = index;
								tile_map[index].origin_index                   = index;
								tile_map[index].origin                         = &tile_map[index].selected_entity;
								tile_map[index].selected_entity.entity_type    = entity->type;
								tile_map[index].selected_entity.area_type      = entity->area_type;
							}
							else{
								// If it's multi tile we first check that there's not an object already in its area.
								for(int j = 0; j < entity->area.y; j++){
									for(int i = 0; i < entity->area.x; i++){
										int multi_tile_index = index - i + (j * LEVEL_SIZE);
										EntitySelection *selection = &tile_map[multi_tile_index].selected_entity;
										if(selection->entity_index != -1) return;
									}
								}
								
								// If the entity or tile occuppies an area bigger than 1x1 we set the corresponding tiles to the correct entity index
								// and set the origin to the tile where we clicked.
								tile_map[index].selected_entity              = *selection;
								tile_map[index].selected_entity.tile_map_index  = index;
								tile_map[index].origin_index = index;
								tile_map[index].origin                       = &tile_map[index].selected_entity;
								for(int j = 0; j < entity->area.y; j++){
									for(int i = 0; i < entity->area.x; i++){
										if(j == 0 && i == 0) continue;
										int multi_tile_index = index - i + (j * LEVEL_SIZE) ;
										tile_map[multi_tile_index].selected_entity                = *selection;
										tile_map[multi_tile_index].selected_entity.tile_map_index = multi_tile_index;
										tile_map[multi_tile_index].origin_index                   = index;
										tile_map[multi_tile_index].origin                         = &tile_map[index].selected_entity;
										tile_map[multi_tile_index].selected_entity.entity_type    = entity->type;
										tile_map[multi_tile_index].selected_entity.area_type      = entity->area_type;
									}
								}
							}
							
							if(entity->special_placement)
								entity->special_placement();
						}
						else if(mouse.right.state == MouseButtonState::MOUSE_PRESSED && entity_on_location->entity_index != -1){
							// When right click is pressed the tile or multi tile on mouse position gets erased from the tile map. 
							MapObject *map_object = &tile_map[index];
							Entity *entity_to_erase = get_selection_entity(&map_object->selected_entity);
							if(&map_object->selected_entity == map_object->origin && entity_to_erase->area.x == 1 && entity_to_erase->area.y == 1){
								EntitySelection empty_selection;
								tile_map[index].selected_entity = empty_selection;
							}
							else{
								int origin_index = map_object->origin_index;
								// assert(origin_index > -1);
								EntitySelection empty_selection;
								for(int j = 0; j < entity_to_erase->area.y; j++){
									for(int i = 0; i < entity_to_erase->area.x; i++){
										int multi_tile_index = origin_index - i + (j * LEVEL_SIZE) ;
										tile_map[multi_tile_index].selected_entity = empty_selection;
										// tile_map[multi_tile_index].selected_entity.index = -1;
										// tile_map[multi_tile_index].selected_entity.entity_index = -1;
										tile_map[multi_tile_index].origin          = NULL;
									}
								}
							}
						}
					}
				}
			}
			
			
			
			if(editor->test_level){
				editor->state = EditorState::EDITOR_TEST;
				reset_camera(&Game::camera);
				init_level_entity_manager(&editor->current_level, &Game::em);
			} 
			break;
		}
		
		case EDITOR_TEST:{
			update_level(renderer, &editor->current_level, &Game::em);
			check_collisions(&Game::em, &editor->current_level);
			if(!editor->test_level){
				editor->state = EditorState::EDITOR_EDIT;
				reset_camera(&Game::camera);
			} 
			break;
		}
	}
	
}

void render_tile_map(Renderer *renderer, MapObject *tile_map){
	for(int j = 0; j < LEVEL_SIZE; j++){
		for(int i = 0; i < LEVEL_SIZE; i++){
			V2 position = {i * TILE_SIZE, j * TILE_SIZE};
			int index = i * LEVEL_SIZE + j;
			EntitySelection *selection = &tile_map[index].selected_entity;
			MapObject *map_object      = &tile_map[index];
			if(selection->entity_index != -1 && selection->prototype_list){
				if(&map_object->selected_entity == map_object->origin){
					Sprite *icon = &selection->prototype_list->entities[selection->entity_index]->icon.sprite; 
					// render_sprite(renderer, icon, position);
					Entity *entity = get_selection_entity(selection);
					Rect bounding_box = {position.x, position.y, entity->area.x * TILE_SIZE, entity->area.y * TILE_SIZE};
					render_quad(renderer, &bounding_box, &icon->info.texture, &icon->clipping_box);
				}
			}		
			
		}
	}	
}

static void render_editor_level(Renderer *renderer, LevelEditor *editor){
	// Render all the level's layers.
	for(int k = 0; k < LEVEL_LAYERS; k++){
		MapObject *layer = editor->current_level.layers[k];
		render_tile_map(renderer, layer);
	}
}

void render_level_editor(Renderer *renderer, LevelEditor *editor){
	static float level_size_in_pixels = LEVEL_SIZE * TILE_SIZE;
	static Rect tileable_area = {0, level_size_in_pixels - TILE_SIZE ,level_size_in_pixels, level_size_in_pixels - TILE_SIZE};
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
			render_text(renderer, get_font(&Game::asset_manager, "default"), complete_string, {0,0}, {255,255,255}, false,  get_shader_ptr(&Game::asset_manager, "Gui_shader"));
			
			if(Game::console.show_console){
				render_console(&Game::console, renderer);
				return;
			}
			
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

bool save_level(LevelEditor *editor, const char *name){
	if(!check_if_file_exists(name)) return false;
	FILE *file;
	file = fopen(name, "wb+");
	if(!file) return false;
	
	fwrite(HEADER, sizeof(char), 6, file);
	fwrite(editor->current_level.name, sizeof(char), LEVEL_NAME_SIZE, file);

	// Data from the 5 layers of the level.
	for(int i = 0; i < LEVEL_LAYERS; i++){
		fwrite((void*)editor->current_level.layers[i], sizeof(MapObject), LEVEL_SIZE * LEVEL_SIZE, file);
	}
	
	// Collision regions data.
	fwrite((void*)&editor->current_level.collision_regions.size, sizeof(int), 1, file);
	fwrite((void*)editor->current_level.collision_regions.data, sizeof(Rect), editor->current_level.collision_regions.size, file);
	
	fclose(file);
	return true;
}

bool save_new_level(LevelEditor *editor, const char *name){
	// Maybe we should return an error code so that we know what to print console.
	if(check_if_file_exists(name)) return false;
	FILE *file;
	file = fopen(name, "wb");
	if(!file) return false;
	
	fwrite(HEADER, sizeof(char), 6, file);
	fwrite(editor->current_level.name, sizeof(char), LEVEL_NAME_SIZE, file);
	// fprintf(file, "%s\n", editor->current_level.name);

	// Data from the 5 layers of the level.
	for(int i = 0; i < LEVEL_LAYERS; i++){
		fwrite(editor->current_level.layers[i], sizeof(MapObject), LEVEL_SIZE * LEVEL_SIZE, file);
	}
	
	// Collision regions data.
	fwrite((void*)&editor->current_level.collision_regions.size, sizeof(int), 1, file);
	fwrite((void*)editor->current_level.collision_regions.data, sizeof(Rect), editor->current_level.collision_regions.size, file);
	// fprintf(file, "%d", editor->current_level.collision_regions.size);
	
	fclose(file);
	return true;
}

bool load_level(LevelEditor *editor, const char *name){
	if(!check_if_file_exists(name)) return false;
	FILE *file;
	file = fopen(name, "rb");
	if(!file) return false;
	
	Level *level = &editor->current_level;
	// empty_level(level);
	
	char header[10]{};
	// fscanf(file, "%s", header);
	fread(header, sizeof(char), 6, file);
	if(strcmp(header, HEADER) != 0) return false;
	
	char level_name[LEVEL_NAME_SIZE]{};
	// fscanf(file, "%s", level_name);
	fread(level_name, sizeof(char), LEVEL_NAME_SIZE, file);
	strcpy(level->name, level_name);
	
	// Data from the 5 layers of the level.
	for(int i = 0; i < LEVEL_LAYERS; i++){
		fread(level->layers[i], sizeof(MapObject), LEVEL_SIZE * LEVEL_SIZE, file);
	}
	
	for(int j = 0; j < LEVEL_LAYERS; j++){
		MapObject *layer = level->layers[j];
		for(int i = 0; i < LEVEL_SIZE * LEVEL_SIZE; i++){
			int origin_index = layer[i].origin_index;
			EntitySelection *selection = &layer[origin_index].selected_entity;
			layer[i].origin = selection;
			
			switch(selection->entity_type){
				case ENTITY_TILE:{
					if(selection->area_type == AreaType::AREA_SINGLE){
						selection->prototype_list = &editor->tiles;
					}
					else
						selection->prototype_list = &editor->multi_tiles;
				break;
				}
				default:{
					if(selection->area_type == AreaType::AREA_SINGLE){
						selection->prototype_list = &editor->entities;
					}
					else
						selection->prototype_list = &editor->multi_entities;
				break;
				}
			}
		}
	}
	
	// FIX THIS.
	// Collision regions data.
	fread((void*)&level->collision_regions.size, sizeof(int), 1, file);
	fread((void*)level->collision_regions.data, sizeof(Rect), level->collision_regions.size, file);
	
	
	fclose(file);
	return true;
}