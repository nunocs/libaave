/*
 * libaave/tools/hrtf_tub_set.c: generate TU-Berlin HRTF .c file
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * Usage: ./hrtf_tub_set QU_KEMAR_anechoic_??.wav > ../hrtf_tub_set.c
 */

#include <stdio.h>

/*
 * Number of samples to use from the 2048-sample TU-Berlin HRIRs.
 * We'll use only the first 1024.
 */
#define N 1024

/* DFT length (zero-padding). */
#define DFT_N (N * 4)

#define DFT_TYPE float
#include "../dft.h"

int main(int argc, char **argv)
{
	FILE *f;
	unsigned i, j;
	static DFT_TYPE hrir[N * 2];
	float hrtf[DFT_N];
	int data[2048][360*2]; /* 2048 32-bit samples, 360 degrees * 2 ears */

	if (argc != 2) {
		fprintf(stderr, "Usage: %s QU_KEMAR_anechoic_??.wav\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "rb");
	if (!f) {
		perror(argv[1]);
		return 1;
	}

	/*
	 * Skip to the beginning of the data.
	 * Skip "RIFF" chunk, "fmt " subchunk and "data" subchunk header.
	 */
	if (fseek(f, 44, SEEK_SET)) {
		perror("seek");
		return 1;
	}

	if (fread(&data, 1, sizeof data, f) != sizeof data) {
		perror("fread");
		return 1;
	}

	fclose(f);

	printf("/* This file was automatically generated."
		" See tools/hrtf_tub_set.c */\n"
		"const float hrtf_tub_set[720][%u]={", DFT_N);

	for (i = 0; i < 360 * 2; i++) {
		for (j = 0; j < N; j++)
			hrir[j] = data[j][i] / 2147483648.;
		dft(hrtf, hrir, DFT_N);
		printf("{");
		for (j = 0; j < DFT_N; j++)
			printf("%e,", hrtf[j]);
		printf("},");
	}

	printf("};\n");

	return 0;
}
