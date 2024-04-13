static void ConvertRGBToxyY(const double red,const double green,
  const double blue,double *low_x,double *low_y,double *cap_Y)
{
  double
    gamma,
    X,
    Y,
    Z;

  ConvertRGBToXYZ(red,green,blue,&X,&Y,&Z);
  gamma=PerceptibleReciprocal(X+Y+Z);
  *low_x=gamma*X;
  *low_y=gamma*Y;
  *cap_Y=Y;
}