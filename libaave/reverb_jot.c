/*   This file is part of LibAAVE.
 * 
 *   LibAAVE is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibAAVE is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibAAVE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Copyright 2014 André Oliveira, Nuno Silva, Guilherme Campos,
 *   Paulo Dias, José Vieira/IEETA - Universidade de Aveiro
 *
 *
 *   libaave/reverb_jot.c: Jot feedback delay network artificial reverberation tail.
 */

/**
 * @file reverb_jot.c
 *
 * The reverb_jot.c file implements a classic Jot FDN reverberator
 * to add an artificial reverberation tail to each anechoic sound source
 * pointed by @p aave->sources, to simulate the late reflections that the
 * geometry.c part of the auralisation process could not determine in time.
 *
 * @image html fdn_diagram.png "Jot's Feedback Delay Network diagram"
 * @image latex fdn_diagram.eps "Jot's Feedback Delay Network diagram" width=\textwidth
 *
 * N = 64 is the currently adopted circulation matrix order, b[N] is a unitary vector
 * (identity, not implemented) and c[N,2] is a two collumn matrix for decorrelation
 * of stereo output. The circulating matrix a[N,N] is of Householder type, represented
 * by the equation a[N,N] = j[N,N] - 2/N * u[N] * u[N]^T where j[N,N] is a permutation 
 * matrix and u[N] a unitary collumn vector. Amplitude and high frequency damping are
 * performed by @p absorption_filter h(z) and high pass tone correction is performed by 
 * @p tone_correction_filter t(z), for left and right outputs. @p delay_filter z sizes 
 * @p feedback_delays are chosen from a set of prime numbers within the range 0-1800.
 *
 * Reverb predelay @p Tmixing is calculated acording to Jot's aproximation Tmixing = sqrt(Volume).
 * Reverb constant amplitude atenuation @mix is calculated as 1/rc, where 
 * rc = pow((area * abs)/(16 * PI),0.5) is the critical distance at which direct sound
 * energy is equal to reverberation energy, area is the total surface area for the room
 * and abs is the average absorption coeficient of the room. The @p pre_delay value used 
 * is the Tmixing predelay value summed with hrtf buffer size to compensate the latency 
 * introduced by HRTF processing.
 *
 * @todo A @p dc_block_filter was introduced to aproximate low frequency damping. Improve
 * this filter (or introduce another) for flexible bandwidth selection. 
 *
 * Reference:
 * Jasmin Frenette, "REDUCING ARTIFICIAL REVERBERATION ALGORITHM REQUIREMENTS
 * USING TIME-VARIANT FEEDBACK DELAY NETWORKS", Master Thesis, Dec 2000.
 */

#include "aave.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

/** Define a maximum delay time of 200 ms */
#define MAX_DELAY_TIME 8820

/** N = 64 delay sizes for each fdn delay line. Prime numbers within the range 0 - 1800. */
	static const short feedback_delays[FDN_ORDER] = {29,53,79,101,127,149,173,197,223,251,277,307,331,353,379,401,431,457,479,503,541,563,587,613,641,673,701,727,751,773,797,821,853,877,907,929,953,977,1009,1031,1061,1087,1109,1151,1181,1213,1237,1259,1283,1307,1361,1399,1423,1447,1471,1493,1523,1549,1571,1597,1619,1657,1693,1721};

/*
	static const short feedback_delays[FDN_ORDER] = {281,331,373,419,461,503,547,593,641,683,727,769,811,853,907,953,997,1039,1087,1129,1171,1213,1259,1301,1361,1409,1451,1493,1543,1597,1657,1699};*/
/*
	static const short feedback_delays[FDN_ORDER] = {29,53,79,101,127,149,173,197,223,251,277,307,331,353,379,401,431,457,479,503,541,563,587,613,641,673,701,727,751,773,797,821};*/
	/*static const short feedback_delays[FDN_ORDER] = {251,281,311,347,389,409,457,499,563,601,647,691,733,773,839,887,919,977,1031,1129,1151,1171,1187,1201};*/
	/* static const short feedback_delays[FDN_ORDER] = {251,311,347,409,499,563,601,691,733,773,839,887,919,977,1031,1129,1151,1171,1187,1201}; */ 
	/* static const short feedback_delays[FDN_ORDER] = {563,601,691,773,839,887,919,977,997,1031,1061,1093,1129,1151,1171,1187}; */
	/* static const short feedback_delays[FDN_ORDER] = {499,563,601,691,773,839,887,919,977,997,1031,1061,1093,1129,1171,1187}; */
	/* static const short feedback_delays[FDN_ORDER] = {251,311,347,409,499,563,601,691,733,773,839,919,977,1031,1129,1201};*/

/**
 * Data of a delay filter.
 */
struct delay_filter {

    /** Index to the latest value inserted in the buffer. */
    unsigned index;

    /**
     * Buffer to store the inserted values.
     */
    float buffer[MAX_DELAY_TIME];
};

/**
 * Data of a low-pass filter for frequency-dependent reverberation time.
 */
struct absorption_filter {

    /** Previous output of the filter (y[n-1]). */
    float y;
};

/**
 * Data of a tone correction (high pass) filter.
 */
struct tone_correction_filter {

    /** Previous output of the filter (y[n-1]). */
    float y;
};

/**
 * Data of a dc block filter for a rough reverb highpass.
 * @todo Improve bandwidth definition.
 */
struct dc_block_filter {

    /** Previous output of the filter (y[n-1]). */
    float y;
    /** Previous input of the filter (x[n-1]). */
    float x;
    /** bandwidth. */
    float b;
} dcbf[] = {{0,0,0.01},{0,0,0.01}};

/**
 * Execute a delay block: y[n] = x[n - k].
 * @p d is the delay block data,
 * @p x is the input value, and
 * @p k is the number of samples of delay.
 * Returns the output value y[n].
 */
static float process_delay_filter(struct delay_filter *d, float x, unsigned k)
{
    float y;
    unsigned i;

    i = (d->index + 1) % k;
    d->index = i;

    y = d->buffer[i];
    d->buffer[i] = x;

    return y;
}

/**
 * Execute a low-pass filter with gain atenuation: y[n] = g * (1-b) * x[n] + b * y[n-1].
 * @p lp is the low-pass filter data,
 * @p x is the input value,
 * @p b is the bandwidth/damping coefficient.
 * @p b is the gain atenuation.
 * Returns the output value y[n].
 */
static float process_absorption_filter(struct absorption_filter *af, float x, float g, float b)
{
    af->y = g * (1-b) * x + b * af->y;

    return af->y;
}

/**
 * Execute a tone correction (high pass) filter: y[n] = g * (x[n] - b * x[n-1]) / 1-b.
 * @p tcf is the high-pass filter data,
 * @p x is the input value,
 * @p b is the bandwidth/damping coefficient.
 * Returns the output value y[n].
 */
static float process_tone_correction_filter(struct tone_correction_filter *tcf, float x, float b)
{

    float y = (x - b * tcf->y) / (1-b);
    tcf->y = x;

    return y;
}

/**
 * Execute a dc block filter: y[n] = g * (x[n] - x[n-1]) + (1-b) * y[n-1].
 * @p dcbf is the dc block filter data,
 * @p x is the input value,
 * Returns the output value dcbf->y.
 */
static float process_dc_block_filter(struct dc_block_filter *dcbf, float x)
{

    dcbf->y = (1-0.5 * dcbf->b) * (x - dcbf->x) + ((1 - dcbf->b) * dcbf->y);
    dcbf->x = x;

    return dcbf->y;
}

void aave_reverb_print_parameters(struct aave* aave, struct aave_reverb *rev) {

	printf("\n** LATE REVERB PARAMETERS **\n\n");
	printf(" ROOM VOLUME = %d (m3)\n",aave->reverb->volume);
	printf(" ROOM AREA = %d (m2)\n",aave->reverb->area);
	printf(" ROOM ABS COEF = %f\n",aave->room_material_absorption);
	printf(" REVERB MIX = %.2f\n",rev->mix);
	printf(" CRITICAL DISTANCE = %.2f (m)\n",rev->rc);
	printf(" FDN ORDER = %d\n",FDN_ORDER);
	printf(" RT60 = %.2d (ms)\n",aave->reverb->rt60);
	printf(" ALPHA = %.2f\n",rev->alpha);	
	printf(" PREDELAY = %.2f (ms)\n",rev->Tmixing);
	printf("\n****************************\n\n");
}

/**
 * Initialize late reverberation unit parameters.
 * Default values for room volume, area and rt60 will be used.
 */
void aave_reverb_init(struct aave *aave) {

    int i;
    struct aave_surface *surface;

    /* Default values for room area, volume and rt60. */
    unsigned short volume = 5000;
    unsigned short area   = 5000;
    unsigned short RT60   = 2000;

    if (aave->reverb != NULL)
		return;

    aave->reverb = (struct aave_reverb*) malloc(sizeof(struct aave_reverb));
    aave->reverb_active = 1;

    aave->reverb->level = 1;
    aave->reverb_active = 1;

    aave->reverb->pre_delay = aave->hrtf_frames * 2;
    aave->reverb->Tmixing = sqrt((float) volume);
	aave->reverb->pre_delay += aave->reverb->Tmixing * 0.001 * AAVE_FS;

    aave->reverb->alpha = 0.15;
    aave->reverb->beta = (1-sqrt(aave->reverb->alpha))/(1+sqrt(aave->reverb->alpha));

    for (surface = aave->surfaces; surface; surface = surface->next)
	    aave->room_material_absorption += surface->avg_absorption_coef / aave->nsurfaces;

    aave->reverb->rc = pow( (area * aave->room_material_absorption) / (16 * M_PI), 0.5);
    aave->reverb->mix = 1 / aave->reverb->rc;

    for (i=0;i<FDN_ORDER;i++) {

        if (i%2) aave->reverb->decorrelation_coefs[i][0]=-1;
        else aave->reverb->decorrelation_coefs[i][0]=1;

        aave->reverb->fdn_output_taps[i]=0;
        aave->reverb->absorption_gain[i] = pow(10,(-3*feedback_delays[i]*(1000./ AAVE_FS))/RT60);
        aave->reverb->absorption_bandwidth[i] = 1 - ( 2 / (1+pow(aave->reverb->absorption_gain[i],1 - 1./aave->reverb->alpha)));
    }

    for (i=0;i<FDN_ORDER;i+=2) {
        if( i/2 % 2) aave->reverb->decorrelation_coefs[i][1] = aave->reverb->decorrelation_coefs[i+1][1]=-1;
        else aave->reverb->decorrelation_coefs[i][1] = aave->reverb->decorrelation_coefs[i+1][1] = 1;
    }
}

/**
 * Set room volume for late reverberation parametrization.
 * Reverberation unit must already be initialized.
 */
void aave_reverb_set_volume(struct aave *aave, unsigned short volume)
{
	aave->reverb->pre_delay = aave->hrtf_frames * 2;
    aave->reverb->volume = volume;
    aave->reverb->Tmixing = sqrt(volume);
	aave->reverb->pre_delay = aave->reverb->Tmixing * 0.001 * AAVE_FS;
}

/**
 * Set room area for late reverberation parametrization.
 * Reverberation unit must already be initialized.
 */
void aave_reverb_set_area(struct aave *aave, unsigned short area)
{
    struct aave_surface *surface;

    aave->reverb->area = area;

    for (surface = aave->surfaces; surface; surface = surface->next)
	    aave->room_material_absorption += surface->avg_absorption_coef / aave->nsurfaces;

    aave->reverb->rc = pow( (area * aave->room_material_absorption) / (16 * M_PI), 0.5);
    aave->reverb->mix = 1 / aave->reverb->rc;
}

/**
 * Set room rt60 for late reverberation parametrization.
 * Reverberation unit must already be initialized.
 */
void aave_reverb_set_rt60(struct aave *aave, unsigned short RT60)
{
    unsigned short i;

    aave->reverb->rt60 = RT60;

    aave->reverb->alpha = 0.15;
    aave->reverb->beta = (1-sqrt(aave->reverb->alpha))/(1+sqrt(aave->reverb->alpha));

    for (i=0;i<FDN_ORDER;i++) {

        if (i%2) aave->reverb->decorrelation_coefs[i][0]=-1;
        else aave->reverb->decorrelation_coefs[i][0]=1;

        aave->reverb->fdn_output_taps[i]=0;
        aave->reverb->absorption_gain[i] = pow(10,(-3*feedback_delays[i]*(1000./ AAVE_FS))/RT60);
        aave->reverb->absorption_bandwidth[i] = 1 - (2 / (1+pow(aave->reverb->absorption_gain[i],1 - 1./aave->reverb->alpha)));
    }

    for (i=0;i<FDN_ORDER;i+=2) {
        if( i/2 % 2) aave->reverb->decorrelation_coefs[i][1] = aave->reverb->decorrelation_coefs[i+1][1]=-1;
        else aave->reverb->decorrelation_coefs[i][1] = aave->reverb->decorrelation_coefs[i+1][1] = 1;
    }
}

/**
 * Run a Jot FDN reverberator to add an artificial reverberation tail
 * to the @p n single channel frames (@p n samples) of each anechoic
 * sound source pointed by @p aave->sources. Store output in @p audio.
 */
void aave_reverb_jot(struct aave *aave, short *audio, unsigned n)
{

    struct aave_reverb *rev = aave->reverb;
    struct aave_source *source = aave->sources;
    static struct delay_filter predelay;
    static struct tone_correction_filter tcf[2];
    static struct absorption_filter af[FDN_ORDER];
    static struct delay_filter df[FDN_ORDER];
    float x, y[2], buffer[FDN_ORDER];
    unsigned i,j;
    float acum;
      
    /* printf("mix = %.2f, aave_mix_factor = %.2f, rc = %f\n",rev->mix * rev->level, rev->level, rev->rc); */    
    /* printf("source = %p,buffer = %p, n = %d\n",(void*) source, (void*) audio,n); */   
    
    for (i = 0; i < n; i++) {

        acum = y[0] = y[1] = x = 0;
	source = aave->sources;
	
	while (source) {
        x += source->buffer[ (source->buffer_index - n + i) & (AAVE_SOURCE_BUFSIZE - 1)];
	    source = source->next;
	}
        
        x = process_delay_filter(&predelay, x,rev->pre_delay);

        for (j=0;j<FDN_ORDER;j++) {
            buffer[j] = x + rev->fdn_output_taps[j];
            buffer[j] = process_delay_filter(&df[j],buffer[j],feedback_delays[j]);
            buffer[j] = process_absorption_filter(&af[j],buffer[j],rev->absorption_gain[j],rev->absorption_bandwidth[j]);
            acum += buffer[j] * (2./(float)FDN_ORDER);	
        }

        for (j=0;j<FDN_ORDER;j++) {       
            rev->fdn_output_taps[j] = acum - buffer[(j+FDN_ORDER-1) % FDN_ORDER];
            y[0] += rev->decorrelation_coefs[j][0] * buffer[j] * (1./(float)FDN_ORDER);
            y[1] += rev->decorrelation_coefs[j][1] * buffer[j] * (1./(float)FDN_ORDER);       
        }
            
            y[0] = process_tone_correction_filter(&tcf[0],y[0],rev->beta);
            y[1] = process_tone_correction_filter(&tcf[1],y[1],rev->beta);

            y[0] = process_dc_block_filter(&dcbf[0],y[0]);
            y[1] = process_dc_block_filter(&dcbf[1],y[1]);

            audio[i * 2 + 0] = (audio[i * 2 + 0] + (y[0] * rev->mix * rev->level)) * aave->gain;
            audio[i * 2 + 1] = (audio[i * 2 + 1] + (y[1] * rev->mix * rev->level)) * aave->gain; 
    }
}
