PROGNAME = eratosthenes
SRCFILES = eratosthenes.c
TARNAME = eratosthenes.tgz

IMAGEVIEW = display
CC = gcc

WARNFLAGS = -Wall -Werror-implicit-function-declaration -Wshadow \
		  -Wstrict-prototypes -pedantic-errors
CFLAGS = -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112
LFLAGS = -lpthread

# Do some substitution to get a list of .o files from the given .c files.
OBJFILES = $(patsubst %.c,%.o,$(SRCFILES))

.PHONY: all run runlocal plot clean dist todo

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS)

runlocal: $(PROGNAME)
	./$(PROGNAME) $(RUNARGS)

todo:
	-@for file in *.c *.h; do \
		grep -FHnT -e TODO $$file | \
			sed 's/:[ \t]*\/\//: \/\//' | \
			sed 's/:[ \t]*\/\*/: \/\*/'; \
		done; true

dist:
	tar cvzf $(TARNAME) Makefile *.c *.h

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME)
