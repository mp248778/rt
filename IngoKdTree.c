#include <stdlib.h>
#include "IngoKdTree.h"
#include "event.h"
#include "aabb.h"
#include "sah.h"
#include "sahsplitcandidate.h"
#include "printer.h"

#include "IngoKdTreeInternal.h"

igKdTree* igBuild(Triangle *t, const unsigned tcnt) {
	igEvents *es = igCreateEvents(t, tcnt);
	AABB *aabb = getAABB(t, tcnt);
	igTriangleData *igdt = igPrepareTriangleData(t, tcnt);

	igKdTree *root = igCreateTree(t, igdt, aabb, es);

	igTriangleDataFree(igdt);
	return root;
}

igKdTree* igCreateTree(Triangle *t, igTriangleData *igdt, AABB *aabb, igEvents *es) {
	igKdTree *node;
	SAHSplitCandidate *split = igFindSAHSplitCandidate(t, aabb, es);

//	printSAHSplitCandidate(split);

	if(split->terminate) {
		node = igCreateLeaf(t, es);
	} else {
		igClassifyTriangles(t, igdt, es, split);

		igEvents *esleft, *esright;
		igSpliceEvents(t, igdt, &es, &esleft, &esright, aabb);

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
	aabbFree(aabb);
	return node;
}


void igClassifyTriangles(Triangle *t, igTriangleData *igdt, igEvents *es, SAHSplitCandidate *split) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (*es)[axis];
		while(eit) {
			igdt[eit->trid] = IGBOTH;
			eit = eit->next;
		}
	}
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (*es)[axis];
		while(eit) {
			if(eit->type == IGEND && eit->plane < split->plane) igdt[eit->trid] = IGLEFT_ONLY;
			else if(eit->type == IGBEGIN && eit->plane > split->plane) igdt[eit->trid] = IGRIGHT_ONLY;
			eit = eit->next;
		}
	}
}

igKdTree* igCreateInternalNode(SAHSplitCandidate *sahc, igKdTree *left, igKdTree *right) { 
	igKdTree *node = malloc(sizeof(igKdTree));
	node->plane = sahc->plane;
	node->left = left;
	node->right = right;
	node->axis = sahc->axis;
	node->tcnt = ~0u;
	node->trids = NULL;
	return node;
}

igKdTree* igCreateLeaf(Triangle *t, igEvents *es) {
	igKdTree *leaf = malloc(sizeof(igKdTree));
	leaf->axis = 3;
	leaf->right = leaf->left = NULL;
	leaf->plane = 0;
	unsigned tcnt = leaf->tcnt = igCountTriangles(es);
	leaf->trids = malloc(tcnt * sizeof(unsigned));
	igEvent* eit;
	for(eit = (*es)[0]; eit; eit = eit->next)
		if(eit->type == IGBEGIN || eit->type == IGPLANAR) {
				leaf->trids[tcnt - 1] = eit->trid;
				tcnt--;
		}
	igEventsFree(es);
	return leaf;
}

void igSpliceEvents(Triangle *t, igTriangleData *igdt, igEvents **es, igEvents **esleft, igEvents **esright, AABB *aabb) {
	(*esright) = igInitEvents();
	(*esleft) = igInitEvents();
	igEvents *left = *esleft, *right = *esright;
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (**es)[axis] = malloc(sizeof(igEvent));
		eit->next = NULL;
		while(eit) {
			switch(igdt[eit->trid]) {
				case IGLEFT_ONLY: {
					igEvent *tmp = eit->next;
					(*left)[axis] = (*left)[axis]->next = eit;
					eit = tmp;
					break;
				}
				case IGRIGHT_ONLY: {
					igEvent *tmp = eit->next;
					(*right)[axis] = (*right)[axis]->next = eit;
					eit = tmp;
					break;
				}
				case IGBOTH: {
					igEvent *tmp = eit->next;
					(**es)[axis] = eit->next = (**es)[axis];
					eit = tmp;
				}
			}
		}
		eit = (**es)[axis]->next;
		free((**es)[axis]);
		(**es)[axis] = eit;
	}
}

void igGenerateNewEvents(Triangle *t, igTriangleData* igdt, igEvents** es, igEvents **nesleft, igEvents **nesright, AABB *aabb, SAHSplitCandidate *sahc) {
	igEvent *eit = (**es)[sahc->axis];
	*nesleft = igInitEvents();
	*nesright = igInitEvents();
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		(*es)[axis] = malloc(sizeof(igEvent));
		(*es)[axis]->next = NULL;
	}
	while(eit) {
		if(igdt[eit->trid] == IGBOTH && eit->type == IGBEGIN) {
			AABB aabbs[2];
			perfectsplit(aabb, &t[eit->trid], aabbs, sahc);
			igCreateEvent(aabbs, *nesleft, eit->trid);
			igCreateEvent(aabbs + 1, *nesright, eit->trid);
		}
	}
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (*es)[axis]->next;
		free((*es)[axis]);
		(*es)[axis] = next;
	}
}

igTriangleData* igPrepareTriangleData(Triangle *t, unsigned tcnt) {
	return malloc(tcnt * sizeof(igTriangleData));
}

void igTriangleDataFree(igTriangleData *igdt) {
	free(igdt);
}
