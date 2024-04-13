static inline void ConvertRGBToXYZ(const Quantum red,const Quantum green,
  const Quantum blue,double *X,double *Y,double *Z)
{
  double
    b,
    g,
    r;

  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  r=QuantumScale*DecodePixelGamma((MagickRealType) red);
  g=QuantumScale*DecodePixelGamma((MagickRealType) green);
  b=QuantumScale*DecodePixelGamma((MagickRealType) blue);
  *X=0.4124564*r+0.3575761*g+0.1804375*b;
  *Y=0.2126729*r+0.7151522*g+0.0721750*b;
  *Z=0.0193339*r+0.1191920*g+0.9503041*b;
}