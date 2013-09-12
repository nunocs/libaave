/*
 * libaave/init.c: initialisation routines
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <string.h> /* memset() */
#include "aave.h"

/*
 * Initialise the auralisation data structure.
 *
 * The listener's initial position is (0, 0, 0).
 *
 * The listener's head initial orientation is invalid!
 * You must call aave_set_listener_orientation()!
 */
void aave_init(struct aave *aave)
{
	memset(aave, 0, sizeof *aave);
}

void aave_init_source(struct aave *aave, struct aave_source *source)
{
	memset(source, 0, sizeof *source);
	source->aave = aave;
}
