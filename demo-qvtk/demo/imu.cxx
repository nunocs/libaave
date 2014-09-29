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
 *   demo-qvtk/imu.cpp: InterSense InertiaCube3 Inertial Measurement Unit
 */

#include <../isense/isense.h>
#include "imu.h"
#include "global.h"

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
        msleep(50);

        if (ISD_GetTrackingData(handle, &data)) {

            yaw = data.Station[0].Euler[0] * (M_PI / 180);
            pitch = data.Station[0].Euler[1] * (M_PI / 180);
            roll = data.Station[0].Euler[2] * (M_PI / 180);

            globalYawangle = yaw * (180/M_PI) + yawOffset;
            globalPitchangle = pitch * (180/M_PI) + pitchOffset;
            globalRollangle = roll * (180/M_PI) + rollOffset;
        }
    }
}
