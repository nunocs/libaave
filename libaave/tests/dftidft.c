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
 *   libaave/test/dftidft.c: test the correctness of dft() and idft()
 */

#include <stdio.h>
#include "aave.h"

#define DFT_TYPE short
#include "../dft.h"

#define IDFT_TYPE int
#include "../idft.h"

#define N 4096

int main()
{
	short x[N/2];
	int y[N];
	float X[N];
	unsigned i;

	dft(X, x, N);
	idft(y, X, N);

	for (i = 0; i < N/2; i++)
		if (x[i] > y[i] + 1 || x[i] < y[i] - 1)
			fprintf(stderr, "%u: x=%d y=%d\n", i, x[i], y[i]);

	for ( ; i < N; i++)
		if (y[i] != 0)
			fprintf(stderr, "%u: y=%d\n", i, y[i]);

	return 0;
}
