static void ConvertYCbCrToRGB(const double Y,const double Cb,
  const double Cr,double *red,double *green,double *blue)
{
  ConvertYPbPrToRGB(Y,Cb,Cr,red,green,blue);
}