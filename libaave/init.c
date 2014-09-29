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
 *   Copyright 2013,2014 André Oliveira, Nuno Silva, Guilherme Campos,
 *   Paulo Dias, José Vieira/IEETA - Universidade de Aveiro
 *
 *
 *   libaave/init.c: initialisation routines
 */

/**
 * @file init.c
 *
 * The init.c file contains the functions to initialise the data structures
 * used in the AcousticAVE library.
 */

#include <string.h> /* memset() */
#include "aave.h"
#include "stdio.h"

/**
 * Initialise the auralisation data structure.
 *
 * The listener's initial position is (0, 0, 0).
 *
 * The listener's head initial orientation is invalid!
 * You must call aave_set_listener_orientation()!
 *
 * The initial output gain is 1 (0dB).
 *
 * The artificial reverberation tail is initially enabled.
 */
void aave_init(struct aave *aave)
{
	
	struct aave_surface *surface;

	aave->gain = 1;
	aave->reflections = 3;
	aave->room_material_absorption = 0;

	for (surface = aave->surfaces; surface; surface = surface->next) 
		aave->room_material_absorption += surface->avg_absorption_coef / aave->nsurfaces;

	aave->reverb = (struct aave_reverb*) malloc(sizeof(struct aave_reverb));

	aave->reverb->pre_delay = aave->hrtf_frames * 2;
	init_reverb(aave->reverb, aave->volume, aave->area, aave->room_material_absorption, aave->rt60);
	print_reverb_parameters(aave,aave->reverb);
}

/**
 * Initialise a sound source data structure to be used by the aave engine.
 */
void aave_init_source(struct aave *aave, struct aave_source *source)
{
	memset(source, 0, sizeof *source);
	source->aave = aave;
}
