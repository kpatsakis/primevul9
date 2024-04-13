static inline void ConvertRGBToCMY(const double red,const double green,
  const double blue,double *cyan,double *magenta,double *yellow)
{
  *cyan=QuantumScale*(QuantumRange-red);
  *magenta=QuantumScale*(QuantumRange-green);
  *yellow=QuantumScale*(QuantumRange-blue);
}