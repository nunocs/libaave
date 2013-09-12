/*
 * libaave/obj.c: read room model from Wavefront .obj file
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * References:
 * Wavefront .obj file: http://en.wikipedia.org/wiki/Wavefront_.obj_file
 */

#include <stdio.h> /* fgets, fopen, sscanf */
#include <stdlib.h> /* malloc */
#include <string.h> /* strncmp */
#include "aave.h"

/*
 * Maximum number of vertices supported.
 * TODO: we could remove this limitation using dynamic memory allocation.
 */
#define MAX_VERTICES 1024

/*
 * Add the polygons in the obj file FILENAME to the auralization world.
 */
void aave_read_obj(struct aave *aave, const char *filename)
{
	FILE *f;
	float x, y, z;
	char s[256]; /* FIXME: hardcoded limit of character string length. */
	float vertices[MAX_VERTICES][3];
	int vertex_count;
	struct aave_surface *surface;
	unsigned i, j;
	int index;

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
			surface->npoints = 0;
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
		}
	}

	fclose(f);
}
