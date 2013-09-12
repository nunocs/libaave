/*
 * libaave/hrtf_listen.c: audio processing with LISTEN HRTF set
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include "aave.h"

extern const float hrtf_listen_set_1040[][2][1024];

/*
 * Select the HRTF pair for the specified azimuth and elevation angles.
 * Elevation is [-90;90] degrees. Azimuth is [-180;180] degrees.
 */
static void aave_hrtf_listen_get(const float *hrtf[2], int elevation, int azimuth)
{
	unsigned i;

	azimuth = -azimuth;
	if (azimuth < 0)
		azimuth += 360;

	if (elevation < -45)
		elevation = -45;
	else if (elevation > 45)
		elevation = 45;

	/* TODO: elevations 60, 75 and 90 */

	i = (elevation + 45) / 15 * 24 + azimuth / 15;

	hrtf[0] = hrtf_listen_set_1040[i][0];
	hrtf[1] = hrtf_listen_set_1040[i][1];
}

void aave_hrtf_listen(struct aave *a)
{
	a->hrtf_frames = 512;
	a->hrtf_get = aave_hrtf_listen_get;
	a->hrtf_output_buffer_index = 0;
}
