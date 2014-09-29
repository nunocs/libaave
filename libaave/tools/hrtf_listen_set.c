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
 *   Copyright 2013 André Oliveira, Nuno Silva, Guilherme Campos,
 *   Paulo Dias, José Vieira/IEETA - Universidade de Aveiro
 *
 *
 *   libaave/tools/hrtf_listen_set.c: generate file with HRTFs of a LISTEN set
 *   Usage: ./hrtf_listen_set XXXX IRC_XXXX_C_*P3*.wav IRC_XXXX_C_*P0*.wav > ../hrtf_listen_set_XXXX.c
 */

#include <stdio.h>

/* Frames per HRTF. */
#define HRIR_SIZE 512

/* DFT length (zero-padding). */
#define DFT_N (HRIR_SIZE * 4)

#define DFT_TYPE float
#include "../dft.h"

static void hrtf_listen(const char *filename)
{
	FILE *f;
	unsigned char stereo[HRIR_SIZE * 6]; /* 24 bits, 2 channels */
	static DFT_TYPE mono[HRIR_SIZE * 2];
	float hrtf[DFT_N];
	unsigned i;

	f = fopen(filename, "rb");
	if (!f) {
		fprintf(stderr, "error opening file %s\n", filename);
		return;
	}

	/* Seek to the begining of the sample data, in the data chunk. */
	if (fseek(f, 44, SEEK_SET)) {
		fprintf(stderr, "seek error in file %s\n", filename);
		return;
	}

	if (fread(stereo, 6, HRIR_SIZE, f) != HRIR_SIZE) {
		fprintf(stderr, "read error in file %s\n", filename);
		return;
	}

	fclose(f);

	/* Left channel. */
	printf("{{");
	for (i = 0; i < HRIR_SIZE; i++)
		mono[i] = (stereo[6*i+0] | stereo[6*i+1]<<8 | ((signed char *)stereo)[6*i+2]<<16) / 16777215.;
	dft(hrtf, mono, DFT_N);
	for (i = 0; i < DFT_N; i++)
		printf("%e,", hrtf[i]);

	/* Right channel. */
	printf("},{");
	for (i = 0; i < HRIR_SIZE; i++)
		mono[i] = (stereo[6*i+3] | stereo[6*i+4]<<8 | ((signed char *)stereo)[6*i+5]<<16) / 16777215.;
	dft(hrtf, mono, DFT_N);
	for (i = 0; i < DFT_N; i++)
		printf("%e,", hrtf[i]);

	printf("}},");
}

int main(int argc, char **argv)
{
	printf("/* This file was automatically generated."
		" See tools/hrtf_listen_set.c */\n"
		"const float hrtf_listen_set_%s[][2][%u]={", *++argv, DFT_N);

	while (*++argv)
		hrtf_listen(*argv);

	printf("};\n");

	return 0;
}
