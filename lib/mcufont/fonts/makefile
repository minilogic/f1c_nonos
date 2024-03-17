# Path to the command-line utility program
MCUFONT = mcufont.exe

# Names of fonts to process
FONTS = comic36 comic72 \
	times36 times72 \
	DejaVuSans12bw DejaVuSerif16 DejaVuSerif-Bold24 DejaVuSerif32 \
	fixed_5x8 fixed_7x14 fixed_10x20

FFONTS = DejaVuSans12 DejaVuSans12bw DejaVuSerif16 DejaVuSerif32 \
	fixed_5x8 fixed_7x14 fixed_10x20 DejaVuSans12bw_bwfont

# Characters to include in the fonts
CHARS = 0-255 0x2010-0x2015

all: $(FONTS:=.c) $(FONTS:=.dat)

clean:
	rm -f $(FONTS:=.c) $(FONTS:=.dat)

%.c: %.dat $(MCUFONT)
	$(MCUFONT) rlefont_export $<

comic36.dat: comic.ttf
	$(MCUFONT) import_ttf $< 36
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

comic72.dat: comic.ttf
	$(MCUFONT) import_ttf $< 72
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

times36.dat: times.ttf
	$(MCUFONT) import_ttf $< 36
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

times72.dat: times.ttf
	$(MCUFONT) import_ttf $< 72
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

DejaVuSans12bw.dat: DejaVuSans.ttf
	$(MCUFONT) import_ttf $< 12 bw
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

DejaVuSerif16.dat: DejaVuSerif.ttf
	$(MCUFONT) import_ttf $< 16
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

DejaVuSerif-Bold24.dat: DejaVuSerif-Bold.ttf
	$(MCUFONT) import_ttf $< 24
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

DejaVuSerif32.dat: DejaVuSerif.ttf
	$(MCUFONT) import_ttf $< 32
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50

%.dat: %.bdf
	$(MCUFONT) import_bdf $<
	$(MCUFONT) filter $@ $(CHARS)
	$(MCUFONT) rlefont_optimize $@ 50
