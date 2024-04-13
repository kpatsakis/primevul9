static inline void SplineWeights(const double x,double (*weights)[4])
{
  double
    alpha,
    beta;

  /*
    Nicolas Robidoux' 12 flops (6* + 5- + 1+) refactoring of the computation
    of the standard four 1D cubic B-spline smoothing weights. The sampling
    location is assumed between the second and third input pixel locations,
    and x is the position relative to the second input pixel location.
  */
  alpha=(double) 1.0-x;
  (*weights)[3]=(double) (1.0/6.0)*x*x*x;
  (*weights)[0]=(double) (1.0/6.0)*alpha*alpha*alpha;
  beta=(*weights)[3]-(*weights)[0];
  (*weights)[1]=alpha-(*weights)[0]+beta;
  (*weights)[2]=x-(*weights)[3]-beta;
}