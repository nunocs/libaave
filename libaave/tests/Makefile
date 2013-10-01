CC = gcc
CFLAGS = -Wall -pedantic -I.. -O2 -flto
LDFLAGS = ../libaave.a

programs = dftidft dftindex material

all: $(programs)

dftidft: dftidft.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

dftindex: dftindex.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

material: material.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS) -lm

clean:
	$(RM) $(programs)
