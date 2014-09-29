/*
 * libaave/dftindex.c: calculate the indices into DFT data
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/**
 * @file dftindex.c
 *
 * The discrete Fourier transform (DFT) implementation in dft.h stores the
 * Fourier coefficients in non-sequential order. This dftindex.c file
 * implements a dft_index() function that returns the index into the
 * calculated DFT data that corresponds to each Fourier coefficient.
 */

/**
 * The DFT index lookup table, for N = 128.
 * This means the dft_index() function therefore only supports N <= 128.
 * @todo If the order of the material absorption filter designed in
 * material.c increases to N > 128, increase this table accordingly.
 */
static const unsigned char dft_index_table[] = {
/*	N=4	N=8	N=16	N=32	N=64	N=128	*/
	0,
						32,
					16,
						48,
				8,
						56,
					24,
						40,
			4,
						44,
					28,
						60,
				12,
						52,
					20,
						36,
		2,
						38,
					22,
						54,
				14,
						62,
					30,
						46,
			6,
						42,
					26,
						58,
				10,
						50,
					18,
						34,
	1,
						35,
					19,
						51,
				11,
						59,
					27,
						43,
			7,
						47,
					31,
						63,
				15,
						55,
					23,
						39,
		3,
						37,
					21,
						53,
				13,
						61,
					29,
						45,
			5,
						41,
					25,
						57,
				9,
						49,
					17,
						33,
};

/**
 * This function returns the index into the DFT data calculated by dft()
 * that contains the Fourier coefficient @p i for a DFT of size @p n.
 * @p n is a power of 2, up to the maximum supported by dft_index_table
 * (currently 128). @p i is a value from 0 up to @p n / 2 - 1,
 * since the input data is real:
 * - X[0] = X[0].real; X[0].imag = 0;
 * - X[1] = X[N/2].real; X[N/2].imag = 0;
 * - X[N/2+i].real = X[N/2-i].real;
 * - X[N/2+i].imag = - X[N/2-i].imag;
 */
unsigned dft_index(unsigned i, unsigned n)
{
	return dft_index_table[i * (128 / n)];
}
