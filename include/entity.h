#pragma once
#include "math.h"
#include "def_array.h"
#include "sprite.h"
#include <vector>

struct PrototypeList;

struct EntitySelection{
	int entity_index = -1;
	PrototypeList *prototype_list;
};

enum EntityType{
	ENTITY_NONE,
	ENTITY_COLLIDER,
	ENTITY_TILE,
	ENTITY_PLAYER,
	ENTITY_SLIME
};

struct Icon{
	Sprite sprite;
	Rect bounding_box;
};

struct Entity{
	EntityType type = EntityType::ENTITY_NONE;
	Rect bounding_box;
	V2 position;
	V2 area;
	Icon icon;
	Texture texture;
	int prototype_id = -1;
};

struct Collider : public Entity{
	Sprite sprite;
};

void init_collider(Collider   *collider, V2 icon_size);
// void update_collider(Collider *collider, Renderer *renderer);
// void render_collider(Collider *collider, Renderer *renderer);

struct Tile : public Entity{
	Tile(){
		type = EntityType::ENTITY_TILE;
		area = {1,1};
	}
	Sprite sprite;
};

struct Player : public Entity{
	float speed;
	V2 direction;
	Sprite sprite;
};

void init_player(Player *player, V2 icon_size);
void update_player(Player *player, Renderer *renderer);
void render_player(Player *player, Renderer *renderer);

struct Slime : public Entity{
	Sprite sprite;
};

void init_slime(Slime *slime, V2 icon_size);
void update_slime(Slime *slime, Renderer *renderer);
void render_slime(Slime *slime, Renderer *renderer);

	
struct EntityManager{
	Player player;
	DefArray<Slime> slimes;
	// When adding a new array of an entity type do not forget to init the array in the entity manager init function.
	
	// The entities in this array have to be sorted according to their Y position in descending order and cleared at the end after drawing them.
	// Not every entity will be here, only the entities that need to be sorted, like any moving entities (enemies, habilities, etc).
	DefArray<Entity*> entities_draw_list;
	// TODO:     !!!!!DO NOT FORGET TO CALL clear_array() FOR THIS LIST AFTER DRAWING !!!!!!!!!!
	DefArray<Rect> collision_regions; // This will be used for collision with solid objects.
};

void init_entity_manager(EntityManager *em);
void update_entities(EntityManager *em, Renderer *renderer);
void render_entities(EntityManager *em, Renderer *renderer);
void clear_entity_manager(EntityManager *em);

template<typename T>
T cast_and_position_entity(Entity *e, V2 position){
	T *entity_T = (T*)e;
	T new_T = *entity_T;
	new_T.position = position;
	return new_T;
}

void inline update_position(Entity *entity){
	entity->bounding_box.x = entity->position.x;
	entity->bounding_box.y = entity->position.y;
}

// inline void add_enemy(EntityManager *em, Enemy *e){
	// add_array(&em->enemies, *e);
	// Entity *entity = get_last_element(&em->enemies);
	// add_array(&em->entities_draw_list, entity);
// }


////COLLISIONS
void collision_between(Player *player, Slime *slime, V2 *penetration);
// void collision_between_player_and_collision_regions(Level *level, Player *player);
// void collision_between(Player *player, )

