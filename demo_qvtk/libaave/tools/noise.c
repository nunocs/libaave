/*
 * libaave/tools/noise.c: generate 1 second white noise sound file
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <stdio.h> /* fwrite() */
#include <stdlib.h> /* rand() */

/* Sampling frequency (Hz). */
#define FS 44100

int main()
{
	unsigned i;
	short x;

	for (i = 0; i < FS; i++) {
		x = rand() / (RAND_MAX / 65535) - 32768;
		fwrite(&x, 2, 1, stdout);
	}

	return 0;
}
