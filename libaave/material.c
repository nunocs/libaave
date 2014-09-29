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
 *   libaave/material.c: material absorption filtering
 */

/**
 * @file material.c
 *
 * The material.c file contains the table of materials, with their acoustic
 * reflection factors by frequency band, and the functions to design the
 * material absorption audio filters to apply those reflection factors.
 *
 * The filtering, implemented in audio.c, is performed in the frequency
 * domain, taking advantage of the fact that the sounds are already
 * converted to the frequency domain anyway to perform the HRTF filtering.
 * This allows for the use of material absorption filters with linear
 * phase (best audio quality) and long impulse response (best frequency
 * resolution). The fixed delay induced by the length of the filter can be
 * easily compensated upstream in the auralisation process, if needed.
 *
 * Calculating the Fourier coefficients of the filters is thus implemented
 * using the technique of filter design by frequency sampling, shown in:
 * Udo Zolzer, "Digital Audio Signal Processing", 2nd Edition, Wiley,
 * Section 5.3.3 Filter Design by Frequency Sampling.
 */

#include <math.h>	/* M_PI, cos(), sin() */
#include <string.h>	/* strcmp() */
#include <stdio.h>
#include "aave.h"

/**
 * Create the dft() function to convert filter coefficients
 * to Fourier coefficients.
 */
#define DFT_TYPE float
#include "dft.h"

/**
 * Create the idft() function to convert Fourier coefficients
 * to filter coefficients.
 */
#define IDFT_TYPE float
#include "idft.h"

/**
 * The legth of the filter to design.
 * Should be the same length as the smallest HRIR (MIT = 128).
 */
#define N 128

/**
 * Table of materials, ordered by name.
 *
 * The reflection factors are calculated from random-incidence absorption
 * coefficient alpha values using the equation: r = sqrt(1 - alpha).
 *
 * Reference:
 * "Auralization: Fundamentals of Acoustics, Modelling, Simulation,
 * Algorithms and Acoustic Virtual Reality", Michael Vorlander, 2008,
 * Annex - Tables of random-incidence absorption coefficients, alpha,
 * and Section 11.3.2 Audability test.
 */
static const struct aave_material aave_materials[] = {
	{ "carpet", { 96, 96, 84, 63, 50, 45, 45 } },
	{ "concrete", { 99, 98, 97, 99, 99, 99, 99 } },
	{ "cotton_curtains", { 84, 74, 59, 66, 64, 54, 54 } },
	{ "glass_window", { 95, 97, 98, 98, 98, 98, 98 } },
	{ "thin_plywood", { 76, 89, 95, 96, 97, 97, 97 } },
};

/**
 * Full reflective material to use when no material is specified for a
 * surface, or when the specified material is not found in aave_materials.
 */
const struct aave_material aave_material_none = {
	0, { 100, 100, 100, 100, 100, 100, 100 }
};

/**
 * Return the material with the specified @p name.
 * If no material is found with such name, return aave_material_none.
 *
 * The search is performed using the binary search algorithm
 * (http://en.wikipedia.org/wiki/Binary_search_algorithm),
 * that's why the table of materials must be ordered by name.
 */
const struct aave_material *aave_get_material(const char *name)
{
	unsigned a, b, i;
	int x;

	a = 0;
	b = sizeof aave_materials / sizeof *aave_materials;
	do {
		i = a + (b - a) / 2;
		x = strcmp(name, aave_materials[i].name);
		if (x == 0)
			return &aave_materials[i];
		if (x < 0)
			b = i;
		else
			a = i + 1;
	} while (a != b);

	return &aave_material_none;
}

/* Set to 1 to print the filter coefficients to stdout, for octave. */
#if 0
#include <stdio.h>
static void print_dft(const float *x, unsigned n)
{
	unsigned i, j;

	printf("dft%u=[%g", n, x[0]);
	for (i = 1; i < n / 2; i++) {
		j = dft_index(i, n);
		printf(" %g+j*%g", x[j*2], x[j*2+1]);
	}
	printf(" %g", x[1]);
	for (i = n / 2 + 1; i < n; i++) {
		j = dft_index(n - i, n);
		printf(" %g-j*%g", x[j*2], x[j*2+1]);
	}
	printf("];\n\n");
}
static void print_vec(const float *y, unsigned n)
{
	unsigned i;

	printf("vec%u=[", n);
	for (i = 0; i < n; i++)
		printf(" %g", y[i]);
	printf("];\n\n");
}
#else
#define print_dft(x,n)
#define print_vec(h,n)
#endif

/**
 * Design the material absorption filter for the reflection factors @p k.
 * The calculated DFT coefficients of the filter are stored in @p x, which
 * must have at least 4 * @p n elements to account for the zero-padding,
 * and where @p n is the size of the HRIRs of the HRTF currently in use.
 *
 * Reference:
 * Udo Zolzer, "Digital Audio Signal Processing", 2nd Edition, Wiley,
 * Section 5.3.3 Filter Design by Frequency Sampling.
 */
static void aave_material_filter(const float *k, float *x, unsigned n)
{
	/*static const unsigned short fc[AAVE_MATERIAL_REFLECTION_FACTORS] = {
		125, 250, 500, 1000, 2000, 4000, 8000
	}; */

	/* upper limits for frequency bands */
	static const unsigned short fc[AAVE_MATERIAL_REFLECTION_FACTORS] = {
		177, 355, 710, 1420, 2840, 5680, 11360
	};
	unsigned i, j, w;
	float f, mag, arg, real, imag, a;
	float y[AAVE_MAX_HRTF * 2];

	x[0] = k[0];	/* X[0] = k[0] + j 0 */
	x[1] = 0;	/* X[N/2] = 0 + j 0 */

	j = 1;
	for (i = 1; i < N / 2; i++) {
		f = i * ((float)AAVE_FS / N);
		/* Calculate the magnitude (linear interpolation). */
		if (f <= fc[0]) {
			mag = k[0];
		}
		else if (f >= fc[AAVE_MATERIAL_REFLECTION_FACTORS - 1]) {
			mag = k[AAVE_MATERIAL_REFLECTION_FACTORS - 1];
		}
		else {
			while (f > fc[j])
				j++;
			a = (fc[j] - f) / (fc[j] - fc[j - 1]);
			mag = k[j - 1] * a + k[j] * (1 - a);
		}

		/* Calculate the phase (linear phase). */
		arg = - M_PI * (N - 1) / N * i;

		/* Obtain the complex coefficient. */
		real = mag * cos(arg);
		imag = mag * sin(arg);

		/* Convert to real and imaginary coordinates. */
		w = dft_index(i, N);
		x[w * 2 + 0] = real;
		x[w * 2 + 1] = imag;
	}

	print_vec(k, AAVE_MATERIAL_REFLECTION_FACTORS);
	print_dft(x, N);

	/* Convert filter frequency response to time-domain coefficients. */
	idft(y, x, N);

	print_vec(y, N);

	/*
	 * Zero-pad to 4 times the size of the HRTF set currently selected.
	 * We only need to zero-pad to 2 times, since the dft() function
	 * already further assumes the data is zero-padded to 2 times.
	 */
	memset(y + N, 0, (2 * n - N) * sizeof(float));

	/* Convert time-domain zero-padded coefficients to frequency. */
	dft(x, y, 4 * n);
}

/**
 * Design the material absorption filter for the specified sequence of
 * @p surfaces and reflection order @p reflections. The calculated DFT
 * coefficients of the filter are stored in @p filter, which must have
 * 4 times the elements of the HRIRs of the HRTF set currently in use.
 */
void aave_get_material_filter(struct aave *aave,
				struct aave_surface **surfaces,
				unsigned reflections,
				float *filter)
{
	float k[AAVE_MATERIAL_REFLECTION_FACTORS];
	const unsigned char *c;
	unsigned i, j;

	/* Initialise the total coefficients to be 100% reflective. */
	for (i = 0; i < AAVE_MATERIAL_REFLECTION_FACTORS; i++)
		k[i] = 1;

	/* Multiply the coefficients of the materials of all surfaces. */
	for (i = 0; i < reflections; i++) {
		c = surfaces[i]->material->reflection_factors;
		for (j = 0; j < AAVE_MATERIAL_REFLECTION_FACTORS; j++)
			k[j] *= c[j] * 0.01;
	}

	/* Generate a filter for hrtf_frames in the frequency domain. */
	aave_material_filter(k, filter, aave->hrtf_frames);
}
