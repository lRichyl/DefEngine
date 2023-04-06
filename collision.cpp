#include "collision.h"
#include "level_editor.h"
#include <cmath>
#include <stdio.h>
#include <vector>
#include <cfloat>
// #include "entity.h"

namespace def {
	//Collision check using the minkowski difference method
	bool DoRectsCollide(RECT b1, RECT b2, VEC_2D *penetration){
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

	bool DoRectContainsPoint(RECT b, VEC_2D point){
		if(point.x <= b.x + b.w && point.x >= b.x && point.y <= b.y && point.y >= b.y - b.h){
			return true;
		}else{
			return false;
		}
	}



	void check_collision_between_player_and_collision_regions(Player *player, Collider *collision_regions){
		for(int i = 0; i < ENTITIES_PER_TYPE; i++){
			if(!collision_regions[i].is_active) continue;
			VEC_2D penetration;
			RECT col_reg = collision_regions[i].bounding_box;
			// printf("Col regs: %f, %f, %f, %f\n", col_reg.x, col_reg.y, col_reg.w, col_reg.h);

			if(DoRectsCollide(col_reg, player->bounding_box, &penetration)){
				player->position.x += penetration.x;
				player->position.y += penetration.y;
				update_bounding_box(player);
			}
		}
	}

	void check_collisions(EntityManager *em){
		if(em->player.is_on_level){
			check_collision_between_player_and_entities(&em->player, em->slimes);
			check_collision_between_player_and_collision_regions(&em->player, em->collision_regions);
		}
	}
}