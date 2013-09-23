/*
 * demo-qt5/geometry.cpp: perform the auralisation geometry updates
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include "geometry.h"

extern "C" {
#include <aave.h>
}

Geometry::Geometry(struct aave *aave)
	: QThread()
	, aave(aave)
{
	start(QThread::LowestPriority);
}

void Geometry::run()
{
	for (;;) {
		/* Perform the updates at most at 5Hz (200ms interval). */
		msleep(200);
		aave_update(aave);
	}
}
