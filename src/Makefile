IDIR = ../headers
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj


_DEPS = transmission_utils.h webserver.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o transmission_utils.o webserver.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

webserver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 