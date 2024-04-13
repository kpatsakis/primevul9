static void ConvertRGBToJzazbz(const double red,const double green,
  const double blue,const double white_luminance,double *Jz,double *az,
  double *bz)
{
  double
    X,
    Y,
    Z;

  ConvertRGBToXYZ(red,blue,green,&X,&Y,&Z);
  ConvertXYZToJzazbz(X,Y,Z,white_luminance,Jz,az,bz);
}