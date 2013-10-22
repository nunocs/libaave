/*
 * libaave/audio.c: audio processing
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file audio.c
 *
 * The audio.c file contains the functions that implement the core of the
 * auralisation audio processing of the AcousticAVE library (libaave).
 * The following image illustrates the audio processing model of the library,
 * as seen by the user.
 *
 * @image html audio1.png "Audio processing model"
 * @image latex audio1.eps "Audio processing model" width=\textwidth
 *
 * The user passes the anechoic audio data of each sound source in the
 * auralisation world to the library using aave_put_audio() and then
 * retrieves the calculated auralisation binaural audio data using
 * aave_get_audio().
 *
 * Although the library supports processing one audio frame at a time,
 * users would typically work with blocks of audio frames at a time,
 * for efficiency reasons of the underlying operating system.
 * Each time, the number of anechoic audio frames (samples) in the blocks
 * passed to each source using aave_put_audio() must be the same for
 * all sources, and the same number of binaural audio frames (samples
 * times 2) must be retrieved using aave_get_audio().
 *
 * The current implementation of the library runs most efficiently when
 * the number of audio frames per block is a multiple of the number of
 * samples of the selected head-related transfer functions (HRTF), times 2,
 * as that is the size of the discrete Fourier transforms (DFT) performed
 * internally.
 *
 * The HRTF set to use for the auralisation process can be selected by
 * calling one of aave_hrtf_cipic(), aave_hrtf_listen(), aave_hrtf_mit(),
 * or aave_hrtf_tub(), for the CIPIC, LISTEN, MIT, or TU-Berlin HRTF sets,
 * respectively, and it must be done before ever calling aave_put_audio().
 *
 * The following describes the internal implementation details of the
 * audio processing performed by the library to produce the auralisation.
 * Mere users of the library may want to skip the rest of this section.
 *
 * @image html audio2.png "Overall audio processing block diagram"
 * @image latex audio2.eps "Overall audio processing block diagram" width=\textwidth
 *
 * The above image illustrates the entire auralisation audio processing,
 * performed by aave_get_audio(), from the anechoic audio data of all
 * sound sources present, to the resulting auralisation left and right
 * (binaural) audio data.
 *
 * Each sound source originates a number of sounds that reach the listener
 * (direct sound and reflection sounds, as many as the geometry.c part of
 * the auralisation process is able to find in a given amount of time).
 * Each sound is processed differently according to the travelled distance,
 * direction of arrival, and materials of the reflection surfaces, mainly.
 * This audio processing is represented by the sound-i boxes in the image
 * above and is further expanded in the image below.
 *
 * @image html audio3.png "Individual sound processing block diagram"
 * @image latex audio3.eps "Individual sound processing block diagram" width=\textwidth
 *
 * The anechoic sound source buffer is a ring buffer that contains the
 * anechoic audio data of the sound source, supplied by aave_put_audio().
 * Each sound originated from this sound source gets its input audio data
 * from this ring buffer, from the delayed audio sample position
 * corresponding to the travelled distance of that particular sound.
 *
 * When the listener or sound sources move, the travelled distances change,
 * in a discontinuous manner, since the positions' update rate is usually
 * much lower than the audio sampling rate. The resampler block is
 * responsible for producing a stream of audio data without discontinuities,
 * from the discontinuous distance values. It does this by upsampling and
 * low-pass filtering the distance values, and then interpolating
 * (zero-order) the audio samples in the ring buffer with the
 * corresponding fractional delay, as described in:
 * Peter Brinkman and Michael Gogins, "Doppler effects without equations",
 * Proc. of the 16th Int. Conf. on Digital Audio Effects (DAFx-13).
 *
 * The HRTF processing is performed in the frequency domain, using the
 * fast convolution method mentioned in: Udo Zolzer, "Digital Audio Signal
 * Processing", 2nd Edition, Section 5.3 Nonrecursive Audio Filters.
 * The DFT block thus zero-pads and converts the audio data
 * from time domain to frequency domain, in dft.h.
 * Note that, without the resampler block, three DFT blocks would be
 * needed instead of just one.
 *
 * The material absorption filter block implements the sound attenuation by
 * frequency band of the materials of the surfaces where the sound reflects.
 * This filtering is efficiently performed in the frequency domain
 * simply by calculating N complex multiplications, in cmul().
 * The design of the filter is implemented in material.c.
 *
 * Next, the anechoic sound is turned into binaural by applying the
 * HRTF pair that corresponds to the direction (elevation and azimuth)
 * of arrival of the sound relative to the listener's head.
 * When the listener moves her head, the HRTF pair changes.
 * This causes discontinuities in the output binaural sound, mainly due
 * to the phase differences between the previous and current HRTF pair.
 * The greater the movement, the more audible these discontinuities are.
 * To mask these discontinuities, it is implemented a method that applies
 * both HRTF pairs, corresponding to the previous and current directions,
 * and then fades-out the previous and fades-in the current.
 * It is a multiple-sound and binaural version of the algorithm described in:
 * Tom Barker et al, "Real-time auralisation system for virtual microphone
 * positioning", Proc. of the 15th Int. Conf. on Digital Audio Effects
 * (DAFx12).
 *
 * The amplitude attenuation of the sounds with distance is also performed
 * in the frequency domain, taking advantage that it can be applied at the
 * same time the HRTF filter is, simply by multiplying the magnitude of
 * the frequency response by the appropriate gain value, in cmadd().
 *
 * The result of each sound processing block is therefore 3 binaural audio
 * signals (6 total), still in the frequency domain: DFT bus 0 with the
 * current anechoic audio block processed with the previous distance and
 * direction parametres, DFT bus 1 with the previous block processed with
 * the current parametres, and DFT bus 2 with the current block processed
 * with the current parametres.
 *
 * The binaural audio signals now need to be converted back to the time
 * domain. But instead of performing 6 inverse discrete Fourier transforms
 * (IDFT) per sound and then summing them in the time domain, the different
 * sounds are summed still in the frequency domain, into the DFT busses
 * pictured. This way, only 6 IDFT are performed in total, independently
 * of the number of sounds.
 *
 * To complete the fast convolution method, the sounds in the 6 busses,
 * now in the time domain, are overlap-added, as described in:
 * Udo Zolzer, "Digital Audio Signal Processing", 2nd Edition,
 * Section 5.3.2 Fast Convolution of Long Sequences.
 * @todo Here, it might be more efficient to use the overlap-save method
 * instead of the overlap-add method.
 *
 * Linear fade-outs and fade-ins are then applied to the previous and
 * current parametre busses, respectively, to complete the crossfade
 * method described in Tom Barker et al (see above).
 *
 * At this point, the left and right signals contain the binaural
 * auralisation of the direct sounds and reflection sounds up to
 * a given reflection order. An artificial reverberation tail,
 * implemented in reverb.c, is finally added to simulate the late
 * reflections that the geometry.c part could not calculate.
 */

#include <math.h> /* M_PI */
#include <string.h> /* memcpy() */
#include "aave.h"

/** Create a dft() function to convert 16-bit audio samples to frequency. */
#define DFT_TYPE short
#include "dft.h"

/** Create an idft() function to convert frequency to 32-bit audio samples. */
#define IDFT_TYPE int
#include "idft.h"

/**
 * The b1 coefficient of the single-pole, low-pass, recursive filter
 * implemented in the resampler block to smooth the distance values:
 *
 * y[n] = b1 * y[n-1] + (1 - b1) * x[n]
 *
 * Design steps:
 *
 * d = number of samples for the filter to decay to 36.8%
 *
 * If the distance is updated at about 10Hz (0.1s), then we could make:
 *
 * d = 0.1 * fs = 0.1 * 44100 = 4410 samples
 *
 * Then obtain the b1 coefficient:
 *
 * b1 = exp(-1/d) = exp(-1/4410) ~= 0.99977
 *
 * Reference: The Scientist and Engineer's Guide to DSP, chapter 19.
 */
#define AAVE_DISTANCE_B1 0.99977

/**
 * Return the gain corresponding to the amplitude attenuation
 * of a sound at the specified @p distance (m).
 */
static float attenuation(float distance)
{
	return 1 / (distance + 1);
}

/**
 * Return the fade-in gain at index @p i for a window of the specified
 * @p frames.
 */
static float fade_in_gain(unsigned i, unsigned frames)
{
	return (float)i / frames;
}

/**
 * Return the fade-out gain at index @p i for a window of the specified
 * @p frames.
 */
static float fade_out_gain(unsigned i, unsigned frames)
{
	return (float)(frames - i) / frames;
}

/**
 * Calculate the Complex MULtiplication @p a = @p a * @p b of size @p n.
 *
 * A = A * B
 *
 * A = (ar + j ai) * (br + j br)
 *
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

/**
 * Calculate the Complex Multiplication and ADDition
 * @p y += @p g * @p a * @p b of size @p n.
 *
 * Y += g * A * B
 *
 * Y += g * (ar + j ai) * (br + j br)
 *
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

/**
 * Generate one audio source block.
 * @p sound is the sound whose source to get the anechoic audio data from,
 * @p distance is the distance from the (image) source to the listener,
 * @p x is the buffer to store the generated audio data,
 * @p frames is the number of frames (anechoic samples) to generate, and
 * @p delay is the number of frames of pre-delay to apply to the sound
 * to account for audio user blocks larger than the size of the HRTFs.
 *
 * The distance value changes abruptly between audio blocks every time
 * the listener or the sound source move. To generate audio blocks without
 * discontinuities, the anechoic samples of the sound source are resampled,
 * using first-order interpolation (linear interpolation), a good compromise
 * between audio quality and processing time, to match an upsampled and low-
 * pass filtered version of the discontinuous distance value, as described in
 * Peter Brinkman and Michael Gogins, "Doppler effects without equations",
 * Proc. of the 16th Int. Conf. on Digital Audio Effects (DAFx-13).
 */
static void aave_audio_source_block(struct aave_sound *sound, float distance,
				short *x, unsigned frames, unsigned delay)
{
	unsigned d, i, j;
	short x1, x2;
	float f, a;

	d = sound->source->buffer_index - frames - delay;
	f = sound->distance_smooth;
	for (i = 0; i < frames; i++) {
		f = AAVE_DISTANCE_B1 * f + (1 - AAVE_DISTANCE_B1) * distance;
		a = f * (AAVE_FS / AAVE_SOUND_SPEED);
		j = d++ - (unsigned)a;
		a = a - (unsigned)a;
		x1 = sound->source->buffer[j & (AAVE_SOURCE_BUFSIZE - 1)];
		x2 = sound->source->buffer[(j-1) & (AAVE_SOURCE_BUFSIZE - 1)];
		x[i] = x1 * (1 - a) + x2 * a;
	}
	sound->distance_smooth = f;
}

/**
 * Process one @p sound and add it to the DFT busses @p ydft.
 * @p frames is the number of frames to process.
 * @p delay is the number of frames of pre-delay to apply to the sound
 * to account for audio user blocks larger than the size of the HRTFs.
 */
static void aave_hrtf_add_sound(struct aave *aave, struct aave_sound *sound,
				float ydft[3][2][AAVE_MAX_HRTF * 4],
				unsigned delay, unsigned frames)
{
	unsigned c;
	float gain, distance, elevation, azimuth;
	const float *hrtf[2];
	short x[AAVE_MAX_HRTF * 2];

	if (sound->flags == SOUND_OFF)
		return;

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

	/* Generate the current audio block (resampler). */
	aave_audio_source_block(sound, distance, x, frames, delay);

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

/**
 * Generate one audio buffer of binaural data for the auralisation world
 * @p aave with all sounds in it.
 * @p frames is the number of frames to generate.
 * @p delay is the number of frames of pre-delay to apply to all sounds
 * to account for audio user blocks larger than the size of the HRTFs.
 */
static void aave_hrtf_fill_output_buffer(struct aave *aave, unsigned delay,
							unsigned frames)
{
	unsigned i, c;
	struct aave_sound *s;
	float ydft[3][2][AAVE_MAX_HRTF * 4];
	int y[3][AAVE_MAX_HRTF * 4];
	int x, *overlap_add_buffer;

	/* Reset the DFT busses. */
	for (i = 0; i < 3; i++)
		for (c = 0; c < 2; c++)
			memset(ydft[i][c], 0,
					2 * sizeof(ydft[0][0][0]) * frames);

	/* Add all audible sounds to the DFT busses. */
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
			x = aave->gain *
				((overlap_add_buffer[i] + y[0][i])
						* fade_out_gain(i, frames)
				+ (y[1][i+frames] + y[2][i])
						* fade_in_gain(i, frames));
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

/**
 * Generate @p n 16-bit 2-channel frames of the auralisation world @p aave
 * and put them in the memory location pointed by @p buf.
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

/**
 * Put the @p n frames pointed by @p audio in the ring buffer of @p source.
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
