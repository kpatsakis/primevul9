static void ConvertRGBToYCbCr(const double red,const double green,
  const double blue,double *Y,double *Cb,double *Cr)
{
  ConvertRGBToYPbPr(red,green,blue,Y,Cb,Cr);
}