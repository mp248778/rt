#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "IngoKdTree.h"

int main() {
	Triangle *t;
	unsigned tcnt;
	readTriangles(&t, &tcnt);
	igBuild(t, tcnt);
	return EXIT_SUCCESS;
}

