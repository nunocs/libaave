/*
 * libaave/aave.h: header file of the AcousticAVE library
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file aave.h
 *
 * The aave.h file is the public header file of the AcousticAAVE library
 * (libaave). It contains the declarations of the structures and
 * functions used and implemented by the library.
 * Include it from your program source files to use them.
 *
 * The following diagram provides an overview of the data structures
 * in the library and their associations.
 *
 * @image html aave.png "Data structure diagram"
 * @image latex aave.eps "Data structure diagram" width=\textwidth
 *
 * The aave structure is the main data structure of the AcousticAVE library.
 * It contains all necessary information to completely define an
 * auralisation world and its present auralisation state.
 *
 * The aave structure contains a list of aave_surface structures.
 * These surfaces define the geometry of the auralisation world.
 * The material of each surface is indicated by pointing to the
 * corresponding element in the aave_materials table.
 *
 * The aave structure contains a list of aave_source structures,
 * one for each sound source present in the auralisation world.
 *
 * The sound sources and surfaces in the auralisation world generate
 * a number of sounds that reach the listener, and that are to be auralised
 * by the library. These sounds are stored in the aave structure in
 * a hash table indexed by the reflection order: direct sounds are put in
 * the list of aave_sound structures pointed by sounds[0], 1st reflection
 * sounds are put in sounds[1], 2nd reflections in sounds[2], etc...
 * The aave_update() function is responsible for maintaining the hash table
 * up to date, and the aave_get_audio() function is responsible for
 * auralising all sounds currently in it.
 *
 * Each aave_sound structure contains a reference to its sound source
 * and references to each surface where that sound reflects.
 */

/**
 * @mainpage Introduction
 *
 * The AcousticAVE library (libaave) is an auralisation library.
 * It is the equivalent of a 3D graphics visualisation library,
 * but for audio: given a model of a room, the positions of the sound
 * sources, and the position and head orientation of the listener,
 * libaave produces the 3D soundfield that would be heard by that
 * listener in that virtual environment.
 *
 * libaave supports moving sound sources and listener, therefore it can
 * be used for auralisation of interactive virtual reality environments,
 * usually in combination with a 3D graphics visualisation library.
 *
 * libaave performs in real-time for virtual rooms of some complexity
 * (number of surfaces) and some order of sound reflections
 * (configured by the user), more specifically the total number of sounds,
 * that depend on the processor used. Benchmarks:
 * - Intel Atom N2600 1.6GHz: 66 sounds
 * - Intel Xeon 2GHz: 89 sounds
 * - AMD Opteron 248 2.2GHz: 193 sounds
 *
 * @section installation Installation
 *
 * libaave is implemented in ANSI C and does not depend on any external
 * library, so it is fairly portable (it is known to work on GNU/Linux
 * and Microsoft Windows systems, but should also work on any other
 * platform with an ANSI C compiler).
 *
 * 1. Download the source code:
 * @code
 * git clone https://code.ua.pt/git/acousticave
 * @endcode
 *
 * 2. Build libaave:
 * @code
 * cd acousticave/libaave
 * make
 * @endcode
 *
 * For a quick start on using libaave in your programs, see the examples
 * in acousticave/libaave/examples/.
 *
 * @section coordinates Coordinates
 *
 * For all 3D points (positions and room model vertices), libaave uses
 * the coordinate system commonly used by CAD architecture programs:
 * x=North, y=West, z=up, in metres.
 *
 * For the listener head orientation angles, libaave uses the coordinate
 * system commonly used by inertial sensors: x=North, y=East, z=down,
 * in radians. This means:
 * - roll > 0 is tilt right, roll < 0 is tilt left
 * - pitch > 0 is tilt up, pitch < 0 is tilt down
 * - yaw > 0 is rotate right, yaw < 0 is rotate left, yaw = 0 is North
 *
 * @section overview Overview
 *
 * The file aave.h is the public header file of libaave.
 * Include it from your program source files to use its structures
 * and functions.
 *
 * The file geometry.c implements all functions related to geometry
 * processing: construction of the 3D room model, coordinate conversions,
 * determining the image-source positions, the sound reflection paths,
 * the audible and non-audible sound paths, and passing all this
 * information to the audio processing functions.
 *
 * The file obj.c contains a convenience function that reads a 3D model
 * from a Wavefront .OBJ file and calls the appropriate functions in
 * geometry.c to construct the room to be auralised in just one step.
 *
 * The file audio.c implements the core of the audio processing:
 * receiving anechoic audio data from the sound sources,
 * head-related transfer function (HRTF) processing, and generating
 * the corresponding auralised audio data in binaural format.
 *
 * The files hrtf_cipic.c, hrtf_listen.c, hrtf_mit.c, and hrtf_tub.c
 * implement the interface functions for using the
 * CIPIC, LISTEN, MIT, and TU-Berlin HRTF sets, respectively.
 *
 * The files dft.h and idft.h implement the Discrete Fourier Transform
 * and Inverse Discrete Fourier Transform algorithms,
 * respectively, used mainly for the HRTF processing.
 *
 * The file material.c implements the functions related to the
 * sound absorption caused by the different surface materials:
 * the table of material reflection coefficients by frequency band,
 * the table lookup, and the design of the audio filters.
 *
 * The file reverb.c implements an artificial reverberation algorithm
 * that adds a tail of late reflections to the auralisation output.
 *
 * The directory tools contains the programs used to automatically generate
 * the hrtf_*_set*.c source files from the respective HRTF data sets,
 * the dftsincos.c source file with the sin() and cos() lookup table
 * for the DFT and IDFT algorithms, and miscellaneous utility programs
 * to handle or generate audio files.
 *
 * The directory tests/ contains programs to verify the correctness
 * of the functions implemented in libaave.
 *
 * The directory examples/ contains programs to show how libaave
 * can be used for different auralisation applications.
 *
 * The directory doc/ contains the files to generate this document
 * from the documentation written in the source files.
 *
 * @section acknowledgements Acknowledgements
 *
 * The development of libaave was funded by the Portuguese Government
 * through FCT (Fundação para a Ciência e a Tecnologia) as part of the
 * project AcousticAVE: Auralisation Models and Applications for
 * Virtual Reality Environments (PTDC/EEA-ELC/112137/2009).
 *
 * @example examples/circle.c
 * An example of auralisation of a moving sound source
 * describing a circle around the listener.
 *
 * @example examples/elevation.c
 * An example of auralisation of multiple sound source at different heights
 * with the listener passing by.
 *
 * @example examples/line.c
 * An example of auralisation of a sound source moving on a straight line
 * passing by the listener.
 *
 * @example examples/stream.c
 * An example of auralisation of a streaming input sound, in real-time,
 * with a user-specified room model and reflection order, using the
 * Advanced Linux Sound Architecture.
 */

/**
 * The audio sampling frequency, in Hz, used throughout the library.
 * In particular, libaave expects the anechoic input data of the
 * sound sources to be delivered in this sampling frequency.
 * The auralisation binaural output data is also in this sampling frequency.
 *
 * @todo To support different audio sampling frequencies without having to
 * recompile the library, this value would be set in a member of the aave
 * structure at runtime instead. Of course, that would incur in performance
 * penalties, most importantly in the audio processing (one floating-point
 * division per audio sample per auralised sound). Furthermore, the HRTF
 * data sets would have to be resampled to the desired sampling frequency.
 */
#define AAVE_FS 44100

/**
 * The maximum order of reflections supported.
 * This defines the maximum value the user can select for the
 * order of reflections to calculate in the auralisation process.
 *
 * @todo To support different maximum orders of reflections per instance,
 * this value would be set in a member of the aave structure at runtime
 * and the sounds hash table allocated accordingly. However, I think this
 * is not worth the trouble. Just change this value and recompile, if you
 * want more orders of reflections (and your computer can handle them).
 * The waste is only 4 or 8 bytes per reflection order that is not used,
 * for 32-bit or 64-bit processors respectively.
 */
#define AAVE_MAX_REFLECTIONS 16

/**
 * The maximum number of frames of an HRTF.
 * (The longest HRTFs are TU-Berlin's: 2048).
 *
 * @todo When using HRTFs with less frames (MIT only has 128)
 * there is a considerable waste of memory throughout the library.
 * However, this way the code is much simpler, and slightly faster.
 * Nevertheless, it would be nice if this value could be changed at
 * runtime when the user selects the HRTF set to use.
 */
#define AAVE_MAX_HRTF 2048

/**
 * The number of past anechoic samples to hold for each sound source.
 * This effectively defines the maximum distance that can be auralised:
 *
 * distance [m] = AAVE_SOURCE_BUFSIZE * AAVE_SOUND_SPEED [m/s] / AAVE_FS [Hz]
 *
 * Must be a power of 2 to allow for the most efficient implementation.
 * Some possible values (and corresponding maximum distances for fs=44100Hz):
 *
 * 32768 (255m), 65536 (510m), 131072 (1020m), 262144 (2040m), 524288 (4080m)
 */
#define AAVE_SOURCE_BUFSIZE 131072

/**
 * Reverb pre-delay buffer size, in samples (must be a power of 2).
 * This defines the maximum reverberation pre-delay time (echo):
 *
 * AAVE_REVERB_BUFSIZE1 / AAVE_FS = 32768 / 44100 ~= 743 ms
 */
#define AAVE_REVERB_BUFSIZE1 32768

/**
 * Reverb feedback delay buffer size, in samples (must be a power of 2).
 * This defines the maximum reverberation feedback delay time (density):
 *
 * AAVE_REVERB_BUFSIZE2 / AAVE_FS = 1024 / 44100 ~= 23 ms
 */
#define AAVE_REVERB_BUFSIZE2 1024

/**
 * The number of reflection factors that specify each material.
 * The corresponding frequencies are:
 * 125, 250, 500, 1000, 2000, 4000, 8000 Hz.
 */
#define AAVE_MATERIAL_REFLECTION_FACTORS 7

/**
 * Speed of sound in dry air at 20 degrees Celsius (m/s).
 *
 * Reference: https://en.wikipedia.org/wiki/Speed_of_sound
 */
#define AAVE_SOUND_SPEED 343.2

/**
 * The AcousticAVE main data structure. It contains all the information
 * that defines one acoustic world and its present auralisation state.
 */
struct aave {

	/** Position of the listener in the auralization world [x,y,z] (m). */
	float position[3];

	/** Listener head orientation (inertial-to-body rotation matrix). */
	float orientation[3][3];

	/** Singly-linked list of surfaces in the auralisation world. */
	struct aave_surface *surfaces;

	/** Number of surfaces in the list of surfaces. */
	unsigned nsurfaces;

	/** Singly-linked list of sound sources in the auralisation world. */
	struct aave_source *sources;

	/** Hash table of sounds to auralise, indexed by reflection order. */
	struct aave_sound *sounds[AAVE_MAX_REFLECTIONS];

	/** Maximum number of reflections to calculate for each source. */
	unsigned reflections;

	/** Gain to apply to the output sound. */
	float gain;

	/** The number of frames of the HRTFs currently in use (power of 2). */
	unsigned hrtf_frames;

	/** Function to get the HRTF pair for some elevation and azimuth. */
	void (*hrtf_get)(const float *hrtf[2], int elevation, int azimuth);

	/** Index of the next frame of the HRTF output buffer to be consumed. */
	unsigned hrtf_output_buffer_index;

	/** HRTF audio block output buffer (2 16-bit channels interleaved). */
	short hrtf_output_buffer[AAVE_MAX_HRTF * 4];

	/** HRTF overlap-add buffer (2 32-bit channels). */
	int hrtf_overlap_add_buffer[2][AAVE_MAX_HRTF * 2];

	/** Reverb pre-delay buffer index. */
	unsigned reverb_buffer1_index[2];

	/** Reverb delay buffer index. */
	unsigned reverb_buffer2_index[2];

	/** Reverb pre-delay buffer. */
	float reverb_buffer1[AAVE_REVERB_BUFSIZE1][2];

	/** Reverb delay buffer. */
	float reverb_buffer2[AAVE_REVERB_BUFSIZE2][2];
};

/**
 * Data for each sound source in the auralisation world.
 */
struct aave_source {

	/** Pointer to the next source (singly-linked list). */
	struct aave_source *next;

	/** The AcousticAVE engine this sound source is associated with. */
	struct aave *aave;

	/** Position of the source in the auralisation world [x,y,z] (m). */
	float position[3];

	/** Index of the most recently inserted sample. */
	unsigned buffer_index;

	/** Ring buffer to store the recent past anechoid samples. */
	short buffer[AAVE_SOURCE_BUFSIZE];
};

/**
 * Data for each surface that makes the auralisation world.
 * A surface is defined as an n-point planar polygon.
 * The points are specified in anti-clockwise order.
 */
struct aave_surface {

	/** Pointer to the next surface (singly-linked list). */
	struct aave_surface *next;

	/** Material of the surface. */
	const struct aave_material *material;

	/**
	 * Specification of the plane where this surface lays, in
	 * Hessian normal form: unit normal vector and distance from origin.
	 * (http://mathworld.wolfram.com/HessianNormalForm.html)
	 * The distance is used to calculate the position of the image
	 * sources. The unit normal vector is used just about everywhere.
	 */
	float normal[3];
	float distance;

	/**
	 * Alternate specification of the plane where this surface lays
	 * using 2 versors and a point (points[0]).
	 * This is used to perform calculations in local coordinates,
	 * namely the non-convex polygon - line intersection algorithm.
	 */
	float versors[2][3];

	/** Number of points of the polygon (minimum 3). */
	unsigned npoints;

	/**
	 * Coordinates of each point, in counter-clockwise order
	 * (counter-clockwise normal).
	 *
	 * World coordinates:
	 * - x = points[i][0]
	 * - y = points[i][1]
	 * - z = points[i][2]
	 *
	 * Local coordinates (internally used by the polygon-line
	 * intersection algorithm):
	 * - ex = points[i][3]
	 * - ex = points[i][4]
	 *
	 * @todo Remove hardcoded maximum number of points per surface.
	 */
	float points[32][3 + 2];
};

/**
 * Data for each sound to auralise.
 */
struct aave_sound {

	/** Pointer to the next sound (singly-linked list). */
	struct aave_sound *next;

	/** The sound source that is producing this sound. */
	struct aave_source *source;

	/**
	 * Position of the source or image-source of this sound [x,y,z] (m).
	 * If this is a direct sound, this points to source->position.
	 * If it is a reflection, this points to image_sources[order].
	 */
	float *position;

	/** Flag that indicates if the sound is audible (1) or not (0). */
	int audible;

	/**
	 * The previous fade-in/out sample count value used
	 * (for the fade-in/out of appearing/disappearing sounds).
	 */
	unsigned fade_samples;

	/** The previous distance value used (for the crossfading). */
	float distance;

	/** Smooth (low-pass filtered) distance value (for the resampling). */
	float distance_smooth;

	/** The previous HRTF pair used (for the crossfading). */
	const float *hrtf[2];

	/** The surfaces where this sound reflects. */
	struct aave_surface *surfaces[AAVE_MAX_REFLECTIONS];

	/** The image-source positions calculated for each reflection. */
	float image_sources[AAVE_MAX_REFLECTIONS][3];

	/** The points [x,y,z] where this sound reflects. */
	float reflection_points[AAVE_MAX_REFLECTIONS][3];

	/** The DFT of the previous audio block. */
	float dft[AAVE_MAX_HRTF * 4];

	/** The material absorption filter DFT. */
	float filter[AAVE_MAX_HRTF * 4];
};

/**
 * Acoustic properties of a material.
 */
struct aave_material {

	/** Name used in the usemtl directives of the .obj files. */
	const char *name;

	/** Reflection factors (multiplied by 100), by frequency band. */
	const unsigned char reflection_factors[AAVE_MATERIAL_REFLECTION_FACTORS];
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
