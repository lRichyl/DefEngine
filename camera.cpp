#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"

void init_camera(Camera *camera, Renderer *renderer){
	camera->renderer = renderer;
	camera->position = {0,0};
	camera->size     = {renderer->window->internalWidth, renderer->window->internalHeight};
}

void set_camera_position(Camera *camera, V2 position){
	camera->position = position;
	// When we move the camera we change the view matrix for all the batches that are using the default shader. 
	// If we for example wanna use a special shader for the player we have to update its view matrix to the same as the default matrix. 
	
	// Turn this into a function when we need to update other shaders.
	ShaderProgram shader = camera->renderer->default_shader_program;
	glUseProgram(shader.id);
	
	camera->view = glm::translate(glm::mat4(1.0f), glm::vec3(camera->position.x, camera->position.y, 0.0f));
	int view_uniform_id = glGetUniformLocation(shader.id, ("u_view"));
	glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(camera->view));
	
	glUseProgram(0);
	
	camera->moved = true;
}

void zoom(Camera *camera, bool zoom_out){
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

V2 get_world_position(Camera *camera, V2 position){
	V2 world_pos;
	V2 scale  = {camera->size.x / camera->renderer->window->internalWidth, camera->size.y / camera->renderer->window->internalHeight};
	position.x *= scale.x;
	position.y *= scale.y;
	world_pos = {position.x - camera->position.x, position.y - camera->position.y};
	
	// printf("%f, %f : %f, %f\n", world_pos.x, world_pos.y, scale.x, scale.y);
	return world_pos;
}

void reset_camera(Camera *camera){
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
	
	camera->size = {window->internalWidth, window->internalHeight};
}