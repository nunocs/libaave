#!/bin/sh
subject=1040
x=../../../hrtf/listen/COMPENSATED/WAV/IRC_${subject}_C/IRC_${subject}_C_R0195_T
./hrtf_listen_set ${subject} \
	${x}*P315.wav \
	${x}*P330.wav \
	${x}*P345.wav \
	${x}*P000.wav \
	${x}*P015.wav \
	${x}*P030.wav \
	${x}*P045.wav \
	${x}*P060.wav \
	${x}*P075.wav \
	${x}*P090.wav \
	> ../hrtf_listen_set_${subject}.c
