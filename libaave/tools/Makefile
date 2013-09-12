CC = gcc
CFLAGS = -Wall -pedantic -I.. -O2

programs += dftsincos
programs += hrtf_cipic_set
programs += hrtf_listen_set
programs += hrtf_mit_set
programs += hrtf_tub_set
programs += noise
programs += riffwave
programs += sin

all: $(programs)

dftsincos: dftsincos.c
	$(CC) $(CFLAGS) -o $@ $+ -lm

hrtf_cipic_set: hrtf_cipic_set.c ../dftsincos.c
	$(CC) $(CFLAGS) -ansi -o $@ $+

hrtf_listen_set: hrtf_listen_set.c ../dftsincos.c
	$(CC) $(CFLAGS) -ansi -o $@ $+

hrtf_mit_set: hrtf_mit_set.c ../dftsincos.c
	$(CC) $(CFLAGS) -ansi -o $@ $+

hrtf_tub_set: hrtf_tub_set.c ../dftsincos.c
	$(CC) $(CFLAGS) -ansi -o $@ $+

noise: noise.c
	$(CC) $(CFLAGS) -ansi -o $@ $+

riffwave: riffwave.c
	$(CC) $(CFLAGS) -ansi -o $@ $+

sin: sin.c
	$(CC) $(CFLAGS) -o $@ $+ -lm

clean:
	$(RM) $(programs)
