/*
 * libaave/aave.h: header file of the AcousticAVE library
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * Auralisation world coordinates: x = North, y = West, z = up.
 */

/*
 * Audio sampling frequency (Hz).
 */
#define AAVE_FS 44100

/*
 * Maximum number of reflections to calculate.
 */
#define AAVE_MAX_REFLECTIONS 16

/*
 * The maximum number of frames of an HRTF (the longest is TU-Berlin's: 2048).
 */
#define AAVE_MAX_HRTF 2048

/*
 * The number of past anechoic samples to hold for each sound source.
 * This effectively defines the maximum distance that can be auralised:
 * distance [m] = AAVE_SOURCE_BUFSIZE * AAVE_SOUND_SPEED [m/s] / AAVE_FS [Hz]
 * Must be a power of 2 to allow for the most efficient implementation.
 * Some possible values (and corresponding maximum distances for fs=44100Hz):
 * 32768 (255m), 65536 (510m), 131072 (1020m), 262144 (2040m), 524288 (4080m)
 */
#define AAVE_SOURCE_BUFSIZE 131072

/*
 * Reverb pre-delay buffer size, in samples (must be a power of 2).
 * This defines the maximum reverberation pre-delay time (echo):
 * AAVE_REVERB_BUFSIZE1 / AAVE_FS = 32768 / 44100 ~= 743 ms
 */
#define AAVE_REVERB_BUFSIZE1 32768

/*
 * Reverb feedback delay buffer size, in samples (must be a power of 2).
 * This defines the maximum reverberation feedback delay time (density):
 * AAVE_REVERB_BUFSIZE2 / AAVE_FS = 1024 / 44100 ~= 23 ms
 */
#define AAVE_REVERB_BUFSIZE2 1024

/*
 * The number of absorption coefficients of the materials.
 * Octave band frequencies: 125, 250, 500, 1000, 2000, 4000, 8000 Hz.
 */
#define AAVE_MATERIAL_COEFFICIENTS 7

/*
 * Speed of sound in dry air at 20 degrees Celsius (m/s).
 * https://en.wikipedia.org/wiki/Speed_of_sound
 */
#define AAVE_SOUND_SPEED 343.2

/*
 * AcousticAVE engine data.
 */
struct aave {

	/* Position of the listener in the auralization world [x,y,z] (m). */
	float position[3];

	/* Listener head orientation (inertial-to-body rotation matrix). */
	float orientation[3][3];

	/* Singly-linked list of surfaces in the auralisation world. */
	struct aave_surface *surfaces;

	/* Number of surfaces in the list of surfaces. */
	unsigned nsurfaces;

	/* Singly-linked list of sound sources in the auralisation world. */
	struct aave_source *sources;

	/* Hash table of sounds to auralise, indexed by reflection order. */
	struct aave_sound *sounds[AAVE_MAX_REFLECTIONS];

	/* Maximum number of reflections to calculate for each source. */
	unsigned reflections;

	/* Data for the HRTF processing: */

	/* The number of frames of the HRTFs currently in use (power of 2). */
	unsigned hrtf_frames;

	/* Function to get the HRTF pair for some elevation and azimuth. */
	void (*hrtf_get)(const float *hrtf[2], int elevation, int azimuth);

	/* Index of the next frame of the HRTF output buffer to be consumed. */
	unsigned hrtf_output_buffer_index;

	/* HRTF audio block output buffer (2 16-bit channels interleaved). */
	short hrtf_output_buffer[AAVE_MAX_HRTF * 4];

	/* HRTF overlap-add buffer (2 32-bit channels). */
	int hrtf_overlap_add_buffer[2][AAVE_MAX_HRTF * 2];

	/* Reverb. */
	unsigned reverb_buffer1_index[2];
	unsigned reverb_buffer2_index[2];
	float reverb_buffer1[AAVE_REVERB_BUFSIZE1][2];
	float reverb_buffer2[AAVE_REVERB_BUFSIZE2][2];
};

/*
 * Data for each sound source in the auralisation world.
 */
struct aave_source {

	/* Pointer to the next source (singly-linked list). */
	struct aave_source *next;

	/* The AcousticAVE engine this sound source is associated with. */
	struct aave *aave;

	/* Position of the source in the auralisation world [x,y,z] (m). */
	float position[3];

	/* Index of the most recently inserted sample. */
	unsigned buffer_index;

	/* Ring buffer to store the recent past anechoid samples. */
	short buffer[AAVE_SOURCE_BUFSIZE];
};

/*
 * Data for each surface that makes the auralisation world.
 * For now, a surface is defined as an n-point planar polygon.
 * Points are specified in anti-clockwise order.
 */
struct aave_surface {

	/* Pointer to the next surface (singly-linked list). */
	struct aave_surface *next;

	/* Material of the surface. */
	const struct aave_material *material;

	/*
	 * Specification of the plane where this surface lays in
	 * Hessian normal form: unit normal vector and distance from origin.
	 * (http://mathworld.wolfram.com/HessianNormalForm.html)
	 * The distance is used to calculate the position of the image sources.
	 * The unit normal vector is used just about everywhere.
	 */
	float normal[3];
	float distance;

	/*
	 * Alternate specification of the plane where this surface lays
	 * using 2 versors and a point (points[0]).
	 * This is used to perform calculations in local coordinates,
	 * namely the non-convex polygon - line intersection algorithm.
	 */
	float versors[2][3];

	/* Number of points of the polygon (minimum 3). */
	unsigned npoints;

	/*
	 * Coordinates of each point, in counter-clockwise order
	 * (counter-clockwise normal).
	 *
	 * World coordinates:
	 * x = points[i][0]
	 * y = points[i][1]
	 * z = points[i][2]
	 *
	 * Local coordinates:
	 * ex = points[i][3]
	 * ex = points[i][4]
	 * (internally used by the polygon-line intersection algorithm)
	 *
	 * FIXME: hardcoded limit of maximum number of points.
	 */
	float points[32][3 + 2];
};

/*
 * Data for each sound to auralise.
 */
struct aave_sound {

	/* Pointer to the next sound (singly-linked list). */
	struct aave_sound *next;

	/* The sound source that is producing this sound. */
	struct aave_source *source;

	/*
	 * Position of the source or image-source of this sound [x,y,z] (m).
	 * If this is a direct sound, this points to source->position.
	 * If it is a reflection, this points to image_sources[order].
	 */
	float *position;

	/* Bitmap of the state of the sound. */
	int flags;
	#define SOUND_OFF 0
	#define SOUND_FADE_IN 1
	#define SOUND_FADE_OUT 2

	/* The previous distance value used (for the crossfading). */
	float distance;

	/* Smooth (low-pass filtered) distance value (for the resampling). */
	float distance_smooth;

	/* The previous HRTF pair used (for the crossfading). */
	const float *hrtf[2];

	/* The surfaces where this sound reflects. */
	struct aave_surface *surfaces[AAVE_MAX_REFLECTIONS];

	/* The image-source positions calculated for each reflection. */
	float image_sources[AAVE_MAX_REFLECTIONS][3];

	/* The points [x,y,z] where this sound reflects. */
	float reflection_points[AAVE_MAX_REFLECTIONS][3];

	/* The DFT of the previous audio block. */
	float dft[AAVE_MAX_HRTF * 4];

	/* The material absorption filter DFT. */
	float filter[AAVE_MAX_HRTF * 4];
};

/*
 * Audio properties of the surface materials.
 */
struct aave_material {

	/* Name used in the usemtl directives of the .obj files. */
	const char *name;

	/* Reflection coefficients (multiplied by 100) by frequency band. */
	const unsigned char coefficients[AAVE_MATERIAL_COEFFICIENTS];
};

/* audio.c */
extern void aave_get_audio(struct aave *, short *, unsigned);
extern void aave_put_audio(struct aave_source *, const short *, unsigned);

/* dftindex.c */
extern unsigned dft_index(unsigned, unsigned);

/* geometry.c */
extern void aave_add_source(struct aave *, struct aave_source *);
extern void aave_add_surface(struct aave *, struct aave_surface *);
extern void aave_get_coordinates(const struct aave *, const float *, float *, float *, float *);
extern void aave_set_listener_orientation(struct aave *, float, float, float);
extern void aave_set_listener_position(struct aave *, float, float, float);
extern void aave_set_source_position(struct aave_source *, float, float, float);
extern void aave_update(struct aave *);

/* hrtf_cipic.c */
extern void aave_hrtf_cipic(struct aave *);

/* hrtf_listen.c */
extern void aave_hrtf_listen(struct aave *);

/* hrtf_mit.c */
extern void aave_hrtf_mit(struct aave *);

/* hrtf_tub.c */
extern void aave_hrtf_tub(struct aave *);

/* init.c */
extern void aave_init(struct aave *);
extern void aave_init_source(struct aave *, struct aave_source *);

/* material.c */
extern const struct aave_material aave_material_none;
extern const struct aave_material *aave_get_material(const char *);
extern void aave_get_material_filter(struct aave *, struct aave_surface **, unsigned, float *);

/* obj.c */
extern void aave_read_obj(struct aave *, const char *);

/* reverb.c */
extern void aave_reverb(struct aave *, short *, unsigned);
