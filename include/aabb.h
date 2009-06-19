#ifndef AABB_HEADER
#define AABB_HEADER

#include "triangle.h"

typedef float AABB[6];

AABB* getAABB(Triangle *, unsigned);
void aabbFree(AABB*);
void subdivideAABB(AABB*, float, unsigned, AABB**, AABB**);
float surfaceArea(AABB*);


#endif
