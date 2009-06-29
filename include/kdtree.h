#ifndef KDTREE_HEADER
#define KDTREE_HEADER

#include <stdint.h>

typedef struct kdTree {
	union {
		float plane;
		uint32_t tcnt;
	} d;
	uint32_t ptr;
	
} kdTree;

#endif
