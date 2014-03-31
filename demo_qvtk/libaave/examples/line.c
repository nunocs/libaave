/*
 * libaave/examples/line.c: sound source moving on a straight line
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * Usage: ./line < mono.raw > binaural.raw
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "aave.h"

/* The velocity that the sound source is moving at (m/s). */
#define V (40 /* km/h */ * 1000 / 3600.)

/* The initial Y coordinate position of the sound source (m). */
#define Y0 -60

int main(int argc, char **argv)
{
	struct aave *aave;
	struct aave_source *source;
	short in[1], out[2];
	float y;

	/* Initialise the auralisation engine. */
	aave = malloc(sizeof *aave);
	aave_init(aave);

	/* Select the HRTF set to use. */
	/* aave_hrtf_cipic(aave); */
	/* aave_hrtf_listen(aave); */
	aave_hrtf_mit(aave);
	/* aave_hrtf_tub(aave); */

	/* Set the position and orientation of the listener. */
	aave_set_listener_position(aave, 0, 0, 0);
	aave_set_listener_orientation(aave, 0, 0, 0);

	/* Load a room model file, if specified on the arguments. */
	if (argc == 2)
		aave_read_obj(aave, argv[1]);

	/* Add a sound source to the auralisation world. */
	source = malloc(sizeof *source);
	aave_init_source(aave, source);
	aave_add_source(aave, source);

	/* Initial position of the sound source. */
	y = Y0;

	/* Read and process one sample at a time from the sound file. */
	while (fread(in, sizeof(in[0]), 1, stdin) == 1) {

		/* Set the position of the source for this sound sample. */
		aave_set_source_position(source, 2, y, 0);

		/* Update the geometry state of the auralisation engine. */
		aave_update(aave);

		/* Pass this sound sample to the auralization engine. */
		aave_put_audio(source, in, 1);

		/* Run the engine to get the corresponding binaural frame. */
		aave_get_audio(aave, out, 1);

		/* Write the binaural frame (2 samples) to stdout. */
		if (fwrite(out, sizeof(out[0]), 2, stdout) != 2)
			return 1;

		/* Update the position of the source for the next sample. */
		y += V / AAVE_FS;
	}

	return 0;
}
