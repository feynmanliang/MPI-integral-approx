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

#define NEXT (rank + total + dir) % total

#define PREV (rank + total - dir) % total

int main(int argc, char **argv) {
   int rank, total;
   int active_node;
   char buff;
   int dir;
   MPI_Status status;

   msginit(&argc,&argv,&rank,&total);

   dir = atoi(argv[1]);

   if (rank == 0) {
      if (dir == 0) {
         /* manager-worker's manager node */
         printf("hello: %d processes, process %d\n", total, rank);
         for (active_node = 1; active_node < total; active_node++) {
            msgsend(&buff, active_node);
            msgrecv(&buff, active_node);
            /* also serves as printer node */
            printf("hello: %d processes, process %d\n", total, active_node);
         }
      }
      else {
         printf("hello: %d processes, process %d\n", total, rank);
         msgsend(&buff, NEXT);
         /* printer node */
         for (active_node = NEXT; 
           ((active_node + total - dir) % total ) != PREV;
           active_node = (active_node + total + dir) % total) {
            msgrecv(&buff, active_node);
            printf("hello: %d processes, process %d\n", total, active_node);
            msgsend(&buff, active_node)
         }
         msgrecv(&buff, PREV);
      }
   }

   else {
      if (dir == 0) { 
         msgrecv(&buff, 0); 
         msgsend(&buff, 0); 
      }
      else { 
         msgrecv(&buff, PREV); 
         msgsend(&buff, 0);
         msgrecv(&buff, 0);
         msgsend(&buff, NEXT); 
      }
   }

   fflush(stdout);
   MPI_Finalize();
   return 0;
}
