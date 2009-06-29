#include <stdio.h>
#include "sah.h"

float SAH(float plane, uint32_t axis, AABB *aabb, uint32_t left, uint32_t right, uint32_t *terminate) {
	AABB *aabbleft;
	AABB *aabbright;
	subdivideAABB(aabb, plane, axis, &aabbleft, &aabbright);
	float totalsurface = surfaceArea(aabb);
	float leftsurface = surfaceArea(aabbleft);
	float rightsurface = surfaceArea(aabbright);

	aabbFree(aabbleft);
	aabbFree(aabbright);

	*terminate = 0;

	return (left * leftsurface + right * rightsurface) / totalsurface;
}
