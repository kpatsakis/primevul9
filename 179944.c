MagickExport MagickRealType EncodePixelGamma(const MagickRealType pixel)
{
  if (pixel <= (0.0031306684425005883*QuantumRange))
    return(12.92f*pixel);
  return((MagickRealType) QuantumRange*(1.055*EncodeGamma((double) QuantumScale*
    pixel)-0.055));
}