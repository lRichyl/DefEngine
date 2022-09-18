#pragma once
#include "math.h"
#include "entity.h"
#include "def_array.h"
// #include "level_editor.h"
// struct Level;

namespace def {
	bool DoRectsCollide(Rect b1, Rect b2, V2 *penetration);
	bool DoRectContainsPoint(Rect b, V2 point);

	void check_collisions(EntityManager *entity_manager);

	template<typename T>
	void check_collision_between_player_and_entities(Player *player, T *entities){
		for(int i = 0; i < ENTITIES_PER_TYPE; i++){
			if(!entities[i].is_active) continue;
			V2 penetration;
			T *e = &entities[i];
			if(DoRectsCollide(player->bounding_box, e->bounding_box, &penetration)){
				collision_between(player, e, &penetration);
			}
			
		}
	}

	void check_collision_between_player_and_collision_regions(Player *player, Collider *collision_regions);
}