static inline void ConvertLabToRGB(const double L,const double a,
  const double b,double *red,double *green,double *blue)
{
  double
    X,
    Y,
    Z;

  ConvertLabToXYZ(100.0*L,255.0*(a-0.5),255.0*(b-0.5),&X,&Y,&Z);
  ConvertXYZToRGB(X,Y,Z,red,green,blue);
}