#include <stdlib.h>
#include <stdio.h>
#include "sahsplitcandidate.h"
#include "sah.h"
#include "utils.h"
#include "printer.h"


void SAHSplitCandidateFree(SAHSplitCandidate *sahc) {
	free(sahc);
}

SAHSplitCandidate* igFindSAHSplitCandidate(Triangle *t, AABB *aabb, igEvents *es) {
	SAHSplitCandidate *split = initialSAHSplitCandidate();
	const uint32_t tcnt = igCountTriangles(es);

	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		uint32_t NL = 0, NP = 0, NR = tcnt;
		igEvent *eit = (*es)[axis]->next;
		while(eit) {
			float plane = eit->plane;
			uint32_t starts = 0, ends = 0, planars = 0;
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
			SAHSplitCandidate *left = newSAHSplitCandidate(axis, plane, aabb, NL + NP, NR, tcnt);
			SAHSplitCandidate *right = newSAHSplitCandidate(axis, plane, aabb, NL, NR + NP, tcnt);
			split = SAHChooseBetter(split, right);
			split = SAHChooseBetter(split, left);

			NL += starts + planars;
		}
	}
	printSAHSplitCandidate(split);
	return split;
}

SAHSplitCandidate* initialSAHSplitCandidate() {
	SAHSplitCandidate *sahc = malloc(sizeof(SAHSplitCandidate));
	sahc->axis = 4;
	sahc->terminate = 1;	//if tcnt == 0
	return sahc;
}

SAHSplitCandidate* newSAHSplitCandidate(uint32_t axis, float plane, AABB *aabb, uint32_t NL, uint32_t NR, uint32_t tcnt) {
	SAHSplitCandidate *sahc = malloc(sizeof(SAHSplitCandidate));
	sahc->axis = axis;
	sahc->plane = plane;
	sahc->cost = SAH(plane, axis, aabb, NL, NR, &sahc->terminate);
	if  (
		((*aabb)[axis] >= plane || (*aabb)[axis + 3] <= plane)	||
		(tcnt == 0) 											||
		(sahc->cost < 0.001f * ((float)tcnt))
		)
	{
			sahc->terminate = 1;
	}

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

void calcNewPoints(float *A, float *B, float *p, float plane, uint32_t axis) {
	float d = (A[axis] - plane)/(A[axis] - B[axis]);
	uint32_t i;
	p[axis] = plane;
	for(i = 0; i < 3; i++) {
		if(i == axis) continue;
		p[i] = d * (B[i] - A[i]) + A[i];
	}
}

void perfectsplit(AABB *aabb, Triangle *t, AABB aabbs[2], SAHSplitCandidate *sahc) {
	uint32_t axis = sahc->axis;
	uint32_t i;
	float *A = *t, *B = *t + 3, *C = *t + 6;
	if(A[axis] > B[axis]) swap(&A,&B);		//sort
	if(B[axis] > C[axis]) swap(&B,&C);
	if(A[axis] > B[axis]) swap(&A,&B);
	if(B[axis] < sahc->plane) swap(&A,&C);	//leave A at one side, B and C at the other one

	float np[6];
	calcNewPoints(A, B, np, sahc->plane, axis);		//intersection points
	calcNewPoints(A, C, np + 3, sahc->plane, axis);
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
