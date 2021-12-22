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
	
	texture = make_texture("assets/textures/4.png");
	
	sprite.info.texture = texture;
	sprite.info.size = {128,128};
	sprite.timer.timeToWait = 1;
	add_animation_frame(&sprite, {0,0,32,32});
	add_animation_frame(&sprite, {32,0,32,32});
    
}

void Game::UpdateGame(float dt){
	// Event e;
	// while(GetNextEvent(&e)){
		// if(e.key == GLFW_KEY_P && e.action == GLFW_PRESS){
		// }
		// else if(e.key == GLFW_KEY_S && e.action == GLFW_PRESS){
		// }
	// }
    
	
}

Rect A = {300,300,200,200};
Rect B = {350,250,200,200};
Rect C = {375,225,200,200};

void Game::DrawGame(float dt, float fps){
	// static ShaderProgram test_shader = make_shader(renderer, "assets/shaders/fragment_shader_test.glsl", "test_shader");
	render_animation(renderer, &sprite, {0,renderer->window->internalHeight});
	
    
    renderer_draw(renderer);
    swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
    UpdateGame(dt);
    DrawGame(dt, fps);
    poll_events();
}