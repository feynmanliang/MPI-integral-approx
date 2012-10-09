CC=mpicc
CFLAGS= -ansi -Wall -pedantic -g -std=gnu99

CFILES=starter.c
HFILES=
OFILES=starter.o

all:	starter

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

starter:	starter.o $(HFILES)
	        $(CC) $(CFLAGS) starter.o -o starter


clean:	
	rm -f *~ starter starter.o

