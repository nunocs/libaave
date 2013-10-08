/*
 * libaave/idft.h: inverse discrete Fourier transform
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file idft.h
 *
 * This idft function calculates the N-point inverse discrete Fourier transform
 * of the X coefficients and stores the result in x.
 * X points to N elements in the format described in dft.h.
 * x points to N elements.
 */

/* Table with the pre-calculated sin() and cos() values. */
extern const float dftsincos[][2];

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
		x0[i] = (f4 + f2) / n; /* TODO: round? */
		x1[i] = (f5 - f3) / n;
		x2[i] = (f4 - f2) / n;
		x3[i] = (f5 + f3) / n;
	}
}
