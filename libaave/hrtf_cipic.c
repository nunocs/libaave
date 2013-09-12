/*
 * libaave/hrtf_cipic.c: audio processing with CIPIC HRTF set
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include "aave.h"

/* The specific CIPIC set to use. */
#define hrtf_cipic_set hrtf_cipic_set_008
extern const float hrtf_cipic_set_008[][512];

/*
 * Select the HRTF pair for the specified azimuth and elevation angles.
 * Elevation is [-90;90] degrees. Azimuth is [-180;180] degrees.
 */
static void aave_hrtf_cipic_get(const float *hrtf[2], int elevation, int azimuth)
{
	if (azimuth <= -80) {
		hrtf[0] = hrtf_cipic_set[0];
		hrtf[1] = hrtf_cipic_set[1];
	} else if (azimuth <= -65) {
		hrtf[0] = hrtf_cipic_set[2];
		hrtf[1] = hrtf_cipic_set[3];
	} else if (azimuth <= -55) {
		hrtf[0] = hrtf_cipic_set[4];
		hrtf[1] = hrtf_cipic_set[5];
	} else if (azimuth <= -45) {
		hrtf[0] = hrtf_cipic_set[6];
		hrtf[1] = hrtf_cipic_set[7];
	} else if (azimuth <= -40) {
		hrtf[0] = hrtf_cipic_set[8];
		hrtf[1] = hrtf_cipic_set[9];
	} else if (azimuth <= -35) {
		hrtf[0] = hrtf_cipic_set[10];
		hrtf[1] = hrtf_cipic_set[11];
	} else if (azimuth <= -30) {
		hrtf[0] = hrtf_cipic_set[12];
		hrtf[1] = hrtf_cipic_set[13];
	} else if (azimuth <= -25) {
		hrtf[0] = hrtf_cipic_set[14];
		hrtf[1] = hrtf_cipic_set[15];
	} else if (azimuth <= -20) {
		hrtf[0] = hrtf_cipic_set[16];
		hrtf[1] = hrtf_cipic_set[17];
	} else if (azimuth <= -15) {
		hrtf[0] = hrtf_cipic_set[18];
		hrtf[1] = hrtf_cipic_set[19];
	} else if (azimuth <= -10) {
		hrtf[0] = hrtf_cipic_set[20];
		hrtf[1] = hrtf_cipic_set[21];
	} else if (azimuth <= -5) {
		hrtf[0] = hrtf_cipic_set[22];
		hrtf[1] = hrtf_cipic_set[23];
	} else if (azimuth <= 0) {
		hrtf[0] = hrtf_cipic_set[22];
		hrtf[1] = hrtf_cipic_set[23];
	} else if (azimuth <= 5) {
		hrtf[0] = hrtf_cipic_set[23];
		hrtf[1] = hrtf_cipic_set[22];
	} else if (azimuth <= 10) {
		hrtf[0] = hrtf_cipic_set[21];
		hrtf[1] = hrtf_cipic_set[20];
	} else if (azimuth <= 15) {
		hrtf[0] = hrtf_cipic_set[19];
		hrtf[1] = hrtf_cipic_set[18];
	} else if (azimuth <= 20) {
		hrtf[0] = hrtf_cipic_set[17];
		hrtf[1] = hrtf_cipic_set[16];
	} else if (azimuth <= 25) {
		hrtf[0] = hrtf_cipic_set[15];
		hrtf[1] = hrtf_cipic_set[14];
	} else if (azimuth <= 30) {
		hrtf[0] = hrtf_cipic_set[13];
		hrtf[1] = hrtf_cipic_set[12];
	} else if (azimuth <= 35) {
		hrtf[0] = hrtf_cipic_set[11];
		hrtf[1] = hrtf_cipic_set[10];
	} else if (azimuth <= 40) {
		hrtf[0] = hrtf_cipic_set[9];
		hrtf[1] = hrtf_cipic_set[8];
	} else if (azimuth <= 45) {
		hrtf[0] = hrtf_cipic_set[7];
		hrtf[1] = hrtf_cipic_set[6];
	} else if (azimuth <= 55) {
		hrtf[0] = hrtf_cipic_set[5];
		hrtf[1] = hrtf_cipic_set[4];
	} else if (azimuth <= 65) {
		hrtf[0] = hrtf_cipic_set[3];
		hrtf[1] = hrtf_cipic_set[2];
	} else { /* azimuth <= 80 */
		hrtf[0] = hrtf_cipic_set[1];
		hrtf[1] = hrtf_cipic_set[0];
	}
}

void aave_hrtf_cipic(struct aave *a)
{
	a->hrtf_frames = 256;
	a->hrtf_get = aave_hrtf_cipic_get;
	a->hrtf_output_buffer_index = 0;
}
