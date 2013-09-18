/*
 * libaave/reverb.c: artificial reverberation tail
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include "aave.h"

/* Pre-delay { left, right }: about 743 ms (prime numbers work best). */
static const int predelay[2] = { 32749, 32603 };

/* Feedback delay { left, right }: about 23 ms (prime numbers work best). */
static const int delay[2] = { 983, 1021 };

/*
 * Add an artificial reverberation tail to the binaural N frames of AUDIO.
 */
void aave_reverb(struct aave *aave, short *audio, unsigned n)
{
	unsigned i, j1[2], j2[2], c;
	int x[2], xi, sum;
	float y, z;

	for (c = 0; c < 2; c++) {
		j1[c] = aave->reverb_buffer1_index[c];
		j2[c] = aave->reverb_buffer2_index[c];
	}

	for (i = 0; i < n; i++) {
		/* Input samples. */
		x[0] = audio[i * 2 + 0];
		x[1] = audio[i * 2 + 1];

		/* The reverb input is the mono sum of both channels. */
		sum = x[0] + x[1];

		for (c = 0; c < 2; c++) {
			/* Pre-delay. */
			y = aave->reverb_buffer1[j1[c]][c] * 0.02;
			aave->reverb_buffer1[j1[c]][c] = sum;

			/* Feedback delay. */
			z = aave->reverb_buffer2[j2[c]][c] * 0.93;
			aave->reverb_buffer2[j2[c]][c] = y + z;

			/* Output (clip overflown samples). */
			xi = x[c] + y + z;
			if (xi > 32767)
				xi = 32767;
			else if (xi < -32768)
				xi = -32768;
			audio[i * 2 + c] = xi;

			/* Increment buffer indices. */
			j1[c] = (j1[c] + 1) % predelay[c];
			j2[c] = (j2[c] + 1) % delay[c];
		}
	}

	for (c = 0; c < 2; c++) {
		aave->reverb_buffer1_index[c] = j1[c];
		aave->reverb_buffer2_index[c] = j2[c];
	}
}
