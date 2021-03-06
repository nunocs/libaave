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
 *   libaave/hrtf_tub.c: audio processing with TU-Berlin HRTF set
 */

/**
 * @file hrtf_tub.c
 *
 * The hrtf_tub.c file implements the interface to use the TU-Berlin HRTF
 * set. To select this set for the auralisation process, call aave_hrtf_tub()
 * after initialising the aave structure and before calling aave_put_audio().
 *
 * The TU-Berlin HRTF set consists of head-related impulse responses (HRIR)
 * of a KEMAR manikin at 3m, 2m, 1m and 0.5m distances, in the horizontal
 * plane (elevation 0 only), all 360 degrees in azimuth in steps of 1 degree.
 * This interface admits using one of the distance sets at a time, chosen
 * at compile time.
 *
 * Each HRIR is 2048 samples long at 44100Hz (~46.4ms). For computational
 * efficiency (memory and processing), this interface only uses the first
 * 1024 samples.
 *
 * References:
 * Hagen Wierstorf, Matthias Geier, Alexander Raake and Sascha Spors,
 * "A Free Database of Head-Related Impulse Response Measurements
 * in the Horizontal Plane with Multiple Distances",
 * 130th Convention of the Audio Engineering Society, May 2011.
 */

#include "aave.h"

/** The HRTF set, generated by tools/hrtf_mit_tub.c (distance 2m). */
extern const float hrtf_tub_set[720][4096];

/**
 * Get the closest HRTF pair for the specified coordinates.
 * @p elevation is [-90;90] degrees. @p azimuth is [-180;180] degrees.
 * @p hrtf[0] will be the left HRTF, @p hrtf[1] the right HRTF.
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
 * Select the TU-Berlin HRTF set for the auralisation process.
 */
void aave_hrtf_tub(struct aave *a)
{
	a->hrtf_frames = 1024;
	a->hrtf_get = aave_hrtf_tub_get;
	a->hrtf_output_buffer_index = 0;
}
