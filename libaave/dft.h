/*
 * libaave/dft.h: discrete Fourier transform
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file dft.h
 *
 * The dft.h file implements the discrete Fourier transform (DFT) of
 * real-input data of power-of-2 sizes, using the Cooley-Tukey FFT algorithm
 * (http://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm).
 *
 * It is about 3 times faster than the equivalent real input to
 * complex-Hermitian output plan fftw_plan_dft_r2c_1d of the fftw library
 * (http://www.fftw.org/). Furthermore, it implicitly type-converts and
 * zero-pads the input data to twice the size, making the comparison even
 * more favorable for this implementation.
 *
 * The drawback is that the ouput Fourier coefficients end up unordered.
 * However, this is irrevelant for our main purpose of performing fast
 * convolutions, because the corresponding inverse discrete Fourier
 * transform (IDFT) implemented in idft.h also uses the same order.
 * For the case where it is necessary to know the order of the Fourier
 * coefficients, namely the design of the material absorption filters
 * in material.c, the function dft_index() can be used to retrieve the
 * Fourier coefficients in any desired order.
 *
 * This dft.h file is implemented as a "template".
 * To create a dft() function to use in your source code to transform
 * input data of some type, for example type short (16-bit audio samples),
 * include the following in your source code file:
 * @code
 * #define DFT_TYPE short
 * #include "dft.h"
 * @endcode
 * This will insert a static dft() function in your source code file
 * that calculates the Fourier coefficients of an array of short integers.
 */

/** Table with the pre-calculated sin() and cos() values. */
extern const float dftsincos[][2];

/**
 * This dft function calculates the @p n point discrete Fourier transform
 * of the zero-padded real-input data values pointed by @p x
 * and stores the Fourier coefficients in @p X.
 * @p x points to @p n / 2 elements (elements @p n / 2 to @p n - 1 are
 * implicitly zero-padded).
 * @p X points to @p n elements, which correspond to the Fourier
 * coefficients 0 to @p n / 2, (un)ordered as follows:
 * - X[0] = X[0].real;
 * - X[1] = X[N/2].real;
 * - X[2] = X[N/4].real;
 * - X[3] = X[N/4].imag;
 * - etc... (see dft_index() for the complete ordering)
 *
 * Remember that when the input data is real:
 * - X[0].imag = 0;
 * - X[N/2].imag = 0;
 * - X[N/2+i].real = X[N/2-i].real;
 * - X[N/2+i].imag = - X[N/2-i].imag;
 */
static void dft(float *X, const DFT_TYPE *x, unsigned n)
{
	struct complex { float real; float imag; } *X0, *X1;
	const DFT_TYPE *x0, *x1;
	DFT_TYPE a, b;
	float f0, f1, f2, f3, f4, f5;
	float cosx, sinx;
	unsigned i, j, k, m;

	/* DFT N=4 */
	x0 = x;
	x1 = x + n / 4;
	X0 = (struct complex *)X;
	X1 = (struct complex *)X + n / 4;
	for (i = 0; i < n / 4; i++) {
		a = x0[i];
		b = x1[i];
		X0[i].real = a + b;
		X0[i].imag = a - b;
		X1[i].real = a;
		X1[i].imag = -b;
	}

	/* DFT N=8, N=16, ... , N=m */
	for (m = n / 8; m; m /= 2) {
		X1 = (struct complex *)X + m;
		for (i = 0; i < m; i++) {
			f0 = X0[i].real;
			f1 = X0[i].imag;
			f2 = X1[i].real;
			f3 = X1[i].imag;
			X0[i].real = f0 + f2;
			X0[i].imag = f0 - f2;
			X1[i].real = f1;
			X1[i].imag = - f3;
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
				f4 = f2 * cosx + f3 * sinx;
				f5 = f3 * cosx - f2 * sinx;
				X0[i].real = f0 + f4;
				X0[i].imag = f1 + f5;
				X1[i].real = f0 - f4;
				X1[i].imag = f5 - f1;
				i++;
			}
		}
	}
}
