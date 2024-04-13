static inline MagickRealType DiversityPixelIntensity(
  const DiversityPacket *pixel)
{
  MagickRealType
    intensity;

  intensity=0.298839*pixel->red+0.586811*pixel->green+0.114350*pixel->blue;
  return(intensity);
}