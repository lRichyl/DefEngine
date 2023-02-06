#include "entity.h"
#include "console.h"
#include <stdio.h>

void collision_between(Player *player, Slime *slime, V2 *penetration){
	// printf("Hello\n");
}

void collision_between(Player *player, LevelSelector *lvl_sel, V2 *penetration){
	console_load_level(lvl_sel->level_name);
}