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
 *   libaave/tools/sin.c: generate a sinusoid sound file
 */

#include <math.h> /* M_PI, sin() */
#include <stdio.h> /* fwrite() */
#include <stdlib.h> /* atoi() */

/* Sampling frequency (Hz). */
#define FS 44100

int main(int argc, char **argv)
{
	unsigned i, f, n;
	double w;
	short x;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s HZ\n", argv[0]);
		return 1;
	}

	/* Frequency of the sinusoid. */
	f = atoi(argv[1]);

	/* Generate one period of the sinusoid. */
	n = FS / f;
	w = 2 * M_PI / FS * f;
	for (i = 0; i < n; i++) {
		x = sin(w * i) * 32767;
		fwrite(&x, 2, 1, stdout);
	}

	return 0;
}
