static void ConvertRGBToYUV(const double red,const double green,
  const double blue,double *Y,double *U,double *V)
{
  *Y=QuantumScale*(0.298839*red+0.586811*green+0.114350*blue);
  *U=QuantumScale*((-0.147)*red-0.289*green+0.436*blue)+0.5;
  *V=QuantumScale*(0.615*red-0.515*green-0.100*blue)+0.5;
}