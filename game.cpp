#include "game.h"
#include "entity.h"
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
	
	frame_texture = make_texture("assets/textures/4.png");
	smiley = make_texture("assets/textures/smiley.png");
	link = make_texture("assets/textures/link.png");
	texture = make_texture(4, 64, 32);
	int size = texture.channels * texture.width * texture.height;
	
	for(int j = 0; j < texture.height; j++){
		for(int i = 0; i < texture.width; i++){
			// for(int i = 0; i < size; i++){
				// texture.data_buffer[i] = 255;
			if(i % 2){
				set_pixel(&texture, V4{255,0,255,255}, V2{i, j});
			}else{
				set_pixel(&texture, V4{255,150,0,255}, V2{i, j});
			}
			// }
		}
	}
	Texture texture;
	// Sprite frame_sprite;
	// frame_sprite.info.texture = frame_texture;
	init_frame(&frame, {0, 720, 500, 500}, texture);
	set_padding(&frame, 5, 5);
	add_button(&frame, {0, 0, 100, 40}, "Hola", frame_texture, {0,0,0,0});
	move_frame(&frame, {500,500});
	// clear_texture(&texture);
	// font = Font("assets/fonts/Chicken Crispy.ttf", 32);
	
	
	Enemy e;
	e.collision_box = Rect{500,500,10,10};
	add_enemy(&em, &e);
}

void Game::UpdateGame(float dt){
	// Event e;
	// while(GetNextEvent(&e)){
		// if(e.key == GLFW_KEY_P && e.action == GLFW_PRESS){
		// }
		// else if(e.key == GLFW_KEY_S && e.action == GLFW_PRESS){
		// }
	// }
    
	// entity_collision(&p1);
	// entity_collision(&p1);
}

// Rect A = {300,300,200,200};
// Rect B = {350,250,200,200};
// Rect C = {375,225,200,200};

static Rect r  = {640, 360, 100, 100};
static Rect r1 = {660, 360, 100, 100};

void Game::DrawGame(float dt, float fps){
	// static ShaderProgram test_shader = make_shader(renderer, "assets/shaders/fragment_shader_test.glsl", "test_shader");
	// render_animation(renderer, &sprite, {0,(float)renderer->window->internalHeight});
	const char *a = "Qué Cómo sí á úlcera";
	render_frame(renderer, &frame);
	render_text(renderer, &font, a, {450, 450}, V3{255,255,255});
	render_text(renderer, &font, "The quick brown fox jumps over the lazy dog", {640, 360}, V3{255,0,255}, true);
	render_quad(renderer, &r1, &link);
	render_quad(renderer, &r, &smiley);
	
    
    renderer_draw(renderer);
    swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
    UpdateGame(dt);
	check_collisions(&em);
    DrawGame(dt, fps);
    poll_events();
}