images := aave audio1 audio2 audio3 reverb
images := $(foreach i,$(images),$(i:%=images/%.eps) $(i:%=images/%.png))

all: $(images)
	doxygen
	$(MAKE) -C latex

%.eps: %.gv
	dot -Teps -o$@ $+

%.png: %.gv
	dot -Tpng -o$@ $+

clean:
	$(RM) -fr html latex $(images)
