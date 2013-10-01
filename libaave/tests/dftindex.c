/*
 * libaave/tests/dftindex.c: test the dft() and dft_index() functions
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <stdio.h>
#include <stdlib.h>
#include "aave.h"

#define DFT_TYPE int
#include "../dft.h"

int main(int argc, char **argv)
{
	DFT_TYPE *x;
	float *X;
	unsigned i, j, n;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s N\n", argv[0]);
		return 1;
	}

	n = atoi(argv[1]);

	x = malloc(n / 2 * sizeof *x);
	X = malloc(n * sizeof *X);

	for (i = 0; i < n / 2; i++)
		x[i] = i;

	dft(X, x, n);

	printf("X[0] = %f\n", X[0]);
	for (i = 1; i < n / 2; i++) {
		j = dft_index(i, n);
		printf("X[%u-%u] = %f + j %f\n", i, j, X[j * 2], X[j * 2 + 1]);
	}
	printf("X[%u] = %f\n", n / 2, X[1]);

	return 0;
}
