#include "collision.h"
#include <cmath>
#include <stdio.h>
#include <vector>
#include <cfloat>
// #include "entity.h"

//Collision check using the minkowski difference method
bool DoRectsCollide(Rect b1, Rect b2, V2 *penetration){
	float mdtop   =  b1.y - (b2.y - b2.h);
	float mdbott  =  (b1.y - b1.h) - b2.y;
	float mdleft  =  b1.x - (b2.x + b2.w);
	float mdright =  (b1.x + b1.w) - b2.x;

	if(mdright >= 0 && mdleft <= 0 && mdtop >= 0 && mdbott <= 0){
		// std::cout << "collided" << std::endl;


		float min = FLT_MAX;

		if(std::abs(mdright) < min){
			min = std::abs(mdright);
			penetration->x = mdright;
			penetration->y = 0.f;
		}
		if(std::abs(mdleft) < min){
			min = std::abs(mdleft);
			penetration->x = mdleft;
			penetration->y = 0.f;
		}
		if(std::abs(mdbott) < min){
			min = std::abs(mdbott);
			penetration->x = 0.f;
			penetration->y = mdbott;
		}
		if(std::abs(mdtop) < min){
			min = std::abs(mdtop);
			penetration->x = 0.f;
			penetration->y = mdtop;
		}

		// std::cout << "collision" << std::endl;
		return true;
	}
	penetration->x = 0;
	penetration->y = 0;
	return false;


}

bool DoRectContainsPoint(Rect b, V2 point){
	if(point.x <= b.x + b.w && point.x >= b.x && point.y <= b.y && point.y >= b.y - b.h){
		return true;
	}else{
		return false;
	}
}


static std::vector<Rect> collision_boxes;

void check_collisions(EntityManager *em){
	check_collision_between_player_and_entities(&em->player, &em->enemies);
}

// Rect*& set_collision_box(EntityType type, void *entity){
	// Rect r;
	// r.collision_info.origin.type = type;
	// r.collision_info.origin.entity = entity;
	
	
	// collision_boxes.push_back(r);
	// Rect *rect = (Rect*)&collision_boxes.back();
	// printf("%d\n", rect->collision_info.origin.type);
	// return collision_boxes.back();
// }
