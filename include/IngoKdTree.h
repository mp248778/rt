#ifndef INGO_KDTREE_HEADER
#define INGO_KDTREE_HEADER

typedef struct igKdTree {
	struct igKdTree *child; //aligned to 8 byte, so we have some room in here for axis and leaf flag
	union {
		float plane;
		unsigned tcnt;
	};
} igKdTree;


#endif
