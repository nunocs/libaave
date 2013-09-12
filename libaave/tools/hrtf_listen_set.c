/*
 * libaave/tools/hrtf_listen_set.c: generate file with HRTFs of a LISTEN set
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * Usage:
 * ./hrtf_listen_set XXXX IRC_XXXX_C_*P3*.wav IRC_XXXX_C_*P0*.wav > ../hrtf_listen_set_XXXX.c
 */

#include <stdio.h>

/* Frames per HRTF. */
#define N 512

#define DFT_TYPE float
#include "../dft.h"

static void hrtf_listen(const char *filename)
{
	FILE *f;
	unsigned char stereo[N * 6]; /* 24 bits, 2 channels */
	DFT_TYPE mono[N];
	float hrtf[N*2];
	unsigned i;

	f = fopen(filename, "rb");
	if (!f) {
		fprintf(stderr, "error opening file %s\n", filename);
		return;
	}

	/* Seek to the begining of the sample data, in the data chunk. */
	if (fseek(f, 44, SEEK_SET)) {
		fprintf(stderr, "seek error in file %s\n", filename);
		return;
	}

	if (fread(stereo, 6, N, f) != N) {
		fprintf(stderr, "read error in file %s\n", filename);
		return;
	}

	fclose(f);

	/* Left channel. */
	printf("{{");
	for (i = 0; i < N; i++)
		mono[i] = (stereo[6*i+0] | stereo[6*i+1]<<8 | ((signed char *)stereo)[6*i+2]<<16) / 16777215.;
	dft(hrtf, mono, N*2);
	for (i = 0; i < N*2; i++)
		printf("%e,", hrtf[i]);

	/* Right channel. */
	printf("},{");
	for (i = 0; i < N; i++)
		mono[i] = (stereo[6*i+3] | stereo[6*i+4]<<8 | ((signed char *)stereo)[6*i+5]<<16) / 16777215.;
	dft(hrtf, mono, N*2);
	for (i = 0; i < N*2; i++)
		printf("%e,", hrtf[i]);

	printf("}},");
}

int main(int argc, char **argv)
{
	printf("/* This file was automatically generated. See tools/hrtf_listen_set.c */\n"
		"const float hrtf_listen_set_%s[][2][%u]={", *++argv, N*2);

	while (*++argv)
		hrtf_listen(*argv);

	printf("};\n");

	return 0;
}
