#include "math.h"

void V2::SwitchXComponents(V2 *a, V2 *b){
     float temporalX = a->x;

     a->x = b->x;
     b->x = temporalX;
}

void V2::SwitchYComponents(V2 *a, V2 *b){
     float temporalY = a->y;

     a->y = b->y;
     b->y = temporalY;
}
