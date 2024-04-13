static inline void ConvertxyYToRGB(const double low_x,const double low_y,
  const double cap_Y,double *red,double *green,double *blue)
{
  double
    gamma,
    X,
    Y,
    Z;

  gamma=PerceptibleReciprocal(low_y);
  X=gamma*cap_Y*low_x;
  Y=cap_Y;
  Z=gamma*cap_Y*(1.0-low_x-low_y);
  ConvertXYZToRGB(X,Y,Z,red,green,blue);
}