CC = gcc
CFLAGS = -Wall -pedantic -I.. -O2 -flto
LDFLAGS = ../../libaave/libaave.a

programs = benchaud dftidft dftindex material

all: $(programs)

benchaud: benchaud.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS) -lm

dftidft: dftidft.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

dftindex: dftindex.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

material: material.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS) -lm

clean:
	$(RM) $(programs)
