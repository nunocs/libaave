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
 *   libaave/tests/material.c: test the material absorption filter design
 */

#include <stdio.h>	/* printf() */
#include <stdlib.h>	/* malloc() */
#include "../../libaave/aave.h"

/* The number of coefficients of the filter. */
#define N 128

#define VOLUME 5000
#define AREA 5000
#define RT60 5000

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
	unsigned i, j, n;
	float x[N * 4];

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
#if 1
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
