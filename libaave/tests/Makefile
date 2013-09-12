CC = gcc
CFLAGS = -Wall -pedantic -I.. -O2 -flto
LDFLAGS = ../libaave.a

programs = dftidft

all: $(programs)

dftidft: dftidft.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

clean:
	$(RM) $(programs)
