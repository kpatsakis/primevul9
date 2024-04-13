static void ConvertJzazbzToRGB(const double Jz,const double az,
  const double bz,const double white_luminance,double *red,double *green,
  double *blue)
{
  double
    X,
    Y,
    Z;

  ConvertJzazbzToXYZ(Jz,az,bz,white_luminance,&X,&Y,&Z);
  ConvertXYZToRGB(X,Y,Z,red,blue,green);
}