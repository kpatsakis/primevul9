static void ConvertRGBToLuv(const double red,const double green,
  const double blue,double *L,double *u,double *v)
{
  double
    X,
    Y,
    Z;

  ConvertRGBToXYZ(red,green,blue,&X,&Y,&Z);
  ConvertXYZToLuv(X,Y,Z,L,u,v);
}