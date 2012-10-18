#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct integralApprox {
  double value;
  int num_strips;
};

struct integralApprox integrate(double pFunction(double), double interval[], double precision) {
  struct integralApprox result;
  int start;
  int end;
  double prev_value;
  double current_value;
  int num_strips;	
  double width;
  int i;
  double base1;
  double base2;

  start = interval[0];
  end = interval[1];
  current_value = 0;
  num_strips = 0;
  do {
    prev_value = current_value;
    current_value = 0;
    num_strips++;
    
    width = (end - start) / (double) num_strips;
    for (i = 0; i < num_strips; i++) {
      base1 = pFunction(start + i * width);
      base2 = pFunction(start + (i + 1) * width);
      current_value += ((base1 + base2) * width ) / 2;
    }
  } while (fabs(current_value - prev_value) > precision);
  result.value = current_value;
  result.num_strips = num_strips;
  return result;
}
