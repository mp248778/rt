#include "sah.h"

float SAH(float plane, unsigned axis, AABB *aabb, unsigned left, unsigned right, unsigned *terminate) {
	AABB *aabbleft;
	AABB *aabbright;
	subdivideAABB(aabb, plane, axis, &aabbleft, &aabbright);
	float totalsurface = surfaceArea(aabb);
	float leftsurface = surfaceArea(aabbleft);
	float rightsurface = surfaceArea(aabbright);

	aabbFree(aabbleft);
	aabbFree(aabbright);

//	printf("left=%i leftsurf=%f right=%i rightsurf=%f totalsurf=%f cost=%f\n", left, leftsurface, right, rightsurface, totalsurface,
//		(left * leftsurface + right * rightsurface) / totalsurface);


	return (left * leftsurface + right * rightsurface) / totalsurface;
}

