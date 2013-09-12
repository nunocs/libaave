/*
 * libaave/tools/sin.c: generate a sinusoid sound file
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <math.h> /* M_PI, sin() */
#include <stdio.h> /* fwrite() */
#include <stdlib.h> /* atoi() */

/* Sampling frequency (Hz). */
#define FS 44100

int main(int argc, char **argv)
{
	unsigned i, f, n;
	double w;
	short x;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s HZ\n", argv[0]);
		return 1;
	}

	/* Frequency of the sinusoid. */
	f = atoi(argv[1]);

	/* Generate one period of the sinusoid. */
	n = FS / f;
	w = 2 * M_PI / FS * f;
	for (i = 0; i < n; i++) {
		x = sin(w * i) * 32767;
		fwrite(&x, 2, 1, stdout);
	}

	return 0;
}
