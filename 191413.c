static void ConvertRGBToProPhoto(const double red,const double green,
  const double blue,double *r,double *g,double *b)
{
  double
    X,
    Y,
    Z;

  ConvertRGBToXYZ(red,green,blue,&X,&Y,&Z);
  ConvertXYZToProPhoto(X,Y,Z,r,g,b);
}