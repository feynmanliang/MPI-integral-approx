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
  MPI_Send(buffp, sizeof(buffp), MPI_BYTE, to, MYTAG, MPI_COMM_WORLD);

#define msgrecv(buffp, from) \
  MPI_Recv(buffp, sizeof(buffp), MPI_BYTE, from, MYTAG, MPI_COMM_WORLD, &status);

#define msgrecv(buffp, from) \
  MPI_Recv(buffp, sizeof(buffp), MPI_BYTE, from, MYTAG, MPI_COMM_WORLD, &status);

#define msgrecv(buffp, from) \
  MPI_Recv(buffp, sizeof(buffp), MPI_BYTE, from, MYTAG, MPI_COMM_WORLD, &status);

double fn0(double);
double fn1(double);
double fn2(double);
double fn3(double);

int main(int argc, char *argv[]) {
   double start;
   double end;
   double node_portion;
   double (*funcArr[3])(double);
   int fn_num;
   double precision;
   int thds_per_node;

   int rank, total;
   int active_node;
   struct integralApprox buff;
   MPI_Status status;

   msginit(&argc,&argv,&rank,&total);

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
      double final_value;
      int tot_num_strips = 0;
      int nodesReceived = 0;

      for (active_node = 1; active_node < total; active_node++) {
         msgsend(&buff, active_node);
      }
      while (nodesReceived < total-1) {
         msgrecv(&buff, MPI_ANY_SOURCE);
         printf("Proccess %d returned value %f with numstrips %d\n", status.MPI_SOURCE, buff.value, buff.num_strips);
         final_value += buff.value;
         tot_num_strips += buff.num_strips;
         printf("%d out of %d\n", ++nodesReceived, total-1);
      }
      printf("value: %f, tot_num_strips: %d\n", final_value, tot_num_strips);
   }

   else {
      msgrecv(&buff, 0); 
      buff = threaded_integrate(funcArr[fn_num], 
        start + (rank - 1) * node_portion,
        start + (rank) * node_portion,
        precision,
        thds_per_node);
      printf("node: %d, value: %f, numstrips: %d\n", rank, buff.value, buff.num_strips);
      msgsend(&buff, 0); 
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
