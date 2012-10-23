CC=mpicc
CFLAGS= -ansi -Wall -pedantic -g -pthread -std=c99

CFILES=threaded_integrate.c mpi_integrate.c integrate.c 
HFILES=integrate.h threaded_integrate.h
OFILES=threaded_integrate.o mpi_integrate.o integrate.o
BIN=mpi_integrate

all:	sync_ctl

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

sync_ctl:	$(OFILES) $(HFILES)
	        $(CC) $(CFLAGS) $(OFILES) -o $(BIN)


clean:	
	rm -f *~ $(OFILES) $(BIN)

mpirun: 
	mpirun -hostfile myhosts ./$(BIN) 1 0 10
