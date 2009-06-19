#ifndef INGO_KDTREE_HEADER
#define INGO_KDTREE_HEADER

#include "triangle.h"

typedef struct igKdTree {
	unsigned axis;
	float plane;
	struct igKdTree *left; 
	struct igKdTree *right;
	unsigned tcnt;
	unsigned *trids;
} igKdTree;

igKdTree* igBuild(Triangle *, const unsigned);

#endif
