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

int main(int argc, char **argv) {
   int sync;
   int rank, total;
   int start, end, prevoffset, nextoffset, active_node;
   char buff = 2;
   MPI_Status status;

   msginit(&argc,&argv,&rank,&total);

   sync = atoi(argv[1]);

   if (sync == 1 ) {
      start = 0;
      end = total;
      prevoffset = nextoffset = -rank;
   }

   else if (sync == 2) {
      start = 0;
      end = total - 1;
      prevoffset = -1;
      nextoffset = 1;
   }

   else if (sync == 3) {
      start = total - 1;
      end = 0;
      prevoffset = 1;
      nextoffset = -1;
   }

   /* block all non-start nodes */
   if (rank != start) { msgrecv(&buff, rank + prevoffset); }

   if (sync == 1 && rank == 0) { 
      /* manager-worker's manager node */
      printf("hello: %d processes, process %d\n", total, rank);
      for (active_node = 1; active_node < total; active_node++) {
         msgsend(&buff, active_node);
         msgrecv(&buff, active_node);
      }
   }
   else { printf("hello: %d processes, process %d\n", total, rank); }

   /* continue passing message if not end */
   if (rank != end) { msgsend(&buff, rank + nextoffset); }

   fflush(stdout);
   MPI_Finalize();
   return 0;
}
