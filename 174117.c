static inline double LevelPixel(const double black_point,
  const double white_point,const double gamma,const double pixel)
{
  double
    level_pixel,
    scale;

  if (fabs(white_point-black_point) < MagickEpsilon)
    return(pixel);
  scale=1.0/(white_point-black_point);
  level_pixel=QuantumRange*gamma_pow(scale*((double) pixel-black_point),
    1.0/gamma);
  return(level_pixel);
}