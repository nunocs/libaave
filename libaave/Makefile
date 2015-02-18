CC = gcc
CFLAGS = -Wall -pedantic -O2 -flto -fPIC # -g

objects += audio.o
objects += dftindex.o
objects += dftsincos.o
objects += geometry.o
objects += hrtf_cipic.o
objects += hrtf_cipic_set_008.o
objects += hrtf_listen.o
objects += hrtf_listen_set_1040.o
objects += hrtf_mit.o
objects += hrtf_mit_set.o
objects += hrtf_tub.o
objects += hrtf_tub_set.o
objects += hrtf_id.o
objects += init.o
objects += material.o
objects += obj.o
objects += reverb_dattorro.o
objects += reverb_jot.o

libaave.a: $(objects)
	$(AR) crs $@ $+

$(objects): aave.h

clean:
	$(RM) libaave.a $(objects)
