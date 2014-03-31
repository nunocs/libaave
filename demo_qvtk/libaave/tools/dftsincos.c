/*
 * libaave/tools/dftsincos.c: generate sin cos table for dft() and idft()
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * Usage: ./dftsincos N > ../dftsincos.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static unsigned dft_index(unsigned i, unsigned n)
{
	unsigned a, k;

	k = 1;

	for (a = 8; a < n; a *= 2) {
		if (i >= n / a / 2) {
			k = a / 2 - k;
			i = i - n / a / 2;
		}
	}

	return k;
}

int main(int argc, char **argv)
{
	unsigned j, k, n, DFT_N;
	double a;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s DFT_N\n", argv[0]);
		return 1;
	}

	DFT_N = atoi(argv[1]);

	printf("/* This file was automatically generated."
		" See tools/dftsincos.c */\n"
		"const float dftsincos[][2]={");

	for (n = 8; n <= DFT_N; n *= 2) {
		for (j = 0; j < n / 8; j++) {
			k = dft_index(j, n);
			a = 2 * M_PI * k / n;
			printf("{%e,%e},", sin(a), cos(a));
		}
	}

	printf("};\n");

	return 0;
}
