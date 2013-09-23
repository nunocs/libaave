/*
 * demo-qt5/imu.h: Inertial Measurement Unit
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QThread>

class Imu : public QThread
{
	Q_OBJECT

public:
	Imu();

protected:
	void run();

signals:
	void orientation(float roll, float pitch, float yaw);
};
