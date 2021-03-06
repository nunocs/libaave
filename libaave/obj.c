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
 *   libaave/obj.c: read room model from Wavefront .obj file
 */

/**
 * @file obj.c
 * Read room model from Wavefront .obj file.
 *
 * The following elements of the .obj specification are supported:
 * v (vertex), f (face), usemtl (material name).
 * All other elements are ignored, as they are irrelevant for auralisation.
 *
 * The material name in the usemtl element specify the material of
 * the @ref aave_materials table to use for the succeeding face.
 *
 * References:
 * Wavefront .obj file: http://en.wikipedia.org/wiki/Wavefront_.obj_file
 */

#include <stdio.h> /* fgets, fopen, sscanf */
#include <stdlib.h> /* malloc */
#include <string.h> /* strncmp */
#include "aave.h"

/**
 * Maximum number of vertices in the .obj file.
 * @todo Use dynamic memory allocation for the array of vertices
 * to support "unlimited" number of vertices.
 */
#define MAX_VERTICES 1024

/**
 * Read the .obj file @p filename and add its contents to the
 * auralisation engine @p aave.
 */
void aave_read_obj(struct aave *aave, const char *filename)
{
	FILE *f;
	float x, y, z;
	char s[256];
	float vertices[MAX_VERTICES][3];
	int vertex_count;
	struct aave_surface *surface;
	unsigned i, j;
	int index;
	char material_name[128];
	const struct aave_material *material = &aave_material_none;

	f = fopen(filename, "r");
	if (!f)
		return;

	vertex_count = 0;

	while (fgets(s, sizeof s, f)) {
		if (sscanf(s, "v %f %f %f", &x, &y, &z) == 3) {
			/* Add vertex. */
			if (vertex_count < MAX_VERTICES) {
				vertices[vertex_count][0] = x;
				vertices[vertex_count][1] = y;
				vertices[vertex_count][2] = z;
				vertex_count++;
			}
		} else if (!strncmp(s, "f ", 2)) {
			/* Add face. */
			surface = malloc(sizeof *surface);
			if (!surface)
				continue;
			surface->material = material;
			surface->npoints = 0;

			surface->avg_absorption_coef = 0;
			for (i = 0; i < AAVE_MATERIAL_REFLECTION_FACTORS; i++) {
				surface->avg_absorption_coef += (0.01 * material->reflection_factors[i]) / AAVE_MATERIAL_REFLECTION_FACTORS;
			}

			for (i = 0; s[i]; i++) {
				if (s[i] != ' ')
					continue;
				if (sscanf(&s[i], "%d", &index) != 1)
					break;
				if (index > 0 && index <= vertex_count)
					index--;
				else if (index < 0 && -index <= vertex_count)
					index += vertex_count;
				else
					continue;
				for (j = 0; j < 3; j++)
					surface->points[surface->npoints][j] =
							vertices[index][j];
				surface->npoints++;
			}
			aave_add_surface(aave, surface);
		} else if (sscanf(s, "usemtl %s", material_name) == 1) {
			material = aave_get_material(material_name);
		}
	}

	fclose(f);
}
