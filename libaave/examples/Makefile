# Path to the libaave directory
LIBAAVE = ..

CC = gcc
CFLAGS = -Wall -pedantic -I$(LIBAAVE) -O2 -flto -g
LDFLAGS = $(LIBAAVE)/libaave.a -lm

programs = circle elevation line

all: $(programs)

circle: circle.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

elevation: elevation.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

line: line.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(programs)
