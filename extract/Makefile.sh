PROG = libextraction.so

CC = gcc

IDIR = include
LDIR = lib

LIBS = -lm -lconfuse

_OBJ = converte.o DCT.o extract.o FFT.o funcoes.o mfcc.o wav.o
OBJ = $(patsubst %,$(LDIR)/%,$(_OBJ))

_DEPS = converte.h DCT.h FFT.h funcoes.h mfcc.h wav.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

CFLAGS = -fPIC -I$(IDIR)
CFLAGS2 = -shared
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)
	
$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS2) $(LIBS)
	
cleam:
	rm *.~