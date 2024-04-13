static inline void ConvertProPhotoToRGB(const double r,const double g,
  const double b,double *red,double *green,double *blue)
{
  double
    X,
    Y,
    Z;

  ConvertProPhotoToXYZ(r,g,b,&X,&Y,&Z);
  ConvertXYZToRGB(X,Y,Z,red,green,blue);
}