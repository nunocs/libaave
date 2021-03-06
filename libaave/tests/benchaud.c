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
 *   libaave/tools/benchaud.c: benchmark of the audio processing
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../../libaave/aave.h"

/* Number of frames to process at a time. */
#define FRAMES 1024

/* Number of iterations to measure for each added sound source. */
#define N 100

#define VOLUME 5000
#define AREA 5000
#define RT60 5000

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
  	aave->area = AREA;
  	aave->volume = VOLUME;
  	aave->rt60 = RT60;
	aave_read_obj(aave, "model.obj");
	aave_hrtf_mit(aave);
	aave_init(aave);
	aave->reverb->active = 1;
	aave->reflections = 3;
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
