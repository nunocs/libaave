/*
 * libaave/dft.h: discrete Fourier transform
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * This dft function calculates the N-point discrete Fourier transform of
 * the zero-padded real data values pointed by x and stores the result in X.
 * x points to N/2 elements (elements N/2 to N-1 would be 0).
 * X points to N elements as follows:
 * X[0] = X[0].real;
 * X[1] = X[N/2].real;
 * X[2] = X[N/4].real;
 * X[3] = X[N/4].imag;
 * etc...
 * X[N-2] = X[N/2-1].real;
 * X[N-1] = X[N/2-1].imag;
 *
 * Remember that, when the input data is real:
 * X[0].imag = 0;
 * X[N/2].imag = 0;
 * X[N/2+i].real = X[N/2-i].real;
 * X[N/2+i].imag = - X[N/2-i].imag;
 */

/* Table with the pre-calculated sin() and cos() values. */
extern const float dftsincos[][2];

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
