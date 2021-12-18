#include "game.h"
#include "window.h"
#include "input.h"
#include "collision.h"
#include "audio.h"
// #include "engine/sprite.h"

#include "GLFW/glfw3.h"

#include <stdlib.h>


AssetManager Game::asset_manager;

Game::Game(Renderer *r, Window *w){
    
    renderer = r;
    window = w;
    
    shot = make_sound("assets/sounds/shot.ogg");
	forest = make_sound("assets/sounds/forest_sound.ogg");
	music = make_music("assets/sounds/The Biggest Discovery.ogg");
	// print_sound_data(&forest);
}

void Game::UpdateGame(float dt){
	Event e;
	while(GetNextEvent(&e)){
		if(e.key == GLFW_KEY_P && e.action == GLFW_PRESS){
			play_music(&music);
			// play_sound(&forest);
		}
		else if(e.key == GLFW_KEY_S && e.action == GLFW_PRESS){
			stop_music(&music);
			// stop_sound(&forest);
		}
	}
    
	
}

Rect A = {300,300,200,200};
Rect B = {350,250,200,200};
Rect C = {375,225,200,200};

void Game::DrawGame(float dt, float fps){
	static ShaderProgram test_shader = make_shader(renderer, "assets/shaders/fragment_shader_test.glsl", "test_shader");
	static Texture texture = make_texture("assets/textures/4.png");
	render_quad(renderer, &A, &texture, 2);
	render_quad_with_shader(renderer, &B, &texture, test_shader, 0, NULL, false, 1, {0,255,0});
	render_quad(renderer, &C, &texture, 1);
	// play_sound(&shot);
    // render_colored_rect(renderer, &A, {0,255,0}    , 1,  0);
	// render_colored_rect(renderer, &C, {255,255,0}  , 1,  90);
	// render_colored_rect(renderer, &B, {0,255,255}  , 1,  20);
	
    
    renderer_draw(renderer);
    swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
    UpdateGame(dt);
    DrawGame(dt, fps);
    poll_events();
}