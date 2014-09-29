/*   This file is part of LibAAVE.
 * 
 *   LibAAVE is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibAAVE is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibAAVE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Copyright 2013 André Oliveira, Nuno Silva, Guilherme Campos,
 *   Paulo Dias, José Vieira
 *
 *
 *   libaave/examples/elevation.c: multiple sound source heights
 *
 *   Compile: gcc elevation.c -I.. ../libaave.a -lm -o elevation
 *   Usage: ./elevation sound1.raw sound2.raw ... > binaural.raw
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "aave.h"

/* The height distance between each sound source (m). */
#define H 10

/* The horizontal distance between listener and sound sources (m). */
#define D 2

/* The velocity that the listener is moving up (m/s). */
#define V 0.5

#define VOLUME 5000
#define AREA 5000
#define RT60 5000

int main(int argc, char **argv)
{
	struct aave *aave;
	struct aave_source *sources;
	FILE **sounds;
	short in[1], out[2];
	unsigned i, k, n;

	/* Initialise auralisation engine. */
	aave = malloc(sizeof *aave);

  	/* Set auralization input parameters */
  	aave->area = AREA;
  	aave->volume = VOLUME;
  	aave->rt60 = RT60;

  	/* Read room model. */
  	aave_read_obj(aave, "model.obj");

  	/* Select the HRTF set to use. */
  	aave_hrtf_mit(aave);
  	/* aave_hrtf_cipic(aave);  */
  	/* aave_hrtf_listen(aave); */
  	/* aave_hrtf_tub(aave);    */

  	/* Initialise the auralisation engine. */
  	aave_init(aave);

  	/* Extra auralization parameters */
  	aave->reverb->active = 0;
  	/* aave->reverb->mix = 0; */
  	aave->reflections = 0;

	/* Set the orientation of the listener. */
	aave_set_listener_orientation(aave, 0, 0, 0);

	/* Number of sounds specified in the arguments. */
	n = argc - 1;

	/* Initialise the sound sources. */
	sources = malloc(n * sizeof *sources);
	sounds = malloc(n * sizeof *sounds);
	for (i = 0; i < n; i++) {
		aave_init_source(aave, &sources[i]);
		aave_add_source(aave, &sources[i]);
		aave_set_source_position(&sources[i], D, 0, i * H);
		sounds[i] = fopen(argv[i+1], "rb");
		if (!sounds[i]) {
			fprintf(stderr, "error opening file %s\n", argv[i+1]);
			return 1;
		}
	}

	/* Process one sample at a time until we get to the highest source. */
	for (k = 0; k <= n * (AAVE_FS * H / (float)V); k++) {

		/* Update the position of the listener. */
		aave_set_listener_position(aave, 0, 0, k * (V/(float)AAVE_FS));

		/* Update the geometry state of the auralisation engine. */
		aave_update(aave);

		/*
		 * Read one sample from each sound file and give it to
		 * its sound source. If the file ends, give it a 0 sample.
		 */
		for (i = 0; i < n; i++) {
			if (fread(in, sizeof(in[0]), 1, sounds[i]) != 1)
				in[0] = 0;
			aave_put_audio(&sources[i], in, 1);
		}

		/* Run the engine to get the corresponding binaural frame. */
		aave_get_audio(aave, out, 1);

		/* Write the binaural frame (2 samples) to stdout. */
		if (fwrite(out, sizeof(out[0]), 2, stdout) != 2)
			return 1;
	}

	return 0;
}
