#include "sahsplitcandidate.h"

void Testprefectsplit(CuTest *tc) {
	AABB aabb;
	aabb[0] = aabb[1] = aabb[2] = 0.0f;
	aabb[3] = aabb[4] = aabb[5] = 1.0f;
	Triangle t;
	t[0] = t[3] = t[6] = 0.0f;
	t[1] = t[4] = 0.0f; 
	t[7] = 1.0f;;
	t[2] = t[5] = 1.0f;
	t[8] = 0.5f;
	SAHSplitCandidate sahc;
	sahc.axis = 1;
	sahc.plane = 0.5f;
	AABB aabbs[2];

}
