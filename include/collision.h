#pragma once
#include "math.h"
#include "entity.h"
#include <vector>

// struct EntityCollisionInfo{
	// EntityType type = EntityType::ENTITY_NONE;
	// void *entity;
// };

// struct CollisionInfo{
	// EntityCollisionInfo origin;
	// EntityCollisionInfo target;
	
	// bool collided = false;
// };

// struct Player;
// struct Enemy;







bool DoRectsCollide(Rect b1, Rect b2, V2 *penetration);
bool DoRectContainsPoint(Rect b, V2 point);

void check_collisions(EntityManager *entity_manager);

template<typename T>
void check_collision_between_player_and_entities(Player *player, std::vector<T> *entities){
	for(int i = 0; i < entities->size(); i++){
		V2 penetration;
		if(DoRectsCollide(player->collision_box, (*entities)[i].collision_box, &penetration)){
			collision_between(player, &(*entities)[i], &penetration);
		}
		
	}
}
// Rect*& set_collision_box(EntityType type, void *entity);

// template<typename T>
// void entity_collision(T *e){
	// if(e->collision_box->collision_info.collided == true){
		// collision_response(e);
		// e->collision_box->collision_info.collided = false;
	// }
// }