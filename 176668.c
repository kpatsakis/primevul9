static inline void ConvertXYZToRGB(const double X,const double Y,const double Z,
  Quantum *red,Quantum *green,Quantum *blue)
{
  double
    b,
    g,
    r;

  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  r=3.2404542*X-1.5371385*Y-0.4985314*Z;
  g=(-0.9692660)*X+1.8760108*Y+0.0415560*Z;
  b=0.0556434*X-0.2040259*Y+1.0572252*Z;
  *red=ClampToQuantum((MagickRealType) EncodePixelGamma(QuantumRange*r));
  *green=ClampToQuantum((MagickRealType) EncodePixelGamma(QuantumRange*g));
  *blue=ClampToQuantum((MagickRealType) EncodePixelGamma(QuantumRange*b));
}