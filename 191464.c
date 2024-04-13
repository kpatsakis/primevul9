static inline void ConvertCMYToRGB(const double cyan,const double magenta,
  const double yellow,double *red,double *green,double *blue)
{
  *red=QuantumRange*(1.0-cyan);
  *green=QuantumRange*(1.0-magenta);
  *blue=QuantumRange*(1.0-yellow);
}