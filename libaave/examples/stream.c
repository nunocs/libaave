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
 *   libaave/examples/stream.c: auralise a streaming input sound
 *
 *   Compile: gcc stream.c -I.. -lasound ../libaave.a -lm -o stream
 *   Usage: ./stream model.obj reflection order
 */

#include <stdio.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include "aave.h"

/* Number of frames to capture/playback per loop. */
#define FRAMES 256

#define VOLUME 4000
#define AREA 5000
#define RT60 5000

int main(int argc, char **argv)
{
	struct aave *aave;
	struct aave_source *source;
	snd_pcm_t *capture, *playback;
	int n;
	short buffer[FRAMES * 2];

	if (argc != 3) {
		fprintf(stderr,
			"Usage: %s MODEL.OBJ REFLECTION_ORDER\n", argv[0]);
		return 1;
	}

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
  	aave->reverb->active = 1;

	/* Set the highest order of reflections to generate. */
	aave->reflections = atoi(argv[2]);

	/* Set the position and orientation of the listener. */
	aave_set_listener_position(aave, 0, 0, 0);
	aave_set_listener_orientation(aave, 0, 0, 0);

	/* Add the sound source to the auralisation world. */
	source = malloc(sizeof *source);
	aave_init_source(aave, source);
	aave_add_source(aave, source);
	aave_set_source_position(source, 3, 0, 0);

	/* Perform the geometry calculations. */
	aave_update(aave);
	
	//sleep(2);

	/* Open ALSA capture device. */
	n = snd_pcm_open(&capture, "default", SND_PCM_STREAM_CAPTURE, 0);
	if (n < 0) {
		fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(n));
		return 1;
	}
	n = snd_pcm_set_params(capture, SND_PCM_FORMAT_S16,
				SND_PCM_ACCESS_RW_INTERLEAVED,
				1, 44100, 0, 50000); /* 50ms latency */
	if (n < 0) {
		fprintf(stderr, "snd_pcm_set_params: %s\n", snd_strerror(n));
		return 1;
	}

	/* Open ALSA playback device. */
	n = snd_pcm_open(&playback, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (n < 0) {
		fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(n));
		return 1;
	}
	n = snd_pcm_set_params(playback, SND_PCM_FORMAT_S16,
				SND_PCM_ACCESS_RW_INTERLEAVED,
				2, 44100, 0, 50000); /* 50ms latency */
	if (n < 0) {
		fprintf(stderr, "snd_pcm_set_params: %s\n", snd_strerror(n));
		return 1;
	}
	
	for (;;) {
		/* Read the mono frames from the capture device. */
		n = snd_pcm_readi(capture, buffer, FRAMES);
		if (n < 0) {
			fprintf(stderr, "snd_pcm_readi: %s\n",
							snd_strerror(n));
			return 1;
		}

		/* Feed the mono frames to the sound source. */
		aave_put_audio(source, buffer, n);

		/* Get the binaural frames of the auralisation result. */
		aave_get_audio(aave, buffer, n);

		/* Write the binaural frames to the playback device. */
		n = snd_pcm_writei(playback, buffer, n);
		if (n < 0) {
			fprintf(stderr, "snd_pcm_writei: %s\n",
				 snd_strerror(n));
			return 1;
		}
	}

	return 0;
}
