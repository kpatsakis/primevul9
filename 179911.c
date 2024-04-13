MagickExport MagickRealType DecodePixelGamma(const MagickRealType pixel)
{
  if (pixel <= (0.0404482362771076*QuantumRange))
    return(pixel/12.92f);
  return((MagickRealType) (QuantumRange*DecodeGamma((double) (QuantumScale*
    pixel+0.055)/1.055)));
}