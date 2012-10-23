#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>
#include "./integrate.h"
#include "./threaded_integrate.h"

#define MYTAG 15

#define msginit(argcp, argvp, rankp, totalp) {\
   MPI_Init(argcp, argvp);\
   MPI_Comm_rank(MPI_COMM_WORLD, rankp);\
   MPI_Comm_size(MPI_COMM_WORLD, totalp);\
}  

#define msgsend(buffp, to) \
  MPI_Send(buffp, 1, mpi_approx, to, MYTAG, MPI_COMM_WORLD);

#define msgrecv(buffp, from) \
  MPI_Recv(buffp, 1, mpi_approx, from, MYTAG, MPI_COMM_WORLD, &status);

#define MPI_WTIME_IS_GLOBAL true

double fn0(double);
double fn1(double);
double fn2(double);
double fn3(double);

int main(int argc, char *argv[]) {
   double (*funcArr[3])(double);
   int fn_num;
   double start;
   double end;
   double precision;
   double node_portion;
   int thds_per_node;
   int rank, total;
   int active_node;
   MPI_Status status;

   msginit(&argc,&argv,&rank,&total);

   /* construct derived datatype */
   int blocklengths[2] = {1, 1};
   MPI_Datatype types[2] = {MPI_DOUBLE, MPI_INT};
   MPI_Datatype mpi_approx;
   MPI_Aint offsets[2];
   offsets[0] = offsetof(struct integralApprox, value);
   offsets[1] = offsetof(struct integralApprox, num_strips);
   MPI_Type_create_struct(2, blocklengths, offsets, types, &mpi_approx);
   MPI_Type_commit(&mpi_approx);

   if ( argc < 4 ) {
      printf( "usage: %s fn_num start end (precision=0.01) (thds_per_node=10)\n", argv[0] );
      exit(0);
   }
   else {
      funcArr[0] = &fn0;
      funcArr[1] = &fn1;
      funcArr[2] = &fn2;
      funcArr[3] = &fn3;

      start = atof(argv[2]);
      end = atof(argv[3]);
      node_portion = (end - start) / (double) (total-1);
      fn_num = atoi(argv[1]);
      precision = 0.01; 
      thds_per_node = 10;
   }
   if ( argc >= 5) { precision = atof(argv[4]); }
   if ( argc >= 6) { thds_per_node = atof(argv[5]); }

   if (rank == 0) {
      double starttime, endtime;
      struct integralApprox buff;
      double final_value;
      int tot_num_strips = 0;
      int nodesReceived = 0;

      starttime = MPI_Wtime();

      for (active_node = 1; active_node < total; active_node++) {
         msgsend(&buff, active_node);
      }
      while (nodesReceived < total-1) {
         msgrecv(&buff, MPI_ANY_SOURCE);
         nodesReceived++;
         printf("Proccess %d returned value %f with numstrips %d\n", status.MPI_SOURCE, buff.value, buff.num_strips);
         final_value += buff.value;
         tot_num_strips += buff.num_strips;
         /* printf("%d out of %d\n", nodesReceived, total-1); */
      }

      endtime = MPI_Wtime();
      printf("Value: %f, Total Num Strips: %d\n", final_value, tot_num_strips);
      printf("Time taken: %f seconds\n", endtime - starttime);
   }

   else {
      struct integralApprox result;
      msgrecv(&result, 0); 
      result = threaded_integrate(funcArr[fn_num], 
        start + (rank - 1) * node_portion,
        start + (rank) * node_portion,
        precision,
        thds_per_node);
      /* printf("node: %d, value: %f, numstrips: %d\n", rank, result.value, result.num_strips); */
      msgsend(&result, 0);
   }

   fflush(stdout);
   MPI_Finalize();
   pthread_exit(NULL);
   return 0;
}

double fn0(double x) {
   return 1;
}

double fn1(double x) {
   return x;
}

double fn2(double x) {
   return x+5;
}

double fn3(double x) {
   return 2 * pow(x, 2) + 9 * x + 4;
}
