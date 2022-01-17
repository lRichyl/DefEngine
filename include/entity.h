#pragma once
#include "math.h"
#include <vector>

// struct Rect;

enum EntityType{
	ENTITY_NONE,
	ENTITY_PLAYER
};

struct Entity{
	EntityType type = EntityType::ENTITY_NONE;
	V2 position;
};



struct Player : public Entity{
	Player();
	Rect collision_box;
};

struct Enemy : public Entity{
	Rect collision_box;
};

// #define ENTITY_COLLISION(Entity, function) \
	// void entity_collision(Entity *e){\
		// if(e->collision_box->collision_info.collided == true){\
			// function\
			// e->collision_box->collision_info.collided = false;\
		// }\
	// }\
	
struct EntityManager{
	Player player;
	std::vector<Enemy> enemies;
};

inline void add_enemy(EntityManager *em, Enemy *e){
	em->enemies.push_back(*e);
}

////COLLISIONS
void collision_between(Player *player, Enemy *enemy, V2 *penetration);

