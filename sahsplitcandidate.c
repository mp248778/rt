#include <stdlib.h>
#include "sahsplitcandidate.h"
#include "sah.h"
#include "util.h"


void SAHSplitCandidateFree(SAHSplitCandidate *sahc) {
	free(sahc);
}

SAHSplitCandidate* igFindSAHSplitCandidate(Triangle *t, AABB *aabb, igEvents *es) {
	SAHSplitCandidate *split = initialSAHSplitCandidate();
	const unsigned tcnt = igCountTriangles(es);

	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		unsigned NL = 0, NP = 0, NR = tcnt;
		igEvent *eit = (*es)[axis];
		while(eit) {
			float plane = eit->plane;
			unsigned starts = 0, ends = 0, planars = 0;
			while(eit && eit->plane == plane && eit->type == IGEND) {
				ends++; eit = eit->next;
			}
			while(eit && eit->plane == plane && eit->type == IGPLANAR) {
				planars++; eit = eit->next;
			}
			while(eit && eit->plane == plane && eit->type == IGBEGIN) {
				starts++; eit = eit->next;
			}
			NP = planars;
			NR -= ends + planars;
			SAHSplitCandidate *left = newSAHSplitCandidate(axis, plane, aabb, NL + NP, NR);
			SAHSplitCandidate *right = newSAHSplitCandidate(axis, plane, aabb, NR, NR + NP);
			split = SAHChooseBetter(split, right);
			split = SAHChooseBetter(split, left);
		}
	}
	return split;
}

SAHSplitCandidate* initialSAHSplitCandidate() {
	SAHSplitCandidate *sahc = malloc(sizeof(SAHSplitCandidate));
	sahc->axis = 4;
	return sahc;
}

SAHSplitCandidate* newSAHSplitCandidate(unsigned axis, float plane, AABB *aabb, unsigned NL, unsigned NR) {
	SAHSplitCandidate *sahc = malloc(sizeof(SAHSplitCandidate));
	sahc->axis = axis;
	sahc->plane = plane;
	sahc->cost = SAH(plane, aabb, NL, NR, &sahc->terminate);
	return sahc;
}

SAHSplitCandidate* SAHChooseBetter(SAHSplitCandidate *sahc1, SAHSplitCandidate *sahc2) {
	if(sahc1->axis == 4 || sahc1->cost > sahc2->cost) {
		SAHSplitCandidateFree(sahc1);
		return sahc2;
	}
	SAHSplitCandidateFree(sahc2);
	return sahc1;
}

static void swap(float **a, float **b) {
	float *tmp = *a;
	*a = *b;
	*b = tmp;
}

void calcNewPoints(float *A, float *B, float *p, float plane, unsigned axis) {
	float d = (A[axis] - plane)/(A[axis] - B[axis]);
	unsigned i;
	p[axis] = plane;
	for(i = 0; i < 3; i++) {
		if(i == axis) continue;
		p[i] = d * (B[i] - A[i]) + A[i];
	}
}

void perfectsplit(AABB *aabb, Triangle *t, AABB aabbs[2], SAHSplitCandidate *sahc) {
	unsigned axis = sahc->axis;
	float *idx[3];
	unsigned i;
	float *A = *t, *B = *t + 3, *C = *t + 6;
	if(A[axis] > B[axis]) swap(A,B);		//sort
	if(B[axis] > C[axis]) swap(B,C);
	if(A[axis] > B[axis]) swap(A,B);
	if(B[axis] < sahc->plane) swap(A,C);	//leave A at one side, B and C at the other one

	float np[6];
	calcNewPoints(A, B, np, sahc->plane, axis);
	calcNewPoints(A, B, np + 3, sahc->plane, axis);
	for(i = 0; i < 3; i++) {
		float minp = minv(3, A[i], np[i], np[i + 3]);
		float maxp = maxv(3, A[i], np[i], np[i + 3]);
		aabbs[0][i] = minp;
		aabbs[0][i + 3] = maxp;
	}
	for(i = 0; i < 3; i++) {
		float minp = minv(4, B[i], C[i], np[i], np[i + 3]);
		float maxp = maxv(4, B[i], C[i], np[i], np[i + 3]);
		aabbs[1][i] = minp;
		aabbs[1][i + 3] = maxp;
	}
	if(aabbs[0][axis] > aabbs[1][axis]) {
		for(i = 0; i < 6; i++) {
			float tmp = aabbs[0][i];
			aabbs[0][i] = aabbs[1][i];
			aabbs[1][i] = tmp;
		}
	}
}
