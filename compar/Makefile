PROG = libmatching.so

CC = gcc

IDIR = src/include
LDIR = src/lib

LIBS = -lm -lconfuse

_OBJ = coef_dist.o matching.o interface.o
OBJ = $(patsubst %,$(LDIR)/%,$(_OBJ))

_DEPS = coef_dist.h matching.h interface.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

CFLAGS = -fPIC -I$(IDIR) -Wall
CFLAGS2 = -shared -Wall
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)
	
$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS2) $(LIBS)
	
cleam:
	rm *.~