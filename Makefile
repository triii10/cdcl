SRCDIR =src

IDIR =src/inc
CC=g++
CFLAGS=-I$(IDIR) -std=c++14 -g

ODIR=src/obj
LDIR =../lib

LIBS=

DEPS = $(IDIR)/%.hpp

all: main

_DEPS = io.hpp main.hpp structure.hpp cdcl.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = io.o main.o cdcl.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: main clean

clean:
	rm -f $(ODIR)/*.o *~ core main