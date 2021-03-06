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
 *   libaave/examples/line.c: sound source moving on a straight line
 *
 *   Compile: gcc line.c -I.. ../libaave.a -lm -o line
 *   Usage: ./line < mono.raw > binaural.raw
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "aave.h"

/* The velocity that the sound source is moving at (m/s). */
#define V (40 /* km/h */ * 1000 / 3600.)

/* The initial Y coordinate position of the sound source (m). */
#define Y0 -60

#define VOLUME 5000
#define AREA 5000
#define RT60 5000

int main(int argc, char **argv)
{
	struct aave *aave;
	struct aave_source *source;
	short in[1], out[2];
	float y;

	/* Initialise auralisation engine. */
	aave = malloc(sizeof *aave);

  	/* Set auralization input parameters */
  	aave->area = AREA;
  	aave->volume = VOLUME;
  	aave->rt60 = RT60;

	/* Load a room model file, if(?) specified on the arguments. */
	if (argc == 2)
		aave_read_obj(aave, argv[1]);
	else
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

	/* Set the position and orientation of the listener. */
	aave_set_listener_position(aave, 0, 0, 0);
	aave_set_listener_orientation(aave, 0, 0, 0);

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
