/*
 * libaave/tools/riffwave.c: RIFF WAVE file format dump
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
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
