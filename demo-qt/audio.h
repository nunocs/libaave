/*
 * demo-qt5/audio.h: audio output thread
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QThread>

class Audio : public QThread
{
public:
	Audio(struct aave *);

protected:
	void run();

private:
	struct aave *aave;
};
