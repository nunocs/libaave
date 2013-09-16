/*
 * demo-qt5/audio.cpp: audio output using the Qt multimedia module
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QAudioFormat>
#include <QAudioOutput>
#include <QFile>
#include <QIODevice>
#include "audio.h"

extern "C" {
#include <aave.h>
}

class AudioIODevice : public QIODevice
{
public:
	AudioIODevice(struct aave *aave);

protected:
	qint64 readData(char *, qint64);
	qint64 writeData(const char *, qint64);

private:
	struct aave *aave;
	unsigned bufhead;
	unsigned bufsize;
	short *buf;
};

AudioIODevice::AudioIODevice(struct aave *aave)
	: QIODevice()
	, aave(aave)
	, bufhead(0)
{
	/* Read the source.raw file (44100Hz 16 bits mono) into a buffer. */
	QFile file("source.raw");
	if (!file.open(QIODevice::ReadOnly)) {
		fprintf(stderr, "Error opening source.raw\n");
		exit(1);
	}
	bufsize = file.size() / 2;
	buf = new short[bufsize];
	memset(buf, 0, bufsize * sizeof(short));
	if (file.read((char *)buf, file.size()) != file.size()) {
		fprintf(stderr, "Error reading source.raw\n");
		exit(1);
	}
	file.close();
}

qint64 AudioIODevice::readData(char *data, qint64 maxSize)
{
	struct aave_source *source;
	unsigned frames;
	qint64 i, n;

	/*
	 * Put this number of frames of the anechoic sound buffer
	 * into the sound source buffer (loop if necessary).
	 */
	frames = maxSize / 4;
	source = aave->sources;
	for (i = 0; i < frames; i += n) {
		n = frames - i;
		if (n > bufsize - bufhead)
			n = bufsize - bufhead;
		aave_put_audio(source, buf + bufhead, n);
		bufhead += n;
		if (bufhead == bufsize)
			bufhead = 0;
	}

	/* Get this number of frames of binaural data from the engine. */
	aave_get_audio(aave, (short *)data, frames);

	return maxSize;
}

qint64 AudioIODevice::writeData(const char * /*data*/, qint64 /*maxSize*/)
{
	return 0;
}

Audio::Audio(struct aave *aave)
	: QThread()
	, aave(aave)
{
	start(QThread::HighestPriority);
}

void Audio::run()
{
	AudioIODevice *io = new AudioIODevice(aave);
	io->open(QIODevice::ReadOnly);

	QAudioFormat format;
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setChannelCount(2);
	format.setCodec("audio/pcm");
	format.setSampleRate(44100);
	format.setSampleSize(16);
	format.setSampleType(QAudioFormat::SignedInt);

	QAudioOutput *output = new QAudioOutput(format);
	output->start(io);

	exec();
}
