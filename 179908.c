static inline void CatromWeights(const double x,double (*weights)[4])
{
  double
    alpha,
    beta,
    gamma;

  /*
    Nicolas Robidoux' 10 flops (4* + 5- + 1+) refactoring of the computation
    of the standard four 1D Catmull-Rom weights. The sampling location is
    assumed between the second and third input pixel locations, and x is the
    position relative to the second input pixel location. Formulas originally
    derived for the VIPS (Virtual Image Processing System) library.
  */
  alpha=(double) 1.0-x;
  beta=(double) (-0.5)*x*alpha;
  (*weights)[0]=alpha*beta;
  (*weights)[3]=x*beta;
  /*
    The following computation of the inner weights from the outer ones work
    for all Keys cubics.
  */
  gamma=(*weights)[3]-(*weights)[0];
  (*weights)[1]=alpha-(*weights)[0]+gamma;
  (*weights)[2]=x-(*weights)[3]-gamma;
}