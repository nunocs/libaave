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
 *   Paulo Dias, José Vieira/IEETA - Universidade de Aveiro
 *
 *
 *   libaave/examples/circle.c: generate a sinusoid sound circling around listener
 *   Compile: gcc circle.c -I.. ../libaave.a -lm -o circle	
 *   Usage: ./circle > output.raw
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <aave.h>

/* The frequency of the sinusoid (Hz). */
#define F 1000

/* The distance the sound source is from the listener (m). */
#define D 5

/* The angular velocity that the sound source is moving at (rad/s). */
#if 1
#define W (2 * M_PI / 10) /* do one full circle in 10 seconds */
#else
#define W (2 * M_PI) /* do one full circle per second */
#endif

#define VOLUME 5000
#define AREA 5000
#define RT60 5000

int main()
{
	struct aave *aave;
	struct aave_source *source;
	double angle;
	float x, y;
	unsigned n;
	short in, out[2];

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

	/* Set position and orientation of the listener. */
	aave_set_listener_position(aave, 0, 0, 0);
	aave_set_listener_orientation(aave, 0, 0, 0);

	/* Add the sound source to the auralisation world. */
	source = malloc(sizeof *source);
	aave_init_source(aave, source);
	aave_add_source(aave, source);

	/* Number of samples processed so far. */
	n = 0;

	/* Process one sample at a time until we do 10 full circles. */
	for (angle = 0; angle <= 10 * 2 * M_PI; angle += W / AAVE_FS) {

		/* Set the position of the source for this sample. */
		x = D * cos(angle);
		y = D * sin(angle);
		aave_set_source_position(source, x, y, 0);

		/* Update the geometry state of the auralisation engine. */
		aave_update(aave);

		/* Generate one sample of audio for the sound source. */
		in = sin(2 * M_PI * F / AAVE_FS * n++) * 32767;
		aave_put_audio(source, &in, 1);

		/* Collect the two samples of auralised binaural output. */
		aave_get_audio(aave, out, 1);

		/* Write the output to stdout in raw format. */
		if (fwrite(out, sizeof(out[0]), 2, stdout) != 2)
			return 1;
	}

	return 0;
}
