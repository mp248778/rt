#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reader.h"

void readTriangles(Triangle **t, uint32_t *tcnt) {
	uint32_t vcnt;
	scanf("%i %i", &vcnt, tcnt);
	float *v = malloc(vcnt * 3 * sizeof(float));
	*t = malloc(*tcnt * sizeof(Triangle));

	uint32_t i;
	for(i = 0; i < vcnt; i++)
		scanf("%f %f %f", &v[3 * i], &v[3 * i + 1], &v[3 * i + 2]);
	for(i = 0; i < *tcnt; i++) {
		uint32_t A, B, C;
		scanf("%i %i %i", &A, &B, &C);
		memcpy(&(*t)[i][0], &v[A * 3], 3 * sizeof(float));
		memcpy(&(*t)[i][3], &v[B * 3], 3 * sizeof(float));
		memcpy(&(*t)[i][6], &v[C * 3], 3 * sizeof(float));
	}
	free(v);
}


kdTree* readKdTree(const char *filename) {
	FILE *file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	uint32_t size = ftell(file);
	rewind(file);
	kdTree *tree = malloc(size);
	fread(tree, size, 1, file);
	return tree;
}
