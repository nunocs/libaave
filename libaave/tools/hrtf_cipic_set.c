/*
 * libaave/tools/hrtf_cipic_set.c: generate CIPIC HRTF set .c file
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * Usage: ./hrtf_cipic_set < cipic_hrir.txt > ../hrtf_cipic_set.c
 */

#include <stdio.h>

/* Number of samples of each CIPIC HRIR. */
#define N 200

/* Next power of two of the number of samples. */
#define N2 256

#define DFT_TYPE float
#include "../dft.h"

int main(int argc, char **argv)
{
	static float hrir[N2];
	float hrtf[N2*2];
	unsigned i, j;
	float x;

	printf("/* This file was automatically generated. "
		"See tools/hrtf_cipic_set.c */\n"
		"const float hrtf_cipic_set[][%u]={", N2*2);

	for (i = 0; i < 25; i++) {
		for (j = 0; j < N; j++) {
			if (fscanf(stdin, "%f", &x) != 1) {
				perror("fscanf");
				return 1;
			}
			hrir[j] = x / 1.2; /* some samples exceed 1 !!! */
		}
		dft(hrtf, hrir, N2*2);
		printf("{");
		for (j = 0; j < N2*2; j++)
			printf("%e,", hrtf[j]);
		printf("},");
	}

	printf("};\n");

	return 0;
}
