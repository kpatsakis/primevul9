static void ConvertRGBToYIQ(const double red,const double green,
  const double blue,double *Y,double *I,double *Q)
{
  *Y=QuantumScale*(0.298839*red+0.586811*green+0.114350*blue);
  *I=QuantumScale*(0.595716*red-0.274453*green-0.321263*blue)+0.5;
  *Q=QuantumScale*(0.211456*red-0.522591*green+0.311135*blue)+0.5;
}