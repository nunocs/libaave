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
#include "global.h"
#include <alsa/asoundlib.h>

/* Number of frames to process in each loop. */
#define FRAMES 8192 /* 186ms at 44100Hz */
#define NSOURCES 1

char* sources[] = {"../Sounds/piano.raw"};
unsigned nBufhead[NSOURCES];
unsigned nBufsize[NSOURCES];
short* nBuf[NSOURCES];

Audio::Audio(struct aave *aave)
    : QThread()
    , aave(aave)
{
    start(QThread::HighestPriority);
}

void Audio::run()
{
    /* Read the source.raw file (44100Hz 16 bits mono) into a buffer. */
    for (int i = 0; i < NSOURCES;i++) {

        QFile file(sources[i]);
        if (!file.open(QIODevice::ReadOnly)) {
            fprintf(stderr, "Error opening source.raw\n");
            return;
        }
        nBufhead[i] = 0;
        nBufsize[i] = file.size() / 2;
        nBuf[i] = new short[nBufsize[i]];
        memset(nBuf[i], 0, nBufsize[i] * sizeof(short));
        if (file.read((char *)nBuf[i], file.size()) != file.size()) {
            fprintf(stderr, "Error reading source.raw\n");
            return;
        }
        file.close();
    }

    /* Open the ALSA audio device. */
    const char device[] = "default";
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

    unsigned n;
    for (;;) {
        /*
         * Put this number of frames of the anechoic sound buffer
         * into the sound source buffer (loop if necessary).
         */

        struct aave_source *source = aave->sources;

        for (int j = 0; j < NSOURCES;j++) {

            for (unsigned i = 0; i < FRAMES; i += n) {
                n = FRAMES - i;
                if (n > nBufsize[j] - nBufhead[j])
                    n = nBufsize[j] - nBufhead[j];
                aave_put_audio(source, nBuf[j] + nBufhead[j], n);
                nBufhead[j] += n;
                if (nBufhead[j] == nBufsize[j])
                    nBufhead[j] = 0;
            }
            source=source->next;
        }

        /* Get the frames of binaural data from the engine. */
        aave_get_audio(aave, buffer, FRAMES);

        /* Write the frames to the audio device (blocking write). */
        err = snd_pcm_writei(handle, buffer, FRAMES);
        if (err != FRAMES) {
            fprintf(stderr, "snd_pcm_writei: %s\n",snd_strerror (err));
            break;
        }
    }
}
