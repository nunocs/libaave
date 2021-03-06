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
 *   libaave/geometry.c: geometry processing
 */

/**
 * @file geometry.c
 *
 * The geometry.c file contains the functions that implement the geometry
 * part of the auralisation process, based on the image source model shown
 * in: "Auralization: Fundamentals of Acoustics, Modelling, Simulation,
 * Algorithms and Acoustic Virtual Reality", Michael Vorlander, 2008,
 * Section 11.3 Image source model.
 *
 * At startup, the aave_add_surface() function is called for each surface,
 * and the aave_add_source() function is called for each sound source,
 * to build the auralisation world.
 *
 * At runtime, the aave_set_listener_position() or aave_set_source_position()
 * functions are called when the listener or sound sources move,
 * followed by aave_update() to perform all geometric calculations
 * to discover the audible sounds for the new positions, and the
 * aave_set_listener_orientation() function is called when the
 * listener moves her head.
 */

#include <math.h> /* M_PI, acos(), atan2(), sqrt() */
#include <stdlib.h> /* malloc() */
#include "aave.h"

/**
 * Calculate the dot product a . b
 */
static float dot_product(const float a[3], const float b[3])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/**
 * Calculate the cross product n = a x b.
 */
static void cross_product(float n[3], const float a[3], const float b[3])
{
	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * Calculate the norm of a vector.
 */
static float norm(const float x[3])
{
	return sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}

/**
 * "Normalise" a vector (divide it by its norm): y = x / norm(x)
 */
static void normalise(float y[3], const float x[3])
{
	float n;
	unsigned i;

	n = norm(x);

	for (i = 0; i < 3; i++)
		y[i] = x[i] / n;
}

/**
 * Calculate the position [x,y,z] of the image-source of the sound source
 * at position @p source created by the surface pointed by @p surface.
 * The position of the image-source is returned in @p image.
 *
 * Reference: "Auralization: Fundamentals of Acoustics, Modelling,
 * Simulation, Algorithms and Acoustic Virtual Reality",
 * Michael Vorlander, 2008, Section 11.3.1 Classical model.
 */
static void aave_image_source(const struct aave_surface *surface,
				const float source[3], float image[3])
{
	float d;
	unsigned i;

	/* Minimum distance from point x to the plane of the polygon. */
	d = dot_product(surface->normal, source) + surface->distance;

	/*
	 * The image-source position is 2 times that distance,
	 * in the opposite direction.
	 */
	d *= 2;
	for (i = 0; i < 3; i++)
		image[i] = source[i] - d * surface->normal[i];
}

/**
 * Calculate the coordinates @p y [x,y] of the point @p x [x,y,z]
 * in the local coordinates of the surface pointed by @p surface.
 */
static void local_coordinates(float y[2], const float x[3],
					const struct aave_surface *surface)
{
	float u[3];
	unsigned i;

	for (i = 0; i < 3; i++)
		u[i] = x[i] - surface->points[0][i];
	for (i = 0; i < 2; i++)
		y[i] = dot_product(u, surface->versors[i]);
}

/**
 * Check if the vector @p v (a line segment from point @p a to point @p b)
 * intersects the surface pointed by @p surface.
 * Returns 0 if false or 1 if true, and the intersection point in @p xyz.
 *
 * Reference: PNPOLY - Point Inclusion in Polygon Test, W. Randolph Franklin,
 * http://www.ecse.rpi.edu/~wrf/Research/Short_Notes/pnpoly.html
 */
static int aave_intersection(const struct aave_surface *surface,
				const float a[3], const float b[3],
				const float v[3], float xyz[3])
{
	float n_dot_v, n_dot_w, s;
	float w[3], x[3], y[2];
	float xi, xj, yi, yj;
	unsigned i, j, count;

	/* Check if the vector is parallel to the surface. */
	n_dot_v = dot_product(surface->normal, v);
	if (n_dot_v == 0)
		return 0;

	/* Vector from point a to the first point of the surface. */
	for (i = 0; i < 3; i++)
		w[i] = surface->points[0][i] - a[i];

	/* Check if the intersection point is outside the a b segment. */
	n_dot_w = dot_product(surface->normal, w);
	s = n_dot_w / n_dot_v;
	if (s <= 0 || s >= 1) /* (s <= -1e-6 || s >= 1 + 1e-6) */
		return 0;

	/*
	 * Intersection point of the line segment and the plane
	 * defined by the polygon surface.
	 */
	for (i = 0; i < 3; i++)
		x[i] = a[i] + s * v[i];

	/* Express the intersection point in surface local coordinates. */
	local_coordinates(y, x, surface);

	/*
	 * See if the intersection point passes through the surface
	 * (pnpoly - point inclusion in polygon test):
	 * count the number of times it crosses the edges in a direction;
	 * if it passes through the surface, this count will be odd.
	 */
	count = 0;
	for (i = 0; i < surface->npoints; i++) {
		j = (i + 1) % surface->npoints;
		xi = surface->points[i][3];
		xj = surface->points[j][3];
		yi = surface->points[i][4];
		yj = surface->points[j][4];
		if (((yi > y[1]) != (yj > y[1])) &&
			(y[0] < (xj-xi)*(y[0]-yi)/(yj-yi)+xi))
				count++;
	}

	/* Copy the intersection point to the user. */
	for (i = 0; i < 3; i++)
		xyz[i] = x[i];

	return count & 1;
}

/**
 * Check if the sound path from point @p a to point @p b is visible.
 * Returns 0 if the line segment b-a is intersected by any surface,
 * or 1 otherwise.
 */
static int aave_is_visible(const struct aave *aave, const float a[3],
							const float b[3])
{
	const struct aave_surface *surface;
	float v[3], x[3];
	unsigned i;

	/* Calculate the vector from point a to point b (line direction). */
	for (i = 0; i < 3; i++)
		v[i] = b[i] - a[i];

	for (surface = aave->surfaces; surface; surface = surface->next)
		if (aave_intersection(surface, a, b, v, x))
			return 0;

	return 1;
}

/**
 * Create the sound path from the source pointed by @source to the listerner,
 * for reflection order @p order, that reflects on the specified sequence of
 * @p surfaces, with corresponding image source positions @p image_sources.
 * The calculated reflection points are stored in @p x.
 * Returns 1 if the sound path is audible, or 0 otherwise.
 */
static int aave_build_sound_path(struct aave *aave, struct aave_source *source,
			unsigned order, struct aave_surface *surfaces[],
			float image_sources[][3], float x[][3])
{
	float *a, *b, v[3];
	unsigned i, j, k;

	b = aave->position;

	for (i = 0; i < order; i++) {
		j = order - i - 1;
		a = image_sources[j];

		/* Calculate the vector from point a to point b. */
		for (k = 0; k < 3; k++)
			v[k] = b[k] - a[k];

		/* See if the line from a to b intersects this surface. */
		if (!aave_intersection(surfaces[j], a, b, v, x[j]))
			return 0;

		/* See if the line from x to b is visible. */
		if (!aave_is_visible(aave, x[j], b))
			return 0;

		/* Set the end of the line for the next iteration. */
		b = x[j];
	}

	/* Finally, see if the line from the source to b is visible. */
	if (!aave_is_visible(aave, source->position, b))
		return 0;

	return 1;
}

/**
 * Create a sound to be auralised by the audio processing.
 * @p source is the sound source that originates the sound,
 * @p order is the reflection order of the sound,
 * @p surfaces is the sequence of surfaces where the sound reflects, and
 * @p image_sources are the positions of the corresponding image-sources.
 */
static void aave_create_sound(struct aave *aave, struct aave_source *source,
			unsigned order, struct aave_surface *surfaces[],
			float image_sources[][3])
{
	unsigned i, j;
	float x[AAVE_MAX_REFLECTIONS][3];
	struct aave_sound *sound;

	/* If the sound path is not visible don't bother creating the sound. */
	if (!aave_build_sound_path(aave, source, order,
					surfaces, image_sources, x))
		return;

	/* First see if this sound path is not already in the sounds list. */
	for (sound = aave->sounds[order]; sound; sound = sound->next) {
		if (sound->source != source)
			continue;
		for (i = 0; i < order; i++)
			if (surfaces[i] != sound->surfaces[i])
				break;
		if (i == order)
			return;
	}

	/* Allocate sound. */
	sound = calloc(1, sizeof *sound);
	if (!sound)
		return;

	/* Copy the geometric data to the sound. */
	for (i = 0; i < order; i++) {
		sound->surfaces[i] = surfaces[i];
		for (j = 0; j < 3; j++) {
			sound->image_sources[i][j] = image_sources[i][j];
			sound->reflection_points[i][j] = x[i][j];
		}
	}

	/* Design the material absortion filter. */
	aave_get_material_filter(aave, sound->surfaces, order, sound->filter);

	/* Set the origin of the sound. */
	if (order == 0)
		sound->position = source->position;
	else
		sound->position = sound->image_sources[order - 1];

	sound->audible = 1;
	sound->source = source;

	/* Add sound to the list of sounds to be auralised. */
	sound->next = aave->sounds[order];
	aave->sounds[order] = sound;
}

/**
 * Recursively create all audible sounds of a given reflection order
 * that originate from a sound source.
 * @p source is the sound source,
 * @p order is the reflection order,
 * @p o is the current reflection order in the recursive process,
 * @p surfaces is the stack of surfaces where the current sound reflects,
 * @p image_sources is the stack of corresponding image source positions.
 *
 * @todo Implement the iterative version of this recursive algorithm.
 */
static void aave_create_sounds_recursively(struct aave *aave,
				struct aave_source *source, unsigned order,
				unsigned o, struct aave_surface *surfaces[],
				float image_sources[][3])
{
	struct aave_surface *surface;

	if (o == order) {
		aave_create_sound(aave, source, order,
						surfaces, image_sources);
		return;
	}

	for (surface = aave->surfaces; surface; surface = surface->next) {
		if (o > 0 && surfaces[o-1] == surface)
			continue;
		if (o > 0)
			aave_image_source(surface, image_sources[o-1],
							image_sources[o]);
		else
			aave_image_source(surface, source->position,
							image_sources[o]);
		surfaces[o] = surface;
		aave_create_sounds_recursively(aave, source,
					order, o+1, surfaces, image_sources);
	}
}

/**
 * Create all audible sounds originated from the specified sound source
 * up to, and including, the specified reflection order.
 */
static void aave_create_sounds(struct aave *aave, struct aave_source *source,
							unsigned order)
{
	struct aave_surface *surfaces[AAVE_MAX_REFLECTIONS];
	float image_sources[AAVE_MAX_REFLECTIONS][3];

	aave_create_sounds_recursively(aave, source, order, 0,
						surfaces, image_sources);
}

/**
 * Update the distance and azimuth of the listener relative to a sound source.
 * Calculate the (distance, elevation, azimuth) vector
 * from the listener-to-source vector (x, y, z)
 * with listener orientation (roll, pitch, yaw).
 * Angles in radians.
 */
static void aave_update_sound(struct aave *aave, struct aave_sound *sound,
							unsigned order)
{
	float *a;
	unsigned i;

	/* Recalculate the position of the image sources. */
	a = sound->source->position;
	for (i = 0; i < order; i++) {
		aave_image_source(sound->surfaces[i], a,
						sound->image_sources[i]);
		a = sound->image_sources[i];
	}

	sound->audible = aave_build_sound_path(aave, sound->source, order,
					sound->surfaces, sound->image_sources,
					sound->reflection_points);
}

/**
 * Get the @p distance (m), @p azimuth (rad) and @p elevation (rad)
 * coordinates of the position @p source_position of a source
 * relative to the listener.
 */
void aave_get_coordinates(const struct aave *aave, const float *source_position, float *distance, float *elevation, float *azimuth)
{
	float vector[3], v[3], dist, phi, theta;
	unsigned i;

	/* Calculate the vector from the listener to the sound origin. */
	for (i = 0; i < 3; i++)
		vector[i] = source_position[i] - aave->position[i];
	dist = norm(vector);

	/* Express the sound path vector in the body frame. */
	for (i = 0; i < 3; i++)
		v[i] = dot_product(aave->orientation[i], vector);

	/* Convert to spherical coordinates (ISO 31-11 convention). */
	phi = atan2f(v[1], v[0]);
	theta = acosf(v[2] / dist);

	/* Convert to azimuth and elevation. */
	*azimuth = - phi;
	*elevation = theta - M_PI / 2;

	*distance = dist;
}

/**
 * Add a sound source to the auralisation world.
 */
void aave_add_source(struct aave *aave, struct aave_source *source)
{
	/* Add source to the list of sources in the auralisation world. */
	source->next = aave->sources;
	aave->sources = source;
}

/**
 * Add a surface to the auralisation world.
 */
void aave_add_surface(struct aave *aave, struct aave_surface *surface)
{
	unsigned i;
	float a[3], b[3], n[3];

	/* Calculate the 2 vectors made by the first 3 points. */
	for (i = 0; i < 3; i++) {
		a[i] = surface->points[1][i] - surface->points[0][i];
		b[i] = surface->points[2][i] - surface->points[0][i];
	}

	/* Calculate the normal of these 2 vectors (n = a x b). */
	cross_product(n, a, b);

	/* Calculate the unit normal vector of the plane of this polygon. */
	normalise(surface->normal, n);

	/* Calculate the distance from the origin. */
	surface->distance = - dot_product(surface->normal, surface->points[0]);

	/*
	 * Calculate 2 versors to define the plane.
	 * One may as well be a = points[1] - points[0], calculated above.
	 * The other is the cross product with the normal.
	 */
	normalise(surface->versors[0], a);
	cross_product(surface->versors[1], surface->normal,
							surface->versors[0]);

	/* Finally, calculate the local coordinates of each point. */
	for (i = 0; i < surface->npoints; i++)
		local_coordinates(&surface->points[i][3], surface->points[i],
								surface);

	/* Add the surface to the auralisation world. */
	surface->next = aave->surfaces;
	aave->surfaces = surface;
	aave->nsurfaces++;
}

/**
 * Set the orientation of the listener's head.
 */
void aave_set_listener_orientation(struct aave *aave,
					float roll, float pitch, float yaw)
{
	float sinr, cosr, sinp, cosp, siny, cosy;

	/* Calculate the inertial-to-body rotation matrix. */
	sinr = sin(roll);
	cosr = cos(roll);
	sinp = sin(pitch);
	cosp = cos(pitch);
	siny = sin(yaw);
	cosy = cos(yaw);
	aave->orientation[0][0] = cosp * cosy;
	aave->orientation[0][1] = - cosr * siny + sinr * sinp * cosy;
	aave->orientation[0][2] = sinr * siny + cosr * sinp * cosy;
	aave->orientation[1][0] = cosp * siny;
	aave->orientation[1][1] = cosr * cosy + sinr * sinp * siny;
	aave->orientation[1][2] = - sinr * cosy + cosr * sinp * siny;
	aave->orientation[2][0] = - sinp;
	aave->orientation[2][1] = sinr * cosp;
	aave->orientation[2][2] = cosr * cosp;
}

/**
 * Set the position of the listener.
 *
 * The aave_update() function should be called afterwards to update the
 * state of the auralisation engine to reflect the new position.
 */
void aave_set_listener_position(struct aave *aave,
					float x, float y, float z)
{
	aave->position[0] = x;
	aave->position[1] = y;
	aave->position[2] = z;
}

/**
 * Set the position of a sound source.
 *
 * The aave_update() function should be called afterwards to update the
 * state of the auralisation engine to reflect the new position.
 */
void aave_set_source_position(struct aave_source *source, float x, float y, float z)
{
	source->position[0] = x;
	source->position[1] = y;
	source->position[2] = z;
}

/**
 * Update the whole state of the auralisation world.
 * Runs the visibility checks for all sounds from all sources.
 */
void aave_update(struct aave *aave)
{
	struct aave_sound *sound;
	struct aave_source *source;
	unsigned i;

	/* First update the sounds that were previously visible. */
	for (i = 0; i <= aave->reflections; i++)
		for (sound = aave->sounds[i]; sound; sound = sound->next)
			aave_update_sound(aave, sound, i);

	/* Then update everything else. */
	for (i = 0; i <= aave->reflections; i++)
		for (source = aave->sources; source; source = source->next)
			aave_create_sounds(aave, source, i);
}
