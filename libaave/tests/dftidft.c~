/*
 * libaave/test/dftidft.c: test the correctness of dft() and idft()
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <stdio.h>
#include "aave.h"

#define DFT_TYPE short
#include "../dft.h"

#define IDFT_TYPE int
#include "../idft.h"

#define N 4096

int main()
{
	short x[N/2];
	int y[N];
	float X[N];
	unsigned i;

	dft(X, x, N);
	idft(y, X, N);

	for (i = 0; i < N/2; i++)
		if (x[i] > y[i] + 1 || x[i] < y[i] - 1)
			fprintf(stderr, "%u: x=%d y=%d\n", i, x[i], y[i]);

	for ( ; i < N; i++)
		if (y[i] != 0)
			fprintf(stderr, "%u: y=%d\n", i, y[i]);

	return 0;
}
