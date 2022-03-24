#pragma once
#include "renderer.h"

struct Camera{
	Renderer *renderer;
	V2 position;
	glm::mat4 view;
	bool moved = false;
	float speed = 50;
	V2 movement;
};

void init_camera(Camera *camera, Renderer *renderer);
V2 get_world_position(Camera *camera, V2 position);
void set_camera_position(Camera *camera, V2 position);