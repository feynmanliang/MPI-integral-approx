#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
   int rank, total;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &total);

   printf("hello: %d processes, process %d\n", total, rank);
   fflush(stdout);

   MPI_Finalize();
   return 0;
}
