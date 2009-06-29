#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "writer.h"
#include "IngoKdTree.h"

int main() {
	Triangle *t;
	uint32_t tcnt;
	readTriangles(&t, &tcnt);
//	kdTree *tree = readKdTree("/tmp/asd");
//	printf("%i %f %i", tree->ptr, tree->d.plane, tree->d.tcnt);
	igKdTree *tree= igBuild(t, tcnt);
	igWriteTree(tree, "/tmp/asd");
	freeIgKdTree(tree);
	return EXIT_SUCCESS;
}

