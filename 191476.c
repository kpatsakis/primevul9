static void ConvertRGBToYPbPr(const double red,const double green,
  const double blue,double *Y,double *Pb,double *Pr)
{
  *Y=QuantumScale*(0.298839*red+0.586811*green+0.114350*blue);
  *Pb=QuantumScale*((-0.1687367)*red-0.331264*green+0.5*blue)+0.5;
  *Pr=QuantumScale*(0.5*red-0.418688*green-0.081312*blue)+0.5;
}