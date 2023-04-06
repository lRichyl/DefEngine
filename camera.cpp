#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"
#include "input.h"
#include "game.h"

#define CAMERA_SPEED 100

namespace def {
	void init_camera(Camera *camera, Renderer *renderer){
		camera->renderer = renderer;
		camera->position = {0,0};
		camera->size     = VEC_2D(renderer->window->internalWidth, renderer->window->internalHeight);
	}

	void set_camera_position(){
		// camera->position = position;
		// When we move the camera we change the view matrix for all the batches that are using the default shader. 
		// If we for example wanna use a special shader for the player we have to update its view matrix to the same as the default matrix. 
		Camera *camera = &Game::camera;
		// Turn this into a function when we need to update other shaders.
		ShaderProgram shader = camera->renderer->default_shader_program;
		glUseProgram(shader.id);
		
		camera->view = glm::translate(glm::mat4(1.0f), glm::vec3(camera->position.x, camera->position.y, 0.0f));
		int view_uniform_id = glGetUniformLocation(shader.id, ("u_view"));
		glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(camera->view));
		
		glUseProgram(0);
		
		camera->moved = true;
	}

	void zoom(bool zoom_out){
		Camera  *camera = &Game::camera;
		ShaderProgram shader = camera->renderer->default_shader_program;
		if(zoom_out){
			glUseProgram(shader.id);
			Window *window = camera->renderer->window;
			camera->zoom += camera->zoom_factor;
			float x = window->internalWidth  * camera->zoom;
			float y = window->internalHeight * camera->zoom;
			glm::mat4 projection = glm::ortho(0.0f, x, 0.0f, y, 0.0f, -100.f);
			int projection_uniform_id = glGetUniformLocation(shader.id, ("u_projection"));
			glUniformMatrix4fv(projection_uniform_id, 1, GL_FALSE, glm::value_ptr(projection));
			glUseProgram(0);
			
			camera->size = {x, y};
		}
		else{
			glUseProgram(shader.id);
			Window *window = camera->renderer->window;
			camera->zoom -= camera->zoom_factor;
			float x = window->internalWidth  * camera->zoom;
			float y = window->internalHeight * camera->zoom;
			glm::mat4 projection = glm::ortho(0.0f, x, 0.0f, y, 0.0f, -100.f);
			int projection_uniform_id = glGetUniformLocation(shader.id, ("u_projection"));
			glUniformMatrix4fv(projection_uniform_id, 1, GL_FALSE, glm::value_ptr(projection));
			glUseProgram(0);
			
			camera->size = {x, y};
		}
	}

	VEC_2D get_world_position(VEC_2D position){
		Camera *camera = &Game::camera;
		VEC_2D world_pos;
		VEC_2D scale  = {camera->size.x / camera->renderer->window->internalWidth, camera->size.y / camera->renderer->window->internalHeight};
		position.x *= scale.x;
		position.y *= scale.y;
		world_pos = {position.x - camera->position.x, position.y - camera->position.y};
		
		// printf("%f, %f : %f, %f\n", world_pos.x, world_pos.y, scale.x, scale.y);
		return world_pos;
	}

	VEC_2D get_screen_position(VEC_2D world_pos){
		Camera *camera = &Game::camera;
		VEC_2D screen_pos;
		VEC_2D scale  = {camera->size.x / camera->renderer->window->internalWidth, camera->size.y / camera->renderer->window->internalHeight};
		world_pos.x /= scale.x;
		world_pos.y /= scale.y;
		screen_pos = {world_pos.x - camera->position.x, world_pos.y - camera->position.y};
		return screen_pos;
	}

	void reset_camera(){
		Camera *camera = &Game::camera;
		camera->position = {0,0};
		ShaderProgram shader = camera->renderer->default_shader_program;
		glUseProgram(shader.id);
		
		Window *window = camera->renderer->window;
		
		glm::mat4 projection = glm::ortho(0.0f, (float)window->internalWidth, 0.0f, (float)window->internalHeight, 0.0f, -100.f);
		int projection_uniform_id = glGetUniformLocation(shader.id, ("u_projection"));
		glUniformMatrix4fv(projection_uniform_id, 1, GL_FALSE, glm::value_ptr(projection));
		
		camera->view = glm::translate(glm::mat4(1.0f), glm::vec3(camera->position.x, camera->position.y, 0.0f));
		int view_uniform_id = glGetUniformLocation(shader.id, ("u_view"));
		glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(camera->view));
		
		glUseProgram(0);
		
		camera->size = VEC_2D(window->internalWidth, window->internalHeight);
	}

	// This function is used for updating the camera while editing a level.
	void update_camera(){
		Camera *camera = &Game::camera;
		Window *w = camera->renderer->window;
		MouseInfo mouse = Game::mouse;
		if(!camera->renderer){
			printf("No renderer attached to the camera\n");
			return;	
		} 

		if(is_key_being_pressed(w, GLFW_KEY_D)){
			camera->position.x += CAMERA_SPEED * Game::dt;
			set_camera_position();
		}
		if(is_key_being_pressed(w, GLFW_KEY_A)){
			camera->position.x -= CAMERA_SPEED * Game::dt;
			set_camera_position();
		}
		if(is_key_being_pressed(w, GLFW_KEY_W)){
			camera->position.y += CAMERA_SPEED * Game::dt;
			set_camera_position();
		}
		if(is_key_being_pressed(w, GLFW_KEY_S)){
			camera->position.y -= CAMERA_SPEED * Game::dt;
			set_camera_position();
		}

		if(mouse.wheel == ScrollWheelState::WHEEL_FORWARDS){
			zoom();
		}
		else if (mouse.wheel == ScrollWheelState::WHEEL_BACKWARDS){
			zoom(true);
		}
	}
}