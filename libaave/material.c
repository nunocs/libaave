/*
 * libaave/material.c: material absortion coefficient filtering
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <math.h>	/* M_PI, cos(), sin() */
#include <string.h>	/* strcmp() */
#include "aave.h"

/* Create the dft() function to convert coefficients (floats) to frequency. */
#define DFT_TYPE float
#include "dft.h"

/* Create the idft() function to convert frequency to coefficients (floats). */
#define IDFT_TYPE float
#include "idft.h"

/*
 * The legth of the filter to design.
 * Should be the same length as the smallest HRIR (MIT = 128).
 */
#define N 128

/*
 * Table of materials, ordered by name.
 *
 * Reference:
 * "Auralization: Fundamentals of Acoustics, Modelling,
 * Simulation, Algorithms and Acoustic Virtual Reality",
 * Annex - Tables of random-incidence absorption coefficients, alpha,
 * Michael Vorlander, 2008.
 */
static const struct aave_material aave_materials[] = {
	{ "carpet", { 92, 92, 70, 40, 25, 20, 20 } },
	{ "concrete", { 99, 97, 95, 98, 98, 98, 98 } },
	{ "cotton_curtains", { 70, 55, 35, 44, 41, 29, 29 } },
	{ "glass_window", { 90, 95, 96, 97, 97, 97, 97 } },
	{ "thin_plywood", { 58, 79, 90, 92, 94, 94, 94 } },
};

/*
 * If none of the above materials match, return this full reflective one.
 */
const struct aave_material aave_material_none = {
	0, { 100, 100, 100, 100, 100, 100, 100 }
};

/*
 * Find the material with the specified name.
 * Binary search algorithm (table of materials must be ordered by name).
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
	printf("];\n");
}
static void print_vec(const float *y, unsigned n)
{
	unsigned i;

	printf("vec%u=[", n);
	for (i = 0; i < n; i++)
		printf(" %g", y[i]);
	printf("];\n");
}
#else
#define print_dft(x,n)
#define print_vec(h,n)
#endif

/*
 * Design the material absorption filter for the coefficients specified in K.
 * The filter DFT coefficients are stored in X, which must have
 * at least 4 * N elements to account for the zero-padding,
 * and where N is the size of the HRIRs of the HRTF currently in use.
 */
static void aave_material_filter(const float *k, float *x, unsigned n)
{
	static const unsigned short fc[AAVE_MATERIAL_COEFFICIENTS] = {
		125, 250, 500, 1000, 2000, 4000, 8000
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
		if (f <= fc[0])
			mag = k[0];
		else if (f >= fc[AAVE_MATERIAL_COEFFICIENTS - 1])
			mag = k[AAVE_MATERIAL_COEFFICIENTS - 1];
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

	print_vec(k, AAVE_MATERIAL_COEFFICIENTS);
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

/*
 * Design the material absorption filter for the
 * specified combination of surfaces and reflection order.
 * The filter DFT coefficients is atored in FILTER, which must have
 * 4 times the elements of the HRIRs of the HRTF set currently in use.
 */
void aave_get_material_filter(struct aave *aave,
				struct aave_surface **surfaces,
				unsigned reflections,
				float *filter)
{
	float k[AAVE_MATERIAL_COEFFICIENTS];
	const unsigned char *c;
	unsigned i, j;

	/* Initialise the total coefficients to be 100% reflective. */
	for (i = 0; i < AAVE_MATERIAL_COEFFICIENTS; i++)
		k[i] = 1;

	/* Multiply the coefficients of the materials of all surfaces. */
	for (i = 0; i < reflections; i++) {
		c = surfaces[i]->material->coefficients;
		for (j = 0; j < AAVE_MATERIAL_COEFFICIENTS; j++)
			k[j] *= c[j] * 0.01;
	}

	/* Generate a filter for hrtf_frames in the frequency domain. */
	aave_material_filter(k, filter, aave->hrtf_frames);
}
