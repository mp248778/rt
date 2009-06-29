#include <stdio.h>
#include "printer.h"

#ifdef DEBUG

void printSAHSplitCandidate(SAHSplitCandidate *sahc) {
	printf("sahc: axis=%i plane=%f terminate=%i cost=%f\n", sahc->axis, sahc->plane, sahc->terminate, sahc->cost); 
}

void printAABB(AABB *aabb) {
	printf("aabb:");
	uint32_t i;
	for(i = 0; i < 6; i++)
			printf(" %f", (*aabb)[i]);
	printf("\n");
}

void printEvents(igEvents *es) {
	uint32_t axis = 0;
	igEvent *eit = (*es)[0]->next;
	for(; eit; eit = eit->next)
		printEvent(eit);
}

void printEvent(igEvent *e) {
	printf("event: axis=%i plane=%f type=%i trid=%i\n", e->axis, e->plane, e->type, e->trid);
}

void printigKdTree(igKdTree *node) {
	printf("node: axis=%i", node->axis);
	if(node->axis == 3) {
		printf(" tcnt=%i trids=", node->tcnt);
		for(int i = 0; i < node->tcnt; i++) {
			printf(" %i", node->trids[i]);
		}
		printf("\n");
	} else {
		printf(" plane=%f\n", node->plane);
	}
}


#else

void printSAHSplitCandidate(SAHSplitCandidate *sahc) {
}

void printAABB(AABB *aabb) {
}

void printEvents(igEvents *es) {
}

void printEvent(igEvent *e) {
}

void printigKdTree(igKdTree *node) {
}

#endif


