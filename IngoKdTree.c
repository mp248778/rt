#include "IngoKdTree.h"
#include "event.h"
#include "aabb.h"
#include "triangle.h"
#include "sah.h"
#include "sahsplitcandidate.h"

#include "IngoKdTreeInternal.h"



igKdTree* igBuild(Triangle *t, const unsigned tcnt) {
	igEvents *es = igCreateEvents(t, tcnt);
	AABB *aabb = getAABB(t, tcnt);
	igTriangleData *igdt = igPrepareTriangleData(t, tcnt);

	igKdTree *root = igCreateTree(t, igdt, aabb, es);

	igTriangleDataFree(igdt);
	aabbFree(aabb);
	igEventsFree(es);

	return root;
}

igKdTree* igCreateTree(Triangle *t, igTriangleData *igdt, AABB *aabb, igEvents *es) {
	igKdTree *node;
	SAHSplitCandidate *split = igFindSAHSplitCandidate(t, aabb, es);

	if(split->terminate) {
		node = igCreateLeaf(t, es);
	} else {
		igClassifyTriangles(t, igdt, es, split);

		igEvents *esleft, *esright;
		igSpliceEvents(t, igdt, &es, &esleft, &esright, aabb, split);

		igEvents *nesleft, *nesright;
		igGenerateNewEvents(t, igdt, &es, &nesleft, &nesright, aabb, split);

		igSortEvents(nesleft);
		igSortEvents(nesright);
		esleft = igMergeEvents(esleft, nesleft);
		esright = igMergeEvents(esright, nesright);
	
		AABB *aabbleft, *aabbright;
		subdivideAABB(aabb, split->plane, split->axis, &aabbleft, &aabbright);

		node = igCreateInternalNode(split, 
							igCreateTree(t, igdt, aabbleft, esleft), 
							igCreateTree(t, igdt, aabbright, esright));
	}
	SAHSplitCandidateFree(split);
	igEventsFree(es);
	aabbFree(aabb);
	return node;
}

SAHSplitCandidate* igFindSAHSplitCandidate(Triangle *t, AABB *aabb, igEvents *es) {
	SAHSplitCandidate *split = initialSAHSplitCandidate();
	const unsigned tcnt = igCountTriangles(es);

	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		unsigned NL = 0, NP = 0, NR = tcnt;
		igEvent *eit = *es[axis];
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
			SAHSplitCandidate *left = newSAHSplitCandidate(axis, plane, NL + NP, NR);
			SAHSplitCandidate *right = newSAHSplitCandidate(axis, plane, NR, NR + NP);
			split = SAHChooseBetter(split, right);
			split = SAHChooseBetter(split, left);
		}
	}
	return split;
}

void igClassifyTriangles(Triangle *t, igTriangleData *igdt, igEvents *es, SAHSplitCandidate *split) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = *es[axis];
		while(eit) {
			igdt[eit->trid] = IGBOTH;
			eit = eit->next;
		}
	}
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = *es[axis];
		while(eit) {
			if(eit->type == IGEND && eit->plane < split->plane) igdt[eit->trid] = IGLEFT_ONLY;
			else if(eit->type == IGBEGIN && eit->plane > split->plane) igdt[eit->trid] = IGRIGHT_ONLY;
			eit = eit->next;
		}
	}
}
