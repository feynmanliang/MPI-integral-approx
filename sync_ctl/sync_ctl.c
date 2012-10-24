#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MYTAG 15

#define msginit(argcp, argvp, rankp, totalp) {\
   MPI_Init(argcp, argvp);\
   MPI_Comm_rank(MPI_COMM_WORLD, rankp);\
   MPI_Comm_size(MPI_COMM_WORLD, totalp);\
}  

#define msgsend(buffp, to) \
  MPI_Send(buffp, sizeof(buffp), MPI_BYTE, to, MYTAG, MPI_COMM_WORLD);

#define msgrecv(buffp, from) \
  MPI_Recv(buffp, sizeof(buffp), MPI_BYTE, from, MYTAG, MPI_COMM_WORLD, &status);

#define NEXT (dir != 0) ? ((rank + total - 1 + dir) % (total - 1)) : 0

#define PREV (dir != 0) ? ((rank + total - 1 - dir) % (total - 1)) : 0

#define PRINTER total - 1

int main(int argc, char **argv) {
   int rank, total;
   int active_node;
   char buff;
   int dir;
   int numPrinted;
   MPI_Status status;

   msginit(&argc,&argv,&rank,&total);

   dir = atoi(argv[1]);

   if (rank == 0) {
      msgsend(&buff, PRINTER);
      msgrecv(&buff, PRINTER);
      if (dir == 0) {
         /* manager-worker's manager node */
         for (active_node = 1; active_node < total - 1; active_node++) {
            msgsend(&buff, active_node);
            msgrecv(&buff, active_node);
         }
      }
      else {
         msgsend(&buff, NEXT);
         msgrecv(&buff, PREV);
      }
   }

   else if (rank == PRINTER) {
      numPrinted = 0;
      while (numPrinted < total - 1) {
         msgrecv(&buff, MPI_ANY_SOURCE);
         printf("hello: %d processes, process %d\n", total-1, status.MPI_SOURCE);
         numPrinted++;
         msgsend(&buff, status.MPI_SOURCE);
      }
   }

   else {
      msgrecv(&buff, PREV); 
      msgsend(&buff, PRINTER);
      msgrecv(&buff, PRINTER);
      msgsend(&buff, NEXT); 
   }

   fflush(stdout);
   MPI_Finalize();
   return 0;
}
