CC=mpicc
CFLAGS= -ansi -Wall -pedantic -g -std=gnu99

CFILES=sync_ctl.c
HFILES=
OFILES=sync_ctl.o
BIN=sync_ctl

all:	sync_ctl

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

sync_ctl:	$(OFILES) $(HFILES)
	        $(CC) $(CFLAGS) $(OFILES) -o $(BIN)


clean:	
	rm -f *~ $(OFILES) $(BIN)

mpirun_manager: 
	mpirun -hostfile myhosts ./$(BIN) 0
	
mpirun_token: 
	mpirun -hostfile myhosts ./$(BIN) 1

mpirun_reverse: 
	mpirun -hostfile myhosts ./$(BIN) -1
