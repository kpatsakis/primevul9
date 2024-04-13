static void ConvertRGBToYDbDr(const double red,const double green,
  const double blue,double *Y,double *Db,double *Dr)
{
  *Y=QuantumScale*(0.298839*red+0.586811*green+0.114350*blue);
  *Db=QuantumScale*(-0.450*red-0.883*green+1.333*blue)+0.5;
  *Dr=QuantumScale*(-1.333*red+1.116*green+0.217*blue)+0.5;
}