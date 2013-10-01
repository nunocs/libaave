/*
 * libaave/dftindex.c: calculate the indices into DFT data
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

/*
 * The implemented DFT stores the values in non-sequencial order.
 * This dft_index() function returns the index into the DFT data that
 * corresponds to the sequencial index I of a DFT of length N.
 */

/*
 * Lookup table, for N = 128.
 * The dft_index() function therefore only accepts N <= 128.
 */
static const unsigned char dft_index_table[] = {
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

unsigned dft_index(unsigned i, unsigned n)
{
	return dft_index_table[i * (128 / n)];
}
