static inline void ConvertLuvToRGB(const double L,const double u,
  const double v,double *red,double *green,double *blue)
{
  double
    X,
    Y,
    Z;

  ConvertLuvToXYZ(100.0*L,354.0*u-134.0,262.0*v-140.0,&X,&Y,&Z);
  ConvertXYZToRGB(X,Y,Z,red,green,blue);
}