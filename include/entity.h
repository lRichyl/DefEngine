#pragma once
#include "math.h"
#include "def_array.h"
#include "sprite.h"

#define ENTITIES_PER_TYPE 50
#define MAX_COLLIDERS 100
#define MAX_TILES_PER_LEVEL 50000 // 10,000 tiles per layer.

#define GET_AVAILABLE_ENTITY(entity_list, entity_manager) {\
	int i = 0;\
	for(i; i < ENTITIES_PER_TYPE; i++){ \
		if(!entity_manager->entity_list[i].is_active){\
			e = &entity_manager->entity_list[i];\
			e->id = i;\
			break;\
		} \
	}\
	if (i == ENTITIES_PER_TYPE)\
		printf("No remaining %s entities\n", #entity_list);\
}

#define ADD_ENTITY(entity_list, type)  \
	type *e;\
	GET_AVAILABLE_ENTITY(entity_list, em);\
	e->is_active = true;\
	e->position = position;\
	e->layer = layer;\
	init_entity(e);\
	return e;\

#define ELIMINATE_ENTITY(entity_list)\
	em->entity_list[id].is_active = false;\


struct Renderer;
struct LevelEditor;

enum EntityType{
	ENTITY_NONE,
	ENTITY_TILE,
	ENTITY_COLLIDER,
	ENTITY_PLAYER,
	ENTITY_SLIME,
	ENTITY_AMOUNT
};

enum AreaType{
	AREA_NONE,
	AREA_SINGLE,
	AREA_MULTI
};

// struct Icon{
// 	Sprite sprite;
// 	// Rect bounding_box;
// };

struct Entity{
	EntityType type      = EntityType::ENTITY_NONE;
	// AreaType   area_type = AreaType::AREA_NONE;
	bool is_active = false;
	bool is_disabled = false;
	Rect bounding_box;
	V2 position;
	// V2 area;
	Sprite icon;
	// Texture texture;
	int id = -1;
	int layer = -1;
	
	// This function pointer is set when a placed entity requires extra configuration, like setting a relantionship with another entity, 
	// setting text, etc.
	void (*special_placement)() = NULL;
};


// Entity* get_selection_entity(EntitySelection *selection);

struct Collider : public Entity{
	Sprite sprite;
};

void init_entity(Collider   *collider);
// void update_collider(Collider *collider, Renderer *renderer);
void render_collider(Collider *collider, Renderer *renderer);

struct Tile : public Entity{
	Tile(){
		type = EntityType::ENTITY_TILE;
		// area_type = AreaType::AREA_SINGLE;
		// area = {1,1};
	}
	Sprite sprite;
};

struct TileSpecifier : public Entity{
	Tile *tile = NULL;
	V2 position;
};

void init_entity(TileSpecifier *tile_e);

struct Player : public Entity{
	float speed;
	V2 direction;
	Sprite sprite;
	bool is_on_level = false;
};

void init_entity(Player *player);
void update_player(Player *player, Renderer *renderer);
void render_player(Player *player, Renderer *renderer);

struct Slime : public Entity{
	Sprite sprite;
};

void init_entity(Slime *slime);
void update_entity(Slime *slime, Renderer *renderer);
void render_entity(Slime *slime, Renderer *renderer);

struct Multi : public Entity{
	Sprite sprite;
};

void init_entity(Multi *multi);
void update_multi(Multi *multi, Renderer *renderer);
void render_multi(Multi *multi, Renderer *renderer);
	
struct EntityManager{
	Player player;
	Slime slimes[ENTITIES_PER_TYPE];

	TileSpecifier tiles[MAX_TILES_PER_LEVEL];
	// When adding a new entity type DO NOT FORGET to call the UPDATE_ENTITES, RENDER_ENTITIES and CLEAR_ENTITIES macros for that entity type in 
	// the entity manager's update, render and clear functions.
	
	// The entities in this array have to be sorted according to their Y position in descending order and cleared at the end after drawing them.
	// Not every entity will be here, only the entities that need to be sorted, like any moving entities (enemies, habilities, etc).
	// DefArray<Entity*> entities_draw_list;
	// TODO:     !!!!!DO NOT FORGET TO CALL clear_array() FOR THIS LIST AFTER DRAWING !!!!!!!!!!
	Collider collision_regions[MAX_COLLIDERS]; // This will be used for collision with solid objects.

	DefArray<Tile*> tiles_prototypes;
	DefArray<Entity*> entities_prototypes;
};

Entity* add_entity(EntityType type, EntityManager *em, V2 position, int layer);
void eliminate_entity(EntityManager *em, EntityType type ,int id);
Entity* get_entity_prototype(EntityManager *em, EntityType type);
DefArray<RenderCommand>* get_render_list_for_layer(int layer);
void init_entity_manager(EntityManager *em);
void update_entities(EntityManager *em, Renderer *renderer);
void render_entities(EntityManager *em, Renderer *renderer);
void render_colliders(EntityManager *em, Renderer *renderer);
void clear_entity_manager(EntityManager *em);

// template<typename T>
// T cast_and_position_entity(Entity *e, V2 position){
// 	T *entity_T = (T*)e;
// 	T new_T = *entity_T;
// 	new_T.position = position;
// 	return new_T;
// }

void update_bounding_box(Entity *entity);

// inline void add_enemy(EntityManager *em, Enemy *e){
	// add_array(&em->enemies, *e);
	// Entity *entity = get_last_element(&em->enemies);
	// add_array(&em->entities_draw_list, entity);
// }


////COLLISIONS
void collision_between(Player *player, Slime *slime, V2 *penetration);
// void collision_between_player_and_collision_regions(Level *level, Player *player);
// void collision_between(Player *player, )

