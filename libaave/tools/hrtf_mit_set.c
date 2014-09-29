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
 *   libaave/tools/hrtf_mit.c: generate file with HRTFs of MIT compact set
 *   Usage: ./hrtf_mit_set compact/elev{-40,-30,-20,-10,0,10,20,30,40,50,60,70,80,90}/H*.wav > ../hrtf_mit_set.c
 */

#include <stdio.h>

#define HRIR_SIZE 128

#define DFT_N (4 * HRIR_SIZE)
#define DFT_TYPE float
#include "../dft.h"

static void hrtf_mit(const char *filename)
{
	FILE *f;
	short stereo[HRIR_SIZE * 2];
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

	if (fread(stereo, 4, HRIR_SIZE, f) != HRIR_SIZE) {
		fprintf(stderr, "read error in file %s\n", filename);
		return;
	}

	fclose(f);

	/* Left channel. */
	printf("{{");
	for (i = 0; i < HRIR_SIZE; i++)
		mono[i] = stereo[2 * i] / 32768.;
	dft(hrtf, mono, DFT_N);
	for (i = 0; i < DFT_N; i++)
		printf("%e,", hrtf[i]);

	/* Right channel. */
	printf("},{");
	for (i = 0; i < HRIR_SIZE; i++)
		mono[i] = stereo[2 * i + 1] / 32768.;
	dft(hrtf, mono, DFT_N);
	for (i = 0; i < DFT_N; i++)
		printf("%e,", hrtf[i]);

	printf("}},");
}

int main(int argc, char **argv)
{
	printf("/* This file was automatically generated."
		" See tools/hrtf_mit_set.c */\n"
		"const float hrtf_mit_set[][2][%u]={", DFT_N);

	while (*++argv)
		hrtf_mit(*argv);

	printf("};\n");

	return 0;
}
