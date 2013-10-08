/*
 * libaave/hrtf_tub.c: audio processing with TU-Berlin HRTF set
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file hrtf_tub.c
 * TU-Berlin HRTF set.
 *
 * References:
 * Hagen Wierstorf, Matthias Geier, Alexander Raake and Sascha Spors,
 * "A Free Database of Head-Related Impulse Response Measurements
 * in the Horizontal Plane with Multiple Distances",
 * 130th Convention of the Audio Engineering Society, May 2011.
 */

#include "aave.h"

extern const float hrtf_tub_set[720][4096];

/**
 * Select the HRTF pair for the specified azimuth angle.
 */
static void aave_hrtf_tub_get(const float *hrtf[2], int elevation, int azimuth)
{
	azimuth = -azimuth;
	if (azimuth < 0)
		azimuth += 360;
	hrtf[0] = hrtf_tub_set[azimuth * 2 + 0]; /* left */
	hrtf[1] = hrtf_tub_set[azimuth * 2 + 1]; /* right */
}

/**
 * Use the TU-Berlin HRTF set for the binauralisation of anechoic sounds.
 */
void aave_hrtf_tub(struct aave *a)
{
	a->hrtf_frames = 1024;
	a->hrtf_get = aave_hrtf_tub_get;
	a->hrtf_output_buffer_index = 0;
}
