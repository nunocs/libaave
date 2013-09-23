/*
 * demo-qt5/audio-alsa.cpp: audio output with Advanced Linux Sound Architecture
 *
 * Copyright 2012,2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QFile>
#include "audio.h"

#include <alsa/asoundlib.h>

extern "C" {
#include <aave.h>
}

/* Number of frames to process in each loop. */
#define FRAMES 8192 /* 186ms at 44100Hz */

Audio::Audio(struct aave *aave)
	: QThread()
	, aave(aave)
{
	start(QThread::HighestPriority);
}

void Audio::run()
{
	/* Read the source.raw file (44100Hz 16 bits mono) into a buffer. */
	QFile file("source.raw");
	if (!file.open(QIODevice::ReadOnly)) {
		fprintf(stderr, "Error opening source.raw\n");
		return;
	}
	unsigned bufhead = 0;
	unsigned bufsize = file.size() / 2;
	short *buf = new short[bufsize];
	memset(buf, 0, bufsize * sizeof(short));
	if (file.read((char *)buf, file.size()) != file.size()) {
		fprintf(stderr, "Error reading source.raw\n");
		return;
	}
	file.close();

	/* Open the ALSA audio device. */
	// const char device[] = "hw:Loopback,0,0";
	// const char device[] = "default";
	const char device[] = "hw";
	snd_pcm_t *handle;
	short buffer[2 * FRAMES];
	int err;
	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
		return;
	}
	err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16,
				SND_PCM_ACCESS_RW_INTERLEAVED,
				2, 44100, 0, 200000); /* 200ms latency */
	if (err < 0) {
		fprintf(stderr, "snd_pcm_set_params: %s\n", snd_strerror(err));
		return;
	}

	struct aave_source *source = aave->sources;
	unsigned n;
	for (;;) {
		/*
		 * Put this number of frames of the anechoic sound buffer
		 * into the sound source buffer (loop if necessary).
		 */
		for (unsigned i = 0; i < FRAMES; i += n) {
			n = FRAMES - i;
			if (n > bufsize - bufhead)
				n = bufsize - bufhead;
			aave_put_audio(source, buf + bufhead, n);
			bufhead += n;
			if (bufhead == bufsize)
				bufhead = 0;
		}

		/* Get the frames of binaural data from the engine. */
		aave_get_audio(aave, buffer, FRAMES);

		/* Write the frames to the audio device (blocking write). */
		err = snd_pcm_writei(handle, buffer, FRAMES);
		if (err != FRAMES) {
			fprintf(stderr, "snd_pcm_writei: %s\n",
				 snd_strerror (err));
			break;
		}
	}
}
