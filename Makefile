CC=gcc
CFLAGS= -ansi -Wall -pedantic -g -pthread -std=c99

CFILES=integrate.c client.c
HFILES=integrate.h
OFILES=integrate.o client.o

all: client

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

client: $(OFILES) $(HFILES)
	 $(CC) $(CFLAGS) $(OFILES) -o client

clean:	
	rm -f *~ client integrate.o client.o

