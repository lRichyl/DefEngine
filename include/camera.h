#pragma once
#include "renderer.h"

struct Camera{
	Renderer *renderer;
	V2 position;
	glm::mat4 view;
};

void init_camera(Camera *camera, Renderer *renderer);
void get_world_position(Camera *camera, V2 position);// NEXT.
void set_camera_position(Camera *camera, V2 position);