#include "game.h"
#include "entity.h"
#include "window.h"
#include "collision.h"
#include "audio.h"
#include "sprite.h"

#include "GLFW/glfw3.h"

#include <stdlib.h>


AssetManager  Game::asset_manager;
MemoryArena   Game::main_arena;
MouseInfo     Game::mouse;
EntityManager Game::em;
float         Game::dt;
Camera        Game::camera;
Console       Game::console;

Game::Game(Renderer *r, Window *w){
    init_memory_arena(&main_arena, 10000000);
	init_entity_manager(&em);
	init_asset_manager(&asset_manager);
    renderer = r;
    window = w;
	
	add_font(&asset_manager, "default", "assets/fonts/Simvoni.ttf", 40); 
	add_font(&asset_manager, "tabs_font", "assets/fonts/Simvoni.ttf", 16);
	add_font(&asset_manager, "console_font", "assets/fonts/Simvoni.ttf", 16);
	add_font(&asset_manager, "prototype_list_font", "assets/fonts/Simvoni.ttf", 32);
	
	// frame_texture = make_texture("assets/textures/4.png");
	add_texture(&asset_manager, "frame texture", "assets/textures/4.png");
	add_texture(&asset_manager, "test_tiles", "assets/textures/test_tiles.png");
	add_texture(&asset_manager, "level_editor_frame", "assets/textures/level_editor_frame.png");
	add_texture(&asset_manager, "level_editor_tab", "assets/textures/level_editor_tab.png");
	
	add_shader(&asset_manager, renderer, "Console_shader", "assets/shaders/fragment_shader_console.fs");
	add_shader(&asset_manager, renderer, "Gui_shader", "assets/shaders/fragment_shader_gui.fs");

	smiley = make_texture("assets/textures/smiley.png");
	link = make_texture("assets/textures/link.png");
	
	init_console(&console, renderer);
	init_camera(&camera, renderer);
	init_level_editor(&level_editor, {500,500,500,500}, get_texture(&asset_manager, "level_editor_frame"), get_texture(&asset_manager, "level_editor_tab"), renderer);
	// set_level_editor_tab_font(&level_editor, get_font(&Game::asset_manager, "tabs_font"));
	add_tab(&level_editor.menu, "Tiles");
	add_tab(&level_editor.menu, "Entities");
	add_tab(&level_editor.menu, "Multi-tile");
	add_tab(&level_editor.menu, "Multi-entities");
	load_prototype_lists(&level_editor);
	
}

void Game::UpdateGame(float dt){
	mouse = GetMouseInfo(renderer->window); // Only call this funtion once per frame. To get mouse info reference the static 'mouse' variable. 
	Game::dt = dt;
	switch(state){
		case GAME_PLAY:{
			// This is the state the actual game will start on.(Intro screen, menu, etc).
			Event e;
			while(GetNextEvent(&e)){
				#ifdef DEV //Open the editor.
				if(e.key == GLFW_KEY_TAB && e.action == GLFW_PRESS){
					state = GameState::GAME_EDITOR;
				}
				#endif
			}
			break;
		}
		
		#ifdef DEV
		case GAME_EDITOR:{
			Event e;
			static V2 pos = {0,0};
			while(GetNextEvent(&e)){
				
				if(e.key == GLFW_KEY_TAB && e.action == GLFW_PRESS){
					state = GameState::GAME_PLAY;
				}
				else if(e.key == GLFW_KEY_T && e.action == GLFW_PRESS){
					level_editor.test_level = !level_editor.test_level;
				}
				else if(e.key == GLFW_KEY_E && e.action == GLFW_PRESS){
					level_editor.show_entity_selector = !level_editor.show_entity_selector;
				}
				else if(e.key == GLFW_KEY_X && e.action == GLFW_PRESS){
					level_editor.current_layer++;
					if(level_editor.current_layer >= LEVEL_LAYERS) level_editor.current_layer = 0;
				}
				else if(e.key == GLFW_KEY_Z && e.action == GLFW_PRESS){
					level_editor.current_layer--;
					if(level_editor.current_layer < 0) level_editor.current_layer = LEVEL_LAYERS - 1;
				}
				else if(e.key == GLFW_KEY_ESCAPE && e.action == GLFW_PRESS){
					console.show_console = !console.show_console;
				}
			}
			
			if(mouse.wheel == ScrollWheelState::WHEEL_FORWARDS){
				zoom(&camera);
			}
			else if (mouse.wheel == ScrollWheelState::WHEEL_BACKWARDS){
				zoom(&camera, true);
			}
			
			V2 movement = {0,0};
			if(IsKeyPressed(renderer->window, GLFW_KEY_RIGHT)){
				movement.x = camera.speed * Game::dt;
				pos.x += movement.x;
				set_camera_position(&Game::camera, pos);
				camera.movement = movement;
			}
			else if(IsKeyPressed(renderer->window, GLFW_KEY_LEFT)){
				movement.x = -(camera.speed * Game::dt);
				pos.x += movement.x;
				set_camera_position(&Game::camera, pos);
				camera.movement = movement;
			}
			
			if(IsKeyPressed(renderer->window, GLFW_KEY_UP)){
				movement.y = camera.speed * Game::dt;
				pos.y += movement.y;
				set_camera_position(&Game::camera, pos);
				camera.movement = movement;
			}
			else if(IsKeyPressed(renderer->window, GLFW_KEY_DOWN)){
				movement.y = -(camera.speed * Game::dt);
				pos.y += movement.y;
				set_camera_position(&Game::camera, pos);
				camera.movement = movement;
			}
			update_level_editor(renderer, &level_editor);
			break;
		}
		#endif
	}
    
	// entity_collision(&p1);
	// entity_collision(&p1);
}



static Rect r  = {640, 360, 100, 100};
static Rect r1 = {660, 360, 100, 100};

void Game::DrawGame(float dt, float fps){
	// const char *a = "Qué Cómo sí á úlcera";
	// render_frame(renderer, &frame);
	// render_text(renderer, get_font(&asset_manager, "default"), a, {450, 450}, V3{255,255,255});
	// render_text(renderer, get_font(&asset_manager, "default"), "The quick brown fox jumps over the lazy dog", {640, 360}, V3{255,0,255}, true);
	// render_quad(renderer, &r1, &link);
	// render_quad(renderer, &r, &smiley);
	
	switch(state){
		case GAME_PLAY:{
		
			break;
		}
		
		case GAME_EDITOR:{
			render_level_editor(renderer, &level_editor);
			break;
		}
	}
	
    
	
    renderer_draw(renderer);
    swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
    UpdateGame(dt);
	
    DrawGame(dt, fps);
	Game::camera.moved = false;
	clear_mouse_info();
    poll_events();
}