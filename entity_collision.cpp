#include "entity.h"
#include <stdio.h>
// #define ENTITY_COLLISION(Entity, function) \
	// void entity_collision(Entity *e){\
		// if(e->collision_box->collision_info.collided == true){\
			// function\
			// e->collision_box->collision_info.collided = false;\
		// }\
	// }\

	
void collision_between(Player *player, Enemy *enemy, V2 *penetration){
	printf("COLLISION WITH ENEMY\n");
}