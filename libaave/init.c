/*
 * libaave/init.c: initialisation routines
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
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
	aave->reflections = 0;
	aave->room_material_absorption = 0;

	for (surface = aave->surfaces; surface; surface = surface->next) {
		aave->room_material_absorption += surface->avg_absorption_coef / aave->nsurfaces;
	}

	/*while (nsources--) {
		printf("init 1 source\n");
		source = (struct aave_source *)malloc(sizeof *source);
    	aave_init_source(aave, source);
    	aave_add_source(aave, source);
	}*/

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
