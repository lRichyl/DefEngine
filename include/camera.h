#pragma once
#include "renderer.h"

namespace def {
	struct Camera{
		Renderer *renderer;
		VEC_2D position;
		glm::mat4 view;
		bool moved = false;
		float speed = 50;
		float zoom_factor = 0.02f;
		float zoom = 1;
		VEC_2D size;
		VEC_2D movement;
	};

	void init_camera(Camera *camera, Renderer *renderer);
	VEC_2D get_world_position(VEC_2D position);
	VEC_2D get_screen_position(VEC_2D world_pos);
	void set_camera_position();
	void zoom(bool zoom_out = false);
	void reset_camera(Camera *camera);
	void update_camera();
}