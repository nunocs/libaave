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
 *   libaave/tests/dftindex.c: test the dft() and dft_index() functions
 */

#include <stdio.h>
#include <stdlib.h>
#include "aave.h"

#define DFT_TYPE int
#include "../dft.h"

int main(int argc, char **argv)
{
	DFT_TYPE *x;
	float *X;
	unsigned i, j, n;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s N\n", argv[0]);
		return 1;
	}

	n = atoi(argv[1]);

	x = malloc(n / 2 * sizeof *x);
	X = malloc(n * sizeof *X);

	for (i = 0; i < n / 2; i++)
		x[i] = i;

	dft(X, x, n);

	printf("X[0] = %f\n", X[0]);
	for (i = 1; i < n / 2; i++) {
		j = dft_index(i, n);
		printf("X[%u-%u] = %f + j %f\n", i, j, X[j * 2], X[j * 2 + 1]);
	}
	printf("X[%u] = %f\n", n / 2, X[1]);

	return 0;
}
