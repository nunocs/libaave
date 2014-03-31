/*
 * libaave/tools/benchaud.c: benchmark of the audio processing
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../../libaave/aave.h"

/* Number of frames to process at a time. */
#define FRAMES 1024

/* Number of iterations to measure for each added sound source. */
#define N 100

int main()
{
	struct aave *aave;
	struct aave_source *source;
	struct timeval t1, t2;
	short buf[FRAMES * 2];
	unsigned i, j, dt, max;
	double average;

	/* Create auralisation engine. */
	aave = malloc(sizeof *aave);
	aave_init(aave);
	aave_hrtf_mit(aave);
	aave_set_listener_position(aave, 0, 0, 0);
	aave_set_listener_orientation(aave, 0, 0, 0);

	/* Add sound sources one by one. */
	for (i = 0; i < 100; i++) {
		source = malloc(sizeof *source);
		aave_init_source(aave, source);
		aave_add_source(aave, source);
		aave_set_source_position(source, 1, 0, 0);
		aave_update(aave);

		/* Measure the audio processing time. */
		average = 0;
		max = 0;
		for (j = 0 ; j < N; j++) {
			gettimeofday(&t1, 0);
			for (source = aave->sources;
			     source;
			     source = source->next)
				aave_put_audio(source, buf, FRAMES);
			aave_get_audio(aave, buf, FRAMES);
			gettimeofday(&t2, 0);
			dt = (t2.tv_sec - t1.tv_sec) * 1000000
				+ (t2.tv_usec - t1.tv_usec);
			average += (double)dt / N;
			if (dt > max)
				max = dt;
		}
		printf("%u %u %u\n", i, (unsigned)average, max);
	}

	return 0;
}
