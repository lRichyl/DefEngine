#ifndef COLLISION_H
#define COLLISION_H
#include "renderer.h"
#include "math.h"


bool DoRectsCollide(Rect b1, Rect b2, V2 *penetration);
bool DoRectContainsPoint(Rect b, V2 point);
// void Bounce(Rect *boundingBox, V2 *velocity, V2 penetration);

#endif
