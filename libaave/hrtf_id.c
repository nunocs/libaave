#include "aave.h"

static float hrtf_identity_set[512];

/***
 * Load an unitary complex vector so that no hrtf processing is applied.
 */
static void aave_hrtf_identity_get(const float *hrtf[2], int elevation, int azimuth)
{
	hrtf[0] = hrtf_identity_set;
	hrtf[1] = hrtf_identity_set;
}

/**
 * Select the identity HRTF set for testing the auralisation process.
 */
void aave_hrtf_identity(struct aave *a)
{
    unsigned i;
    
	a->hrtf_frames = 128;
	a->hrtf_get = aave_hrtf_identity_get;
	a->hrtf_output_buffer_index = 0;
	
	for (i=0; i<256; i++) {
	    hrtf_identity_set[i*2] = 1.;
	    hrtf_identity_set[i*2+1] = 0.;
    }
}
