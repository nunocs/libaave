/*
 * libaave/tests/material.c: test the material absorption filter design
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <stdio.h>	/* printf() */
#include <stdlib.h>	/* malloc() */
#include "../../libaave/aave.h"

/* The number of coefficients of the filter. */
#define N 128

/* The combination of materials. */
static const char *material_names[] = {
//	"glass_window",
//	"thin_plywood",
	"carpet",
//	"cotton_curtains",
};

int main()
{
	struct aave *aave;
	struct aave_surface **surfaces;
	unsigned i, n;
	float x[N * 4];

	/* Create and initialise the auralisation engine. */
	aave = malloc(sizeof *aave);
	aave_init(aave);

	/* Set the HRTF size to use. */
	aave->hrtf_frames = N;

	/* Create the surfaces of the specified materials. */
	n = sizeof material_names / sizeof *material_names;
	surfaces = malloc(n * sizeof *surfaces);
	for (i = 0; i < n; i++) {
		surfaces[i] = malloc(sizeof **surfaces);
		surfaces[i]->material = aave_get_material(material_names[i]);
	}

	/* Design the total material absortion filter. */
	aave_get_material_filter(aave, surfaces, n, x);
#if 0
	/* Print the frequency response of the designed filter. */
	printf("x = [ %e", x[0]);
	for (i = 1; i < N * 2; i++) {
		j = dft_index(i, N * 4);
		printf(", %e + %e i", x[j * 2 + 0], x[j * 2 + 1]);
	}
	printf(", %e", x[1]);
	for (i = 1; i < N * 2; i++) {
		j = dft_index(N * 2 - i, N * 4);
		printf(", %e - %e i", x[j * 2 + 0], x[j * 2 + 1]);
	}
	printf(" ];\n");
#endif
	return 0;
}
