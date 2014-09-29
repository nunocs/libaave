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
 *   libaave/idft.h: inverse discrete Fourier transform
 */

/**
 * @file idft.h
 *
 * The idft.h file implements the inverse discrete Fourier transform (IDFT)
 * of Fourier coefficients of real-input data of power-of-2 sizes,
 * using the Cooley-Tukey FFT algorithm
 * (http://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm).
 *
 * It is about 2 times faster than the equivalent complex-Hermitian input
 * to real output plan fftw_plan_dft_c2r_1d of the fftw library
 * (http://www.fftw.org/).
 *
 * The input Fourier coefficients use the same order as returned
 * by the discrete Fourier transform implemented in dft.h.
 * However, the ouput real data values are correctly ordered.
 *
 * This idft.h file is implemented as a "template".
 * To create an idft() function to use in your source code to transform
 * Fourier coefficients back into real data of some type, for example type
 * int (32-bit audio samples), include the following in your source code:
 * @code
 * #define IDFT_TYPE int
 * #include "idft.h"
 * @endcode
 * This will insert a static idft() function in your source code file that
 * transforms Fourier coefficients back into the corresponding integers.
 */

/** Table with the pre-calculated sin() and cos() values. */
extern const float dftsincos[][2];

/**
 * This idft function calculates the @p n point inverse discrete Fourier
 * transform of the Fourier coefficients pointed by @p X
 * and stores the real output data in @p x.
 * @p X points to @p n elements, which correspond to the Fourier
 * coefficients 0 to @p n / 2, in the order described in dft.h.
 * @p x points to @p n elements correctly ordered.
 */
static void idft(IDFT_TYPE *x, float *X, unsigned n)
{
	struct complex { float real; float imag; } *X0, *X1;
	IDFT_TYPE *x0, *x1, *x2, *x3;
	unsigned i, j, k, m;
	float f0, f1, f2, f3, f4, f5;
	float sinx, cosx;

	X0 = (struct complex *)X;

	/* IDFT N=2, N=4, ... , N=n/4 */
	for (m = 1; m < n / 4; m *= 2) {
		X1 = (struct complex *)X + m;
		for (i = 0; i < m; i++) {
			f0 = X0[i].real;
			f1 = X0[i].imag;
			f2 = X1[i].real;
			f3 = X1[i].imag;
			X0[i].real = f0 + f1;
			X0[i].imag = 2 * f2;
			X1[i].real = f0 - f1;
			X1[i].imag = - 2 * f3;
		}
		j = 0;
		while ((i += m) < n / 2) {
			sinx = dftsincos[j][0];
			cosx = dftsincos[j][1];
			j++;
			for (k = 0; k < m; k++) {
				f0 = X0[i].real;
				f1 = X0[i].imag;
				f2 = X1[i].real;
				f3 = X1[i].imag;
				f4 = f0 - f2;
				f5 = f1 + f3;
				X0[i].real = f0 + f2;
				X0[i].imag = f1 - f3;
				X1[i].real = f4 * cosx - f5 * sinx;
				X1[i].imag = f5 * cosx + f4 * sinx;
				i++;
			}
		}
	}

	/* IDFT N=n/4 */
	x0 = x;
	x1 = x + n / 4;
	x2 = x + n / 2;
	x3 = x + n * 3 / 4;
	X1 = (struct complex *)X + n / 4;
	for (i = 0; i < n / 4; i++) {
		f0 = X0[i].real;
		f1 = X0[i].imag;
		f2 = 2 * X1[i].real;
		f3 = 2 * X1[i].imag;
		f4 = f0 + f1;
		f5 = f0 - f1;
		x0[i] = (f4 + f2) / n; /** @todo round instead of truncate */
		x1[i] = (f5 - f3) / n;
		x2[i] = (f4 - f2) / n;
		x3[i] = (f5 + f3) / n;
	}
}
