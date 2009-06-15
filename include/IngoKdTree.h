#ifndef INGO_KDTREE_HEADER
#define INGO_KDTREE_HEADER

typedef struct igKdTree {
	unsigned axis;
	float plane;
	struct igKdTree *left; 
	struct igKdTree *right;
	unsigned tcnt;
	unsigned *trids;
} igKdTree;

#endif
