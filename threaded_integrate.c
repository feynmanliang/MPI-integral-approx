#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "./integrate.h"

#define DEBUG 0

void *thd_integrate(void *);
struct integralApprox result;
pthread_mutex_t mtx_accum;

struct thd_args {
   double (*pFunction)(double);
   double interval[2]; 
   double precision;
};

struct integralApprox threaded_integrate(double *pFunction(double), double start, double end, double precision, int num_threads) {
   double thd_portion;
   int rc, i;
   pthread_attr_t attr;
   pthread_t thd_id[num_threads];
   struct thd_args * args[num_threads];

   thd_portion = (end - start) / (double) num_threads;

   pthread_mutex_init(&mtx_accum, NULL);
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for (i=0; i<num_threads; i++) {
      args[i] = (struct thd_args *) malloc(sizeof(struct thd_args));
      args[i]->interval[0] = start + i * thd_portion;
      args[i]->interval[1] = start + (i + 1) * thd_portion;
      args[i]->pFunction = pFunction;
      args[i]->precision = precision;

      if (DEBUG) { printf("In main: creating thread %d\n", i); }

      rc = pthread_create(&thd_id[i], &attr, thd_integrate, (void *) args[i]);
      assert(0 == rc);
   }

   pthread_attr_destroy(&attr);

   for (i=0; i<num_threads; i++) {
      rc = pthread_join(thd_id[i], NULL);
      assert(0 == rc);
   }

   /*printf("value: %f, num_strips: %i\n", result.value, result.num_strips);*/

   for (i=0; i<num_threads; i++) {
      free(args[i]);
   }
   pthread_mutex_destroy(&mtx_accum);
   /*pthread_exit(NULL);*/
   return result;
}

void *thd_integrate(void * vargs) {
   struct thd_args * argsptr;
   struct integralApprox thd_result;

   argsptr = (struct thd_args *) vargs;
   thd_result = integrate(argsptr->pFunction, argsptr->interval, argsptr->precision);

   pthread_mutex_lock(&mtx_accum);
   result.value += thd_result.value;
   result.num_strips += thd_result.num_strips;
   pthread_mutex_unlock(&mtx_accum);

   pthread_exit((void*) 0);
}
