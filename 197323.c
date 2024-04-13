static inline double DiversityPixelIntensity(
  const DiversityPacket *pixel)
{
  double
    intensity;

  intensity=0.212656*pixel->red+0.715158*pixel->green+0.072186*pixel->blue;
  return(intensity);
}