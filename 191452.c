static inline void ConvertDisplayP3ToRGB(const double r,const double g,
  const double b,double *red,double *green,double *blue)
{
  double
    X,
    Y,
    Z;

  ConvertDisplayP3ToXYZ(r,g,b,&X,&Y,&Z);
  ConvertXYZToRGB(X,Y,Z,red,green,blue);
}