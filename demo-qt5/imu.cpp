/*
 * demo-qt5/imu.cpp: InterSense InertiaCube3 Inertial Measurement Unit
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <math.h>
#include <isense.h>
#include "imu.h"

Imu::Imu() : QThread()
{
	start(QThread::HighPriority);
}

void Imu::run()
{
	float roll, pitch, yaw; /* radians */

	/* Open Port1 specified in the isports.ini file. */
	ISD_TRACKER_HANDLE handle = ISD_OpenTracker(0, 1, 0, 0);
	if (handle == (ISD_TRACKER_HANDLE)-1)
		return;

	ISD_TRACKING_DATA_TYPE data;
	for (;;) {
		/* Read data at about 50Hz (20ms interval). */
		msleep(20);
		if (ISD_GetTrackingData(handle, &data)) {
			yaw = data.Station[0].Euler[0] * (M_PI / 180);
			pitch = data.Station[0].Euler[1] * (M_PI / 180);
			roll = data.Station[0].Euler[2] * (M_PI / 180);
			emit orientation(roll, pitch, yaw);
		}
	}
}
