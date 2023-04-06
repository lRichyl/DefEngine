#pragma once
#include "math.h"
#include "entity.h"
#include "def_array.h"
// #include "level_editor.h"
// struct Level;

namespace def {
	bool DoRectsCollide(RECT b1, RECT b2, VEC_2D *penetration);
	bool DoRectContainsPoint(RECT b, VEC_2D point);

	void check_collisions(EntityManager *entity_manager);

	template<typename T>
	void check_collision_between_player_and_entities(Player *player, T *entities){
		for(int i = 0; i < ENTITIES_PER_TYPE; i++){
			if(!entities[i].is_active) continue;
			VEC_2D penetration;
			T *e = &entities[i];
			if(DoRectsCollide(player->bounding_box, e->bounding_box, &penetration)){
				collision_between(player, e, &penetration);
			}
			
		}
	}

	void check_collision_between_player_and_collision_regions(Player *player, Collider *collision_regions);
}