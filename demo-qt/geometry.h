/*
 * demo-qt5/geometry.h: perform the auralisation geometry updates
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QThread>

class Geometry : public QThread
{
public:
	Geometry(struct aave *);

protected:
	void run();

private:
	struct aave *aave;
};
