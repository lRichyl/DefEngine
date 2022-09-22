#include "game.h"
#include "entity.h"
#include "window.h"
#include "collision.h"
#include "audio.h"
#include "sprite.h"

#include "GLFW/glfw3.h"

#include <stdlib.h>

namespace def {
	AssetManager            Game::asset_manager;
	MemoryArena             Game::main_arena;
	MouseInfo               Game::mouse;
	EntityManager           Game::em;
	float                   Game::dt;
	Camera                  Game::camera;
	Console                 Game::console;
	DefArray<RenderCommand> Game::layers_render_commands[LEVEL_LAYERS];
	LevelEditor             Game::level_editor;

	static void render_command_lists(){
		for(int j = 0; j < LEVEL_LAYERS; j++){
			render_queued_commands(&Game::layers_render_commands[j]);
		}
	}

	static EditableString e_string;
	Game::Game(Renderer *r, Window *w){
	    init_memory_arena(&main_arena, 10000000);
		init_entity_manager(&em);
		init_asset_manager(&asset_manager);
		init_array(&Input::unicode_array, &Game::main_arena, UNICODE_BUFFER_SIZE);



		// Init the render commands lists.
		for(int i = 0; i < LEVEL_LAYERS; i++){
			init_array(&layers_render_commands[i], &Game::main_arena, LEVEL_WIDTH * LEVEL_HEIGHT);
		}	
	    renderer = r;
	    window = w;
		
		// add_to_string(&e_string, 'A');
		// add_to_string(&e_string, 'B');
		// add_to_string(&e_string, 'C');
		// add_to_string(&e_string, 0xE1);
		
		add_font(&asset_manager, "default", "assets/fonts/Simvoni.ttf", 40); 
		add_font(&asset_manager, "tabs_font", "assets/fonts/Simvoni.ttf", 16);
		add_font(&asset_manager, "console_font", "assets/fonts/Simvoni.ttf", 18);
		add_font(&asset_manager, "prototype_list_font", "assets/fonts/Simvoni.ttf", 32);
		
		// frame_texture = make_texture("assets/textures/4.png");
		add_texture(&asset_manager, "frame texture", "assets/textures/4.png");
		add_texture(&asset_manager, "test_tiles", "assets/textures/test_tiles.png");
		add_texture(&asset_manager, "level_editor_frame", "assets/textures/level_editor_frame.png");
		add_texture(&asset_manager, "level_editor_tab", "assets/textures/level_editor_tab.png");
		
		add_shader(&asset_manager, renderer, "Console_shader", "assets/shaders/fragment_shader_console.fs");
		add_shader(&asset_manager, renderer, "gui_shader", "assets/shaders/fragment_shader_gui.fs");
		
		init_entity(&em.player);
		init_console(&console, renderer);
		init_camera(&camera, renderer);
		init_level_editor(&level_editor, window);
		
	}
	void Game::UpdateGame(float dt){
		mouse = GetMouseInfo(renderer->window); // Only call this funtion once per frame. To get mouse info reference the static 'mouse' variable. 
		#ifdef DEV
			if(console.show){
				update_console(&console, &level_editor, renderer);
			}
			// 96 = |    Fix this.
			if(was_key_pressed(96)){
				toggle_console(&console);
			}

		#endif
		Game::dt = dt;
		switch(state){
			case GAME_PLAY:{
				// This is the state the actual game will start on.(Intro screen, menu, etc).
				#ifdef DEV
					if(was_key_pressed(GLFW_KEY_TAB)){
						state = GameState::GAME_EDITOR;
					}
				#endif
				break;
			}
			
			#ifdef DEV
			case GAME_EDITOR:{
				if(!console.show)
					update_level_editor(&level_editor, renderer);
				
				break;
			}
			#endif
		}
	    
	}

	static Rect r  = {640, 360, 100, 100};
	static Rect r1 = {660, 360, 100, 100};

	void Game::DrawGame(float dt, float fps){
		switch(state){
			case GAME_PLAY:{
			
				break;
			}
			
			case GAME_EDITOR:{
				render_level_editor(&level_editor, renderer);
				break;
			}
		}
		if(console.show)
			render_console(&console, renderer);
		// render_colored_rect(renderer, &r1, V3 {0 , 159, 255});
		render_command_lists();
	    renderer_draw(renderer);
	    swap_buffers(window);
	}

	void Game::GameLoop(float dt, float fps){
	    UpdateGame(dt);
	    if(level_editor.state != EditorState::EDITOR_EDIT)
			check_collisions(&Game::em);
	    DrawGame(dt, fps);
		Game::camera.moved = false; // ??
		clear_mouse_info();
		clear_array(&Input::unicode_array);
		clear_keys_state();
		clear_render_lists();
	    poll_events();
	}

	void Game::clear_render_lists(){
	for(int i = 0; i < LEVEL_LAYERS; i++){
			clear_array(&layers_render_commands[i]);
		}
	}
}