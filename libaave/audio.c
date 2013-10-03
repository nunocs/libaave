/*
 * libaave/audio.c: audio processing
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <math.h> /* M_PI */
#include <string.h> /* memcpy() */
#include "aave.h"

/* Create a dft() function to convert 16-bit audio samples to frequency. */
#define DFT_TYPE short
#include "dft.h"

/* Create a idft() function to convert frequency to 32-bit audio samples. */
#define IDFT_TYPE int
#include "idft.h"

/*
 * b1 coefficient of the low-pass recursive filter for the distance value.
 * Design steps:
 * d = number of samples for the filter to decay to 36.8%
 * If the distance is updated at about 10Hz (0.1s), then we could make:
 * d = 0.1 * fs = 0.1 * 44100 = 4410 samples
 * Then obtain the b1 coefficient:
 * b1 = exp(-1/d) = 0.99977
 * Curiosity: the corresponding cut-off frequency would be:
 * fc = ln(b1) * fs / -2 / PI = ln(0.99977) * 44100 / -2 / PI = 1.6145 Hz
 * Reference: The Scientist and Engineer's Guide to DSP, chapter 19.
 */
#define AAVE_DISTANCE_B1 0.99977

/*
 * Return the gain (attenuation) for a sound at the specified distance.
 */
static float attenuation(float distance)
{
	return 1 / (distance + 1);
}

/*
 * Return the fade-in gain at index i for a window of the specified frames.
 */
static float fade_in_gain(unsigned i, unsigned frames)
{
	return (float)i / frames;
}

/*
 * Return the fade-out gain at index i for a window of the specified frames.
 */
static float fade_out_gain(unsigned i, unsigned frames)
{
	return (float)(frames - i) / frames;
}

/*
 * Calculate the Complex MULtiplication:
 * A = A * B
 * A = (ar + j ai) * (br + j br)
 * A = (ar * br - ai * bi) + j (ar * bi + ai * br)
 */
static void cmul(float *a, const float *b, unsigned n)
{
	float ar, ai, br, bi;
	unsigned i;

	a[0] = a[0] * b[0]; /* A[0] */
	a[1] = a[1] * b[1]; /* A[N/2] */

	for (i = 2; i < n; i += 2) {
		ar = a[i];
		ai = a[i+1];
		br = b[i];
		bi = b[i+1];
		a[i] = ar * br - ai * bi;
		a[i+1] = ar * bi + ai * br;
	}
}

/*
 * Calculate the Complex Multiplication and ADDition:
 * Y += g * A * B
 * Y += g * (ar + j ai) * (br + j br)
 * Y += g * (ar * br - ai * bi) + j g * (ar * bi + ai * br)
 */
static void cmadd(float *y, const float *a, const float *b, unsigned n, float g)
{
	float ar, ai, br, bi;
	unsigned i;

	y[0] += g * a[0] * b[0]; /* A[0] */
	y[1] += g * a[1] * b[1]; /* A[N/2] */

	for (i = 2; i < n; i += 2) {
		ar = a[i];
		ai = a[i+1];
		br = b[i];
		bi = b[i+1];
		y[i] += g * (ar * br - ai * bi);
		y[i+1] += g * (ar * bi + ai * br);
	}
}

/*
 * Process one sound and add it to the DFT busses.
 * aave: auralisation engine
 * sound: sound to process
 * ydft: DFT busses to add the sound to
 * delay: number of frames of pre-delay to apply to the sound
 * frames: number of frames to process
 */
static void aave_hrtf_add_sound(struct aave *aave, struct aave_sound *sound,
				float ydft[3][2][AAVE_MAX_HRTF * 4],
				unsigned delay, unsigned frames)
{
	unsigned c, d, i, j;
	float f, gain, distance, elevation, azimuth;
	const float *hrtf[2];
	short x[AAVE_MAX_HRTF * 2];

	/* Calculate the coordinates for the current positions. */
	aave_get_coordinates(aave, sound->position, &distance, &elevation,
								&azimuth);

	/* Get the best HRTF pair for these coordinates. */
	aave->hrtf_get(hrtf, elevation * (180/M_PI), azimuth * (180/M_PI));

	/* Use sensible defaults for the first time the sound is auralised. */
	if (!sound->hrtf[0]) {
		sound->distance = distance;
		sound->distance_smooth = distance;
		sound->hrtf[0] = hrtf[0];
		sound->hrtf[1] = hrtf[1];
	}

	/* DFT bus 1: previous block with current parameters. */
	if (sound->flags & SOUND_FADE_IN) {
		gain = attenuation(distance);
		for (c = 0; c < 2; c++)
			cmadd(ydft[1][c], sound->dft, hrtf[c], frames*2, gain);
	}

	/* Generate the current audio block. */
	d = sound->source->buffer_index - frames - delay;
	f = sound->distance_smooth;
	for (i = 0; i < frames; i++) {
		f = AAVE_DISTANCE_B1 * f + (1 - AAVE_DISTANCE_B1) * distance;
		j = d++ - (unsigned)(f * (AAVE_FS / AAVE_SOUND_SPEED));
		x[i] = sound->source->buffer[j & (AAVE_SOURCE_BUFSIZE - 1)];
	}
	sound->distance_smooth = f;

	/* Convert to the frequency domain, zero padded to 2 times. */
	dft(sound->dft, x, frames * 2);

	/* Apply the material absorption filter. */
	cmul(sound->dft, sound->filter, frames * 2);

	/* DFT bus 2: current block with current parameters. */
	if (sound->flags & SOUND_FADE_IN) {
		gain = attenuation(distance);
		for (c = 0; c < 2; c++)
			cmadd(ydft[2][c], sound->dft, hrtf[c], frames*2, gain);
	}

	/* DFT bus 0: current block with previous parameters. */
	if (sound->flags & SOUND_FADE_OUT) {
		gain = attenuation(sound->distance);
		for (c = 0; c < 2; c++)
			cmadd(ydft[0][c], sound->dft, sound->hrtf[c],
							frames * 2, gain);
	}

	/* Remember the parameters used for the current block. */
	sound->distance = distance;
	sound->hrtf[0] = hrtf[0];
	sound->hrtf[1] = hrtf[1];

	/* Update the bitmap state of this sound. */
	if (sound->flags == SOUND_FADE_IN)
		sound->flags = SOUND_FADE_IN | SOUND_FADE_OUT;
	else if (sound->flags == SOUND_FADE_OUT)
		sound->flags = 0;
}

/*
 * Generate one audio buffer of binaural data of the auralisation world.
 * aave: auralisation engine
 * delay: number of frames of pre-delay to apply to all sounds
 * frames: number of frames to process
 */
static void aave_hrtf_fill_output_buffer(struct aave *aave, unsigned delay,
							unsigned frames)
{
	unsigned i, c;
	struct aave_sound *s;
	float ydft[3][2][AAVE_MAX_HRTF * 4];
	int y[3][AAVE_MAX_HRTF * 4];
	int x, *overlap_add_buffer;

	for (i = 0; i < 3; i++)
		for (c = 0; c < 2; c++)
			memset(ydft[i][c], 0,
					2 * sizeof(ydft[0][0][0]) * frames);

	for (i = 0; i <= aave->reflections; i++)
		for (s = aave->sounds[i]; s; s = s->next)
			aave_hrtf_add_sound(aave, s, ydft, delay, frames);

	/* Generate the left and right channels. */
	for (c = 0; c < 2; c++) {
		overlap_add_buffer = aave->hrtf_overlap_add_buffer[c];

		/* Convert the 3 DFT busses to the time domain. */
		for (i = 0; i < 3; i++)
			idft(y[i], ydft[i][c], frames * 2);

		for (i = 0; i < frames; i++) {
			x = (overlap_add_buffer[i] + y[0][i])
						* fade_out_gain(i, frames)
				+ (y[1][i+frames] + y[2][i])
						* fade_in_gain(i, frames);
			/* Clip samples that overflow signed 16 bits. */
			if (x > 32767)
				x = 32767;
			else if (x < -32768)
				x = -32768;
			aave->hrtf_output_buffer[i * 2 + c] = x;
		}

		for (i = 0; i < frames; i++)
			overlap_add_buffer[i] = y[2][i+frames];
	}
}

/*
 * Generate N 16-bit 2-channel frames and
 * put them in the memory location pointed by BUF.
 */
void aave_get_audio(struct aave *aave, short *buf, unsigned n)
{
	unsigned frames, index;
	unsigned k;

	frames = 2 * aave->hrtf_frames;
	index = aave->hrtf_output_buffer_index;

	while (n) {
		k = frames - index;
		if (k == 0) {
			aave_hrtf_fill_output_buffer(aave, n, frames);
			index = 0;
			k = frames;
		}
		if (k > n)
			k = n;
		memcpy(buf, aave->hrtf_output_buffer + index * 2, k * 4);
		aave_reverb(aave, buf, k);
		n -= k;
		index += k;
		buf += k * 2;
	}

	aave->hrtf_output_buffer_index = index;
}

/*
 * Push the N frames pointed by AUDIO into the SOURCE's buffer.
 */
void aave_put_audio(struct aave_source *source, const short *audio, unsigned n)
{
	unsigned i = source->buffer_index;
	short *buf = source->buffer;

	while (n--) {
		i = (i + 1) & (AAVE_SOURCE_BUFSIZE - 1);
		buf[i] = *audio++;
	}

	source->buffer_index = i;
}
