#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IngoKdTree.h"
#include "event.h"
#include "aabb.h"
#include "sah.h"
#include "sahsplitcandidate.h"
#include "printer.h"

typedef enum {
	IGBOTH,
	IGLEFT_ONLY,
	IGRIGHT_ONLY
} igTriangleData;

igKdTree* igCreateInternalNode(SAHSplitCandidate*, igKdTree*, igKdTree*);
igKdTree* igCreateLeaf(Triangle*, igEvents*);
igKdTree* igCreateTree(Triangle *, igTriangleData *, AABB *, igEvents *);
igKdTree* igKdTreeRoot();
igTriangleData* igPrepareTriangleData(Triangle*, uint32_t);
void igClassifyTriangles(Triangle*, igTriangleData *, igEvents *, SAHSplitCandidate *);
void igGenerateNewEvents(Triangle*, igTriangleData*, igEvents**, igEvents**, igEvents**, AABB*, SAHSplitCandidate*);
void igSpliceEvents(Triangle*, igTriangleData*, igEvents**, igEvents**, igEvents**, AABB*);
void igTriangleDataFree(igTriangleData*);

igKdTree* igBuild(Triangle *t, const uint32_t tcnt) {
	igEvents *es = igCreateEvents(t, tcnt);
	AABB *aabb = getAABB(t, tcnt);
	igTriangleData *igdt = igPrepareTriangleData(t, tcnt);

	igKdTree *root = igCreateTree(t, igdt, aabb, es);

	igTriangleDataFree(igdt);
	return root;
}

void verifyEvents(igEvents *es) {
	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		float prev = -1.0f;
		igEvent *eit = (*es)[axis];
		while(eit) {
			if(eit->plane < prev) {
				fprintf(stderr, "bladzik %f %f %i \n", prev, eit->plane, eit->trid);
			} else 	prev = eit->plane;
			eit = eit->next;
		}
	}
}

igKdTree* igCreateTree(Triangle *t, igTriangleData *igdt, AABB *aabb, igEvents *es) {
	igKdTree *node;
	SAHSplitCandidate *split = igFindSAHSplitCandidate(t, aabb, es);

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
	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (*es)[axis]->next;
		while(eit) {
			igdt[eit->trid] = IGBOTH;
			eit = eit->next;
		}
	}
	axis = split->axis;
	igEvent *eit = (*es)[axis]->next;
	while(eit) {
		if(eit->axis == axis) {
			if(eit->type != IGBEGIN && eit->plane < split->plane) igdt[eit->trid] = IGLEFT_ONLY;
			else if(eit->type != IGEND && eit->plane > split->plane) igdt[eit->trid] = IGRIGHT_ONLY;
		}
		eit = eit->next;
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
	uint32_t tcnt = leaf->tcnt = igCountTriangles(es);
	leaf->trids = malloc(tcnt * sizeof(uint32_t));
	igEvent* eit;
	for(eit = (*es)[0]->next; eit; eit = eit->next)
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
	igEvents left, right, both;
	memcpy(&left, *esleft, sizeof(igEvents));
	memcpy(&right, *esright, sizeof(igEvents));
	memcpy(&both, *es, sizeof(igEvents));
	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (**es)[axis]->next;
		while(eit) {
			igEvent *tmp = eit->next;
			eit->next = NULL;
			switch(igdt[eit->trid]) {
				case IGLEFT_ONLY:
					left[axis] = left[axis]->next = eit;
					break;
				case IGRIGHT_ONLY:
					right[axis] = right[axis]->next = eit;
					break;
				case IGBOTH:
					both[axis] = both[axis]->next = eit;
					break;
			}
			eit = tmp;
		}
	}
}

void igGenerateNewEvents(Triangle *t, igTriangleData* igdt, igEvents** es, igEvents **nesleft, igEvents **nesright, AABB *aabb, SAHSplitCandidate *sahc) {
	igEvent *eit = (**es)[sahc->axis]->next;
	*nesleft = igInitEvents();
	*nesright = igInitEvents();
	while(eit) {
		if(igdt[eit->trid] == IGBOTH && eit->type != IGEND) {
			AABB aabbs[2];
			perfectsplit(aabb, &t[eit->trid], aabbs, sahc);
			igCreateEvent(aabbs, *nesleft, eit->trid);
			igCreateEvent(aabbs + 1, *nesright, eit->trid);
		}
		eit = eit->next;
	}
	igEventsFree(*es);
}

igTriangleData* igPrepareTriangleData(Triangle *t, uint32_t tcnt) {
	return malloc(tcnt * sizeof(igTriangleData));
}

void igTriangleDataFree(igTriangleData *igdt) {
	free(igdt);
}

void freeIgKdTree(igKdTree *node) {
	if(node->axis == 3) {
		free(node->trids);
	} else {
		freeIgKdTree(node->left);
		freeIgKdTree(node->right);
	}
	free(node);
}
