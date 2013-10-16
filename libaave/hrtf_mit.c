/*
 * libaave/hrtf_mit.c: audio processing using the MIT KEMAR HRTF set
 *
 * Copyright 2012,2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file hrtf_mit.c
 *
 * The hrtf_mit.c file implements the interface to use the MIT KEMAR HRTF
 * compact set. To select this set for the auralisation process, call
 * aave_hrtf_mit() after initialising the aave structure and before
 * calling aave_put_audio().
 *
 * The MIT KEMAR HRTF compact set consists of head-related impulse responses
 * (HRIR) of a KEMAR dummy at 1.4m distance, data-reduced by post-processing
 * to be made more compact. Each HRIR is 128 samples long at 44100Hz
 * (~2.9ms) and only the left ear is stored (the right ear is assumed
 * to be symmetric).
 *
 * HRIR measurements are available for elevations from -40 to 90 degrees in
 * 10 degree steps. Depending on the elevation, the available azimuths are:
 * - elevation 90: 1 azimuth measure
 * - elevation 80: azimuths in 30 degree steps
 * - elevation 70: azimuths in 15 degree steps
 * - elevation 60: azimuths in 10 degree steps
 * - elevation 50: azimuths in 8 degree steps
 * - elevation 40 and -40: azimuths in 6.5 degree steps
 * - elevation 30 and -30: azimuths in 6 degree steps
 * - elevation 20 to -20: azimuths in 5 degree steps
 *
 * References:
 * http://sound.media.mit.edu/resources/KEMAR.html
 */

#include "aave.h"

/** The HRTF set, generated by tools/hrtf_mit_set.c. */
extern const float hrtf_mit_set[][2][512];

/**
 * Get the closest HRTF pair for the specified coordinates.
 * @p elevation is [-90;90] degrees. @p azimuth is [-180;180] degrees.
 * @p hrtf[0] will be the left HRTF, @p hrtf[1] the right HRTF.
 */
static void aave_hrtf_mit_get(const float *hrtf[2], int elevation, int azimuth)
{
	int a;
	unsigned i;

	a = azimuth < 0 ? -azimuth : azimuth;

	if (elevation < -35) /* H-40e???a.wav */
		i = (a + 3) * 2 / 13;
	else if (elevation < -25) /* H-30e???a.wav */
		i = 29 + (a + 3) / 6;
	else if (elevation < -15) /* H-20e???a.wav */
		i = 60 + (a + 3) / 5;
	else if (elevation < -5) /* H-10e???a.wav */
		i = 97 + (a + 3) / 5;
	else if (elevation < 5) /* H0e???a.wav */
		i = 134 + (a + 3) / 5;
	else if (elevation < 15) /* H10e???a.wav */
		i = 171 + (a + 3) / 5;
	else if (elevation < 25) /* H20e???a.wav */
		i = 208 + (a + 3) / 5;
	else if (elevation < 35) /* H30e???a.wav */
		i = 245 + (a + 3) / 6;
	else if (elevation < 45) /* H40e???a.wav */
		i = 276 + (a + 3) * 2 / 13;
	else if (elevation < 55) /* H50e???a.wav */
		i = 305 + (a + 4) / 8;
	else if (elevation < 65) /* H60e???a.wav */
		i = 328 + (a + 5) / 10;
	else if (elevation < 75) /* H70e???a.wav */
		i = 347 + (a + 7) / 15;
	else if (elevation < 90) /* H80e???a.wav */
		i = 360 + (a + 15) / 30;
	else /* H90e000a.wav */
		i = 367;

	if (azimuth >= 0) {
		hrtf[0] = hrtf_mit_set[i][0]; /* left */
		hrtf[1] = hrtf_mit_set[i][1]; /* right */
	} else {
		hrtf[0] = hrtf_mit_set[i][1]; /* left */
		hrtf[1] = hrtf_mit_set[i][0]; /* right */
	}
}

/**
 * Select the MIT KEMAR HRTF compact set for the auralisation process.
 */
void aave_hrtf_mit(struct aave *a)
{
	a->hrtf_frames = 128;
	a->hrtf_get = aave_hrtf_mit_get;
	a->hrtf_output_buffer_index = 0;
}
