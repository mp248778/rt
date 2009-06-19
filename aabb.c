#include <stdlib.h>
#include "aabb.h"
#include "utils.h"

AABB* getAABB(Triangle *t, unsigned tcnt) {
	AABB *aabb = malloc(sizeof(AABB));
	unsigned i;
	for(i = 0; i < 3; i++)
		(*aabb)[i + 3] = (*aabb)[i] = t[0][i];

	for(i = 0; i < tcnt; i++) {
		unsigned axis;
		for(axis = 0; axis < 3; axis++) {
			float min = minv(3, t[i][axis], t[i][axis + 3], t[i][axis + 6]);
			float max = maxv(3, t[i][axis], t[i][axis + 3], t[i][axis + 6]);
			if((*aabb)[axis] > min) (*aabb)[axis] = min;
			if((*aabb)[axis + 3] < max) (*aabb)[axis + 3] = max;
		}
	}
	return aabb;
}

void aabbFree(AABB *aabb) {
	free(aabb);
}

void subdivideAABB(AABB *aabb, float plane, unsigned axis, AABB **left, AABB **right) {
	(*left) = malloc(sizeof(AABB));
	(*right) = malloc(sizeof(AABB));
	unsigned i;
	for(i = 0; i < 6; i++)
		(**left)[i] = (**right)[i] = (*aabb)[i];
	(**left)[3 + axis] = plane;
	(**right)[axis] = plane;
}

float surfaceArea(AABB *aabb) {
	float a = (*aabb)[3] - (*aabb)[0];
	float b = (*aabb)[4] - (*aabb)[1];
	float c = (*aabb)[5] - (*aabb)[2];

	return 2.0f * (a * b + b * c + a * c);
}

