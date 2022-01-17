#include "entity.h"
#include "collision.h"

Player::Player(){
	type = EntityType::ENTITY_PLAYER;
	// collision_box = set_collision_box(type, this);
}