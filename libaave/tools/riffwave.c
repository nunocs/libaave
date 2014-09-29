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
 *   libaave/tools/riffwave.c: RIFF WAVE file format dump
 */

#include <stdio.h>

struct header {
	/* "RIFF" chunk descriptor. */
	unsigned ChunkID;
	unsigned ChunkSize;
	unsigned Format;

	/* "fmt " sub-chunk. */
	unsigned Subchunk1ID;
	unsigned Subchunk1Size;
	unsigned short AudioFormat;
	unsigned short NumChannels;
	unsigned SampleRate;
	unsigned ByteRate;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;

	/* "data" sub-chunk. */
	unsigned Subchunk2ID;
	unsigned Subchunk2Size;
	char data[1];
};

int main(int argc, char **argv)
{
	FILE *f;
	struct header header;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE.WAV\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "rb");
	if (!f) {
		perror(argv[1]);
		return 1;
	}

	if (fread(&header, 1, sizeof header, f) != sizeof header) {
		perror("fread");
		return 1;
	}

	printf("ChunkID: %xh\n", header.ChunkID);
	printf("ChunkSize: %u\n", header.ChunkSize);
	printf("Format: %xh\n", header.Format);

	printf("Subchunk1ID: %xh\n", header.Subchunk1ID);
	printf("Subchunk1Size: %u\n", header.Subchunk1Size);
	printf("AudioFormat: %xh\n", header.AudioFormat);
	printf("NumChannels: %u\n", header.NumChannels);
	printf("SampleRate: %u\n", header.SampleRate);
	printf("ByteRate: %u\n", header.ByteRate);
	printf("BlockAlign: %u\n", header.BlockAlign);
	printf("BitsPerSample: %u\n", header.BitsPerSample);

	printf("Subchunk2ID: %xh\n", header.Subchunk2ID);
	printf("Subchunk2Size: %u\n", header.Subchunk2Size);

	return 0;
}
