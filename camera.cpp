#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"

void init_camera(Camera *camera, Renderer *renderer){
	camera->renderer = renderer;
	camera->position = {0,0};
}

void set_camera_position(Camera *camera, V2 position){
	camera->position = position;
	ShaderProgram shader = camera->renderer->default_shader_program;
	glUseProgram(shader.id);
	
	camera->view = glm::translate(glm::mat4(1.0f), glm::vec3(camera->position.x, camera->position.y, 0.0f));
	int view_uniform_id = glGetUniformLocation(shader.id, ("u_view"));
	glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(camera->view));
	
	glUseProgram(0);
}