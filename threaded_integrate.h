#define public
#define private static

public struct integralApprox threaded_integrate(
  double pFunction(double), double start, double end, double precision, int num_threads);
