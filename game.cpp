#include "game.h"
#include "window.h"
#include "input.h"
#include "collision.h"
// #include "engine/sprite.h"

#include "GLFW/glfw3.h"

#include <stdlib.h>


AssetManager Game::asset_manager;

Game::Game(Renderer *r, Window *w){
    
    renderer = r;
    window = w;
    
    
}

void Game::UpdateGame(float dt){
  
    
	
}

Rect A = {300,300,200,200};
Rect B = {350,250,200,200};
Rect C = {375,225,200,200};
void Game::DrawGame(float dt, float fps){
	static Texture texture = make_texture("assets/textures/4.png");
	render_quad(renderer, &A, &texture, 0);
	render_quad(renderer, &B, &texture, 1, NULL, false, 1, {0,255,0});
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