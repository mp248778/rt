#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reader.h"

void readTriangles(Triangle **t, unsigned *tcnt) {
	unsigned vcnt;
	scanf("%i %i", &vcnt, tcnt);
	float *v = malloc(vcnt * 3 * sizeof(float));
	*t = malloc(*tcnt * sizeof(Triangle));

	unsigned i;
	for(i = 0; i < vcnt; i++)
		scanf("%f %f %f", &v[3 * i], &v[3 * i + 1], &v[3 * i + 2]);
	for(i = 0; i < *tcnt; i++) {
		unsigned A, B, C;
		scanf("%i %i %i", &A, &B, &C);
		memcpy(&(*t)[i][0], &v[A * 3], 3 * sizeof(float));
		memcpy(&(*t)[i][3], &v[B * 3], 3 * sizeof(float));
		memcpy(&(*t)[i][6], &v[C * 3], 3 * sizeof(float));
	}
	free(v);
}
