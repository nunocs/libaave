# Path to the libaave directory
LIBAAVE = ..

CC = gcc
CFLAGS = -Wall -pedantic -I$(LIBAAVE) -O2 -flto
LDFLAGS = $(LIBAAVE)/libaave.a -lm

programs = circle elevation line stream

all: $(programs)

circle: circle.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

elevation: elevation.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

line: line.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

stream: stream.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lasound -s

clean:
	$(RM) $(programs)
