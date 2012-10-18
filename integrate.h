#define public
#define private static

public struct integralApprox {
  double value;
  int num_strips;
};

public struct integralApprox integrate(double pFunction(double), double interval[], double precision);
