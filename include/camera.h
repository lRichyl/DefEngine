#pragma once
#include "renderer.h"

struct Camera{
	Renderer *renderer;
	V2 position;
	glm::mat4 view;
	bool moved = false;
	float speed = 50;
	float zoom_factor = 0.02f;
	float zoom = 1;
	V2 size;
	V2 movement;
};

void init_camera(Camera *camera, Renderer *renderer);
V2 get_world_position(V2 position);
V2 get_screen_position(V2 world_pos);
void set_camera_position();
void zoom(bool zoom_out = false);
void reset_camera(Camera *camera);
void update_camera();